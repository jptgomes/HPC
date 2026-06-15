#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define GENOME_BITS 16
#define VARIANT_SPACE (1 << GENOME_BITS)

typedef struct {
    uint64_t state;
} RNG;

typedef struct {
    double attack_rate;
    double peak_frac;
    double peak_day;
    double variants;
    double extinct;
    double extinction_day;
} SimMetrics;

static inline uint64_t rng_u64(RNG *rng) {
    uint64_t x = rng->state;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    rng->state = x;
    return x;
}

static inline double rng_double(RNG *rng) {
    return (rng_u64(rng) >> 11) * (1.0 / 9007199254740992.0);
}

static inline int rng_int(RNG *rng, int max) {
    return (int)(rng_u64(rng) % (uint64_t)max);
}

static inline int popcount16(int x) {
    return __builtin_popcount((unsigned int)x);
}

static inline double variant_transmissibility(int variant) {
    int mutations = popcount16(variant);
    return 1.0 + 0.015 * mutations;
}

static inline int mutate_variant(int variant, RNG *rng) {
    int bit = rng_int(rng, GENOME_BITS);
    return variant ^ (1 << bit);
}

SimMetrics run_simulation(
    int population,
    int days,
    double beta,
    double recovery_prob,
    double mutation_prob,
    int initial_infected,
    RNG *rng,
    unsigned char *state,
    int *variant,
    int *infected_idx,
    unsigned char *seen_variant
) {
    memset(state, 0, population * sizeof(unsigned char));
    memset(seen_variant, 0, VARIANT_SPACE * sizeof(unsigned char));

    if (initial_infected > population) {
        initial_infected = population;
    }

    int ever_infected = 0;
    int distinct_variants = 1;

    seen_variant[0] = 1;

    while (ever_infected < initial_infected) {
        int p = rng_int(rng, population);
        if (state[p] == 0) {
            state[p] = 1;
            variant[p] = 0;
            ever_infected++;
        }
    }

    int peak_infected = ever_infected;
    int peak_day = 0;
    int extinct = 0;
    int extinction_day = days;

    for (int day = 0; day < days; day++) {
        int infected_count = 0;
        double trans_sum = 0.0;

        for (int i = 0; i < population; i++) {
            if (state[i] == 1) {
                infected_idx[infected_count++] = i;
                trans_sum += variant_transmissibility(variant[i]);
            }
        }

        if (infected_count > peak_infected) {
            peak_infected = infected_count;
            peak_day = day;
        }

        if (infected_count == 0) {
            extinct = 1;
            extinction_day = day;
            break;
        }

        double avg_trans = trans_sum / infected_count;
        double force = beta * avg_trans * ((double)infected_count / (double)population);
        double infection_prob = 1.0 - exp(-force);

        if (infection_prob > 0.95) {
            infection_prob = 0.95;
        }

        for (int i = 0; i < population; i++) {
            if (state[i] == 0) {
                if (rng_double(rng) < infection_prob) {
                    int parent_pos = infected_idx[rng_int(rng, infected_count)];
                    int new_variant = variant[parent_pos];

                    if (rng_double(rng) < mutation_prob) {
                        new_variant = mutate_variant(new_variant, rng);

                        if (!seen_variant[new_variant]) {
                            seen_variant[new_variant] = 1;
                            distinct_variants++;
                        }
                    }

                    state[i] = 3;
                    variant[i] = new_variant;
                    ever_infected++;
                }
            }
        }

        for (int j = 0; j < infected_count; j++) {
            int idx = infected_idx[j];

            if (rng_double(rng) < mutation_prob) {
                int new_variant = mutate_variant(variant[idx], rng);
                variant[idx] = new_variant;

                if (!seen_variant[new_variant]) {
                    seen_variant[new_variant] = 1;
                    distinct_variants++;
                }
            }

            if (rng_double(rng) < recovery_prob) {
                state[idx] = 2;
            }
        }

        for (int i = 0; i < population; i++) {
            if (state[i] == 3) {
                state[i] = 1;
            }
        }
    }

    SimMetrics m;
    m.attack_rate = (double)ever_infected / (double)population;
    m.peak_frac = (double)peak_infected / (double)population;
    m.peak_day = (double)peak_day;
    m.variants = (double)distinct_variants;
    m.extinct = (double)extinct;
    m.extinction_day = (double)extinction_day;

    return m;
}

void print_usage(char *prog) {
    printf("\nUso:\n");
    printf("  %s total_simulacoes populacao dias beta recuperacao mutacao infectados_iniciais\n\n", prog);
    printf("Exemplo:\n");
    printf("  mpirun -np 4 %s 40 20000 160 0.35 0.08 0.0005 10\n\n", prog);
}

int main(int argc, char **argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int total_sims = 100;
    int population = 50000;
    int days = 180;
    double beta = 0.35;
    double recovery_prob = 0.08;
    double mutation_prob = 0.0005;
    int initial_infected = 10;

    if (argc == 2) {
        if (rank == 0) {
            print_usage(argv[0]);
        }
        MPI_Finalize();
        return 0;
    }

    if (argc >= 2) total_sims = atoi(argv[1]);
    if (argc >= 3) population = atoi(argv[2]);
    if (argc >= 4) days = atoi(argv[3]);
    if (argc >= 5) beta = atof(argv[4]);
    if (argc >= 6) recovery_prob = atof(argv[5]);
    if (argc >= 7) mutation_prob = atof(argv[6]);
    if (argc >= 8) initial_infected = atoi(argv[7]);

    if (total_sims < 1) total_sims = 1;
    if (population < 100) population = 100;
    if (days < 1) days = 1;
    if (beta < 0.0) beta = 0.0;
    if (recovery_prob < 0.0) recovery_prob = 0.0;
    if (recovery_prob > 1.0) recovery_prob = 1.0;
    if (mutation_prob < 0.0) mutation_prob = 0.0;
    if (mutation_prob > 1.0) mutation_prob = 1.0;
    if (initial_infected < 1) initial_infected = 1;

    int base = total_sims / size;
    int rest = total_sims % size;
    int local_sims = base + (rank < rest ? 1 : 0);

    unsigned char *state = malloc(population * sizeof(unsigned char));
    int *variant = malloc(population * sizeof(int));
    int *infected_idx = malloc(population * sizeof(int));
    unsigned char *seen_variant = malloc(VARIANT_SPACE * sizeof(unsigned char));

    if (!state || !variant || !infected_idx || !seen_variant) {
        fprintf(stderr, "Rank %d: erro de memoria.\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    RNG rng;
    rng.state = 88172645463325252ULL;
    rng.state ^= (uint64_t)(rank + 1) * 0x9E3779B97F4A7C15ULL;
    rng.state ^= (uint64_t)population * 0xBF58476D1CE4E5B9ULL;
    rng.state ^= (uint64_t)days * 0x94D049BB133111EBULL;

    double local_attack = 0.0;
    double local_peak = 0.0;
    double local_peak_day = 0.0;
    double local_variants = 0.0;
    double local_extinct = 0.0;
    double local_extinction_day = 0.0;

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    for (int s = 0; s < local_sims; s++) {
        SimMetrics m = run_simulation(
            population,
            days,
            beta,
            recovery_prob,
            mutation_prob,
            initial_infected,
            &rng,
            state,
            variant,
            infected_idx,
            seen_variant
        );

        local_attack += m.attack_rate;
        local_peak += m.peak_frac;
        local_peak_day += m.peak_day;
        local_variants += m.variants;
        local_extinct += m.extinct;
        local_extinction_day += m.extinction_day;
    }

    double t1 = MPI_Wtime();
    double local_time = t1 - t0;
    double max_time = 0.0;

    MPI_Reduce(&local_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    double global_attack = 0.0;
    double global_peak = 0.0;
    double global_peak_day = 0.0;
    double global_variants = 0.0;
    double global_extinct = 0.0;
    double global_extinction_day = 0.0;

    int global_sims_done = 0;

    MPI_Reduce(&local_attack, &global_attack, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_peak, &global_peak, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_peak_day, &global_peak_day, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_variants, &global_variants, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_extinct, &global_extinct, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_extinction_day, &global_extinction_day, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_sims, &global_sims_done, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        double avg_attack = global_attack / global_sims_done;
        double avg_peak = global_peak / global_sims_done;
        double avg_peak_day = global_peak_day / global_sims_done;
        double avg_variants = global_variants / global_sims_done;
        double extinction_rate = global_extinct / global_sims_done;
        double avg_extinction_day = global_extinction_day / global_sims_done;

        double person_day_updates = (double)global_sims_done * (double)population * (double)days;
        double updates_per_second = person_day_updates / max_time;

        printf("\n============================================================\n");
        printf(" VirusDNA-MPI: Monte Carlo de propagacao viral e mutacao\n");
        printf("============================================================\n\n");

        printf("Processos MPI:              %d\n", size);
        printf("Simulacoes totais:           %d\n", global_sims_done);
        printf("Populacao por simulacao:     %d\n", population);
        printf("Dias por simulacao:          %d\n", days);
        printf("Infectados iniciais:         %d\n", initial_infected);
        printf("Beta base:                   %.6f\n", beta);
        printf("Probabilidade de recuperacao %.6f\n", recovery_prob);
        printf("Probabilidade de mutacao:    %.8f\n", mutation_prob);
        printf("Genoma abstrato:             %d bits\n", GENOME_BITS);

        printf("\n------------------- Resultados medios ----------------------\n");
        printf("Taxa media infectada:        %.2f %%\n", avg_attack * 100.0);
        printf("Pico medio de infectados:    %.2f %% da populacao\n", avg_peak * 100.0);
        printf("Dia medio do pico:           %.2f\n", avg_peak_day);
        printf("Variantes medias geradas:    %.2f\n", avg_variants);
        printf("Taxa de extincao:            %.2f %%\n", extinction_rate * 100.0);
        printf("Dia medio de extincao:       %.2f\n", avg_extinction_day);

        printf("\n------------------- Desempenho -----------------------------\n");
        printf("Tempo total:                 %.6f s\n", max_time);
        printf("Atualizacoes pessoa-dia:     %.0f\n", person_day_updates);
        printf("Atualizacoes por segundo:    %.2f\n", updates_per_second);
        printf("Simulacoes por segundo:      %.2f\n", global_sims_done / max_time);

        printf("============================================================\n\n");
    }

    free(state);
    free(variant);
    free(infected_idx);
    free(seen_variant);

    MPI_Finalize();
    return 0;
}
