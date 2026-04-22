#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define TAG_CONTAGENS 10

/* Mistura determinística simples para gerar bases sem precisar armazenar a sequência inteira */
unsigned long long mix64(unsigned long long x) {
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccdULL;
    x ^= x >> 33;
    x *= 0xc4ceb9fe1a85ec53ULL;
    x ^= x >> 33;
    return x;
}

/* Gera base sintética determinística para a posição idx */
char base_at(unsigned long long idx) {
    static const char bases[4] = {'A', 'C', 'G', 'T'};
    return bases[mix64(idx) % 4];
}

int main(int argc, char *argv[]) {
    int rank, size;
    unsigned long long N, base, resto, local_n, local_start, local_end;
    unsigned long long contagens[5] = {0, 0, 0, 0, 0}; /* A, C, G, T, motivo */
    unsigned long long totais[5]    = {0, 0, 0, 0, 0};
    char *motivo;
    int m;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 3) {
        if (rank == 0) {
            printf("Uso: %s <tamanho_sequencia> <motivo>\n", argv[0]);
            printf("Exemplo: %s 200000000 ATG\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    N = strtoull(argv[1], NULL, 10);
    motivo = argv[2];
    m = (int)strlen(motivo);

    if (N == 0 || m <= 0) {
        if (rank == 0) {
            printf("Erro: parametros invalidos.\n");
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
        char b = base_at(i);

        switch (b) {
            case 'A': contagens[0]++; break;
            case 'C': contagens[1]++; break;
            case 'G': contagens[2]++; break;
            case 'T': contagens[3]++; break;
        }

        /* Conta ocorrências do motivo começando nesta posição */
        if (i + (unsigned long long)m <= N) {
            int ok = 1;
            for (int j = 0; j < m; j++) {
                if (base_at(i + j) != motivo[j]) {
                    ok = 0;
                    break;
                }
            }
            if (ok) {
                contagens[4]++;
            }
        }
    }

    if (rank == 0) {
        for (int k = 0; k < 5; k++) {
            totais[k] = contagens[k];
        }

        for (int origem = 1; origem < size; origem++) {
            unsigned long long parcial[5];
            MPI_Recv(parcial, 5, MPI_UNSIGNED_LONG_LONG, origem, TAG_CONTAGENS,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for (int k = 0; k < 5; k++) {
                totais[k] += parcial[k];
            }
        }

        double t1 = MPI_Wtime();
        double gc = 100.0 * (double)(totais[1] + totais[2]) / (double)N;

        printf("=============================================\n");
        printf("Analise paralela de DNA (sequencia sintetica)\n");
        printf("Tamanho da sequencia : %llu\n", N);
        printf("Motivo pesquisado    : %s\n", motivo);
        printf("Processos            : %d\n", size);
        printf("A                    : %llu\n", totais[0]);
        printf("C                    : %llu\n", totais[1]);
        printf("G                    : %llu\n", totais[2]);
        printf("T                    : %llu\n", totais[3]);
        printf("Ocorrencias do motivo: %llu\n", totais[4]);
        printf("GC%%                  : %.4f\n", gc);
        printf("Tempo (s)            : %.6f\n", t1 - t0);
        printf("=============================================\n");
    } else {
        MPI_Send(contagens, 5, MPI_UNSIGNED_LONG_LONG, 0, TAG_CONTAGENS, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
