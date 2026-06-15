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

void preencher_vetor(double *v, int n, double valor) {
    for (int i = 0; i < n; i++) {
        v[i] = valor;
    }
}

double produto_manual(double *a, double *b, int n) {
    double soma = 0.0;

    for (int i = 0; i < n; i++) {
        soma += a[i] * b[i];
    }

    return soma;
}

int main(int argc, char *argv[]) {
    int N = 20000000;
    int executar_manual = 1;

    if (argc >= 2) {
        N = atoi(argv[1]);
    }

    if (argc >= 3) {
        if (strcmp(argv[2], "blas") == 0) {
            executar_manual = 0;
        }
    }

    double memoria_mb = (2.0 * N * sizeof(double)) / (1024.0 * 1024.0);

    printf("=====================================\n");
    printf("Laboratorio OpenBLAS - Produto Escalar\n");
    printf("Tamanho dos vetores: %d elementos\n", N);
    printf("Memoria aproximada usada: %.2f MB\n", memoria_mb);
    printf("Modo: %s\n", executar_manual ? "manual + OpenBLAS" : "somente OpenBLAS");
    printf("=====================================\n");

    double *a = malloc(N * sizeof(double));
    double *b = malloc(N * sizeof(double));

    if (!a || !b) {
        printf("Erro ao alocar memoria.\n");
        return 1;
    }

    preencher_vetor(a, N, 1.0);
    preencher_vetor(b, N, 2.0);

    double inicio, fim;
    double resultado_manual = 0.0;
    double resultado_blas = 0.0;
    double tempo_manual = 0.0;
    double tempo_blas = 0.0;

    if (executar_manual) {
        inicio = tempo_atual();

        resultado_manual = produto_manual(a, b, N);

        fim = tempo_atual();

        tempo_manual = fim - inicio;
        printf("Tempo produto manual: %.6f segundos\n", tempo_manual);
        printf("Resultado manual = %.2f\n", resultado_manual);
    }

    inicio = tempo_atual();

    resultado_blas = cblas_ddot(N, a, 1, b, 1);

    fim = tempo_atual();

    tempo_blas = fim - inicio;

    double operacoes = 2.0 * N;
    double gflops = operacoes / tempo_blas / 1e9;

    printf("Tempo OpenBLAS ddot: %.6f segundos\n", tempo_blas);
    printf("Resultado OpenBLAS = %.2f\n", resultado_blas);
    printf("Desempenho aproximado: %.4f GFLOPS\n", gflops);

    if (executar_manual) {
        printf("Ganho OpenBLAS sobre manual = %.2f x\n", tempo_manual / tempo_blas);
    }

    printf("Resultado esperado = %.2f\n", 2.0 * N);

    free(a);
    free(b);

    return 0;
}
