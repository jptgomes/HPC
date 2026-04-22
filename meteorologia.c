#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define TAG_STATS 20
#define TAG_ALERTAS 21
#define PI 3.14159265358979323846

unsigned long long mix64(unsigned long long x) {
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccdULL;
    x ^= x >> 33;
    x *= 0xc4ceb9fe1a85ec53ULL;
    x ^= x >> 33;
    return x;
}

double rand01(unsigned long long x) {
    return (mix64(x) & 0xFFFFFFFFULL) / 4294967295.0;
}

int main(int argc, char *argv[]) {
    int rank, size;
    unsigned long long N, base, resto, local_n, local_start, local_end;
    double stats_local[4] = {0.0, 0.0, 0.0, -1e30};
    double stats_total[4] = {0.0, 0.0, 0.0, -1e30};
    unsigned long long alertas_local = 0, alertas_total = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0) {
            printf("Uso: %s <numero_de_registros>\n", argv[0]);
            printf("Exemplo: %s 10000000\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    N = strtoull(argv[1], NULL, 10);

    if (N == 0) {
        if (rank == 0) {
            printf("Erro: numero de registros invalido.\n");
        }
        MPI_Finalize();
        return 1;
    }

    base = N / size;
    resto = N % size;

    local_n = base + (rank < resto ? 1 : 0);
    local_start = rank * base + (rank < resto ? rank : resto);
    local_end = local_start + local_n;

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    for (unsigned long long i = local_start; i < local_end; i++) {
        double hora = (double)(i % 24ULL);
        double dia = (double)(i / 24ULL);

        double ruido1 = rand01(4ULL * i + 1ULL) - 0.5;
        double ruido2 = rand01(4ULL * i + 2ULL) - 0.5;
        double ruido3 = rand01(4ULL * i + 3ULL);
        double ruido4 = rand01(4ULL * i + 4ULL);

        /* Série sintética didática */
        double temp = 24.0
                    + 6.0 * sin(2.0 * PI * hora / 24.0)
                    + 3.0 * sin(2.0 * PI * dia / 30.0)
                    + 2.0 * ruido1;

        double umid = 65.0
                    - 15.0 * sin(2.0 * PI * hora / 24.0)
                    + 10.0 * ruido2;

        if (umid < 20.0) umid = 20.0;
        if (umid > 100.0) umid = 100.0;

        double vento = 3.0
                     + 4.0 * fabs(sin(2.0 * PI * dia / 7.0))
                     + 3.0 * ruido3;

        double chuva = 0.0;
        double chance_chuva = 0.02 + 0.18 * (umid / 100.0);

        if (umid > 70.0 && ruido4 < chance_chuva) {
            chuva = 5.0 + 25.0 * rand01(8ULL * i + 7ULL);
        }

        stats_local[0] += temp;   /* soma temperatura */
        stats_local[1] += umid;   /* soma umidade */
        stats_local[2] += chuva;  /* chuva acumulada */

        if (vento > stats_local[3]) {
            stats_local[3] = vento; /* vento maximo */
        }

        if (temp > 35.0 || vento > 10.0 || chuva > 20.0) {
            alertas_local++;
        }
    }

    if (rank == 0) {
        for (int k = 0; k < 4; k++) {
            stats_total[k] = stats_local[k];
        }
        alertas_total = alertas_local;

        for (int origem = 1; origem < size; origem++) {
            double parcial[4];
            unsigned long long alertas_parciais;

            MPI_Recv(parcial, 4, MPI_DOUBLE, origem, TAG_STATS,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&alertas_parciais, 1, MPI_UNSIGNED_LONG_LONG, origem, TAG_ALERTAS,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            stats_total[0] += parcial[0];
            stats_total[1] += parcial[1];
            stats_total[2] += parcial[2];

            if (parcial[3] > stats_total[3]) {
                stats_total[3] = parcial[3];
            }

            alertas_total += alertas_parciais;
        }

        double t1 = MPI_Wtime();

        printf("====================================================\n");
        printf("Analise paralela de dados meteorologicos sinteticos\n");
        printf("Registros analisados     : %llu\n", N);
        printf("Processos               : %d\n", size);
        printf("Temperatura media (C)   : %.4f\n", stats_total[0] / (double)N);
        printf("Umidade media (%%)       : %.4f\n", stats_total[1] / (double)N);
        printf("Chuva acumulada (mm)    : %.4f\n", stats_total[2]);
        printf("Vento maximo (m/s)      : %.4f\n", stats_total[3]);
        printf("Numero de alertas       : %llu\n", alertas_total);
        printf("Tempo (s)               : %.6f\n", t1 - t0);
        printf("====================================================\n");
    } else {
        MPI_Send(stats_local, 4, MPI_DOUBLE, 0, TAG_STATS, MPI_COMM_WORLD);
        MPI_Send(&alertas_local, 1, MPI_UNSIGNED_LONG_LONG, 0, TAG_ALERTAS, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
