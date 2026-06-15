#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <cblas.h>

double tempo_atual() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

void preencher(double *m, long total) {
    for (long i = 0; i < total; i++) {
        m[i] = 1.0;
    }
}

void multiplicacao_manual(double *A, double *B, double *C, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double soma = 0.0;
            for (int k = 0; k < N; k++) {
                soma += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = soma;
        }
    }
}

int main(int argc, char *argv[]) {
    int N = 1000;
    int executar_manual = 1;

    if (argc >= 2) {
        N = atoi(argv[1]);
    }

    if (argc >= 3) {
        if (strcmp(argv[2], "blas") == 0) {
            executar_manual = 0;
        }
    }

    long total = (long) N * N;
    double memoria_mb = (4.0 * total * sizeof(double)) / (1024.0 * 1024.0);

    printf("=====================================\n");
    printf("Laboratorio OpenBLAS\n");
    printf("Tamanho da matriz: %d x %d\n", N, N);
    printf("Memoria aproximada usada: %.2f MB\n", memoria_mb);
    printf("Modo: %s\n", executar_manual ? "manual + OpenBLAS" : "somente OpenBLAS");
    printf("=====================================\n");

    double *A = malloc(total * sizeof(double));
    double *B = malloc(total * sizeof(double));
    double *C1 = malloc(total * sizeof(double));
    double *C2 = malloc(total * sizeof(double));

    if (!A || !B || !C1 || !C2) {
        printf("Erro ao alocar memoria.\n");
        return 1;
    }

    preencher(A, total);
    preencher(B, total);

    double inicio, fim;
    double tempo_manual = 0.0;
    double tempo_blas = 0.0;

    if (executar_manual) {
        inicio = tempo_atual();
        multiplicacao_manual(A, B, C1, N);
        fim = tempo_atual();

        tempo_manual = fim - inicio;
        printf("Tempo multiplicacao manual: %.6f segundos\n", tempo_manual);
    }

    inicio = tempo_atual();

    cblas_dgemm(
        CblasRowMajor,
        CblasNoTrans,
        CblasNoTrans,
        N, N, N,
        1.0,
        A, N,
        B, N,
        0.0,
        C2, N
    );

    fim = tempo_atual();

    tempo_blas = fim - inicio;

    double operacoes = 2.0 * N * N * N;
    double gflops = operacoes / tempo_blas / 1e9;

    printf("Tempo OpenBLAS: %.6f segundos\n", tempo_blas);
    printf("Desempenho OpenBLAS: %.2f GFLOPS\n", gflops);

    if (executar_manual) {
        printf("Valor de verificacao C1[0] = %.2f\n", C1[0]);
        printf("Ganho OpenBLAS sobre manual = %.2f x\n", tempo_manual / tempo_blas);
    }

    printf("Valor de verificacao C2[0] = %.2f\n", C2[0]);

    free(A);
    free(B);
    free(C1);
    free(C2);

    return 0;
}
