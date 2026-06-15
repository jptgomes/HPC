#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cblas.h>

#define N 700

double tempo_atual() {
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return t.tv_sec + t.tv_nsec / 1e9;
}

void preencher(double *m) {
	for (int i = 0; i < N * N; i++) {
	      m[i] = 1.0;
	}
}

void multiplicacao_manual(double *A, double *B, double *C) {
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

int main() {
	double *A = malloc(N * N * sizeof(double));
	double *B = malloc(N * N * sizeof(double));
	double *C1 = malloc(N * N * sizeof(double));
	double *C2 = malloc(N * N * sizeof(double));

	if (!A || !B || !C1 || !C2) {
	printf("Erro ao alocar memória.\n");
	return 1;
	}

	preencher(A);
	preencher(B);

	double inicio, fim;

	inicio = tempo_atual();

	multiplicacao_manual(A, B, C1);

	fim = tempo_atual();

	printf("Tempo multiplicação manual: %.6f segundos\n", fim - inicio);

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

	printf("Tempo OpenBLAS: %.6f segundos\n", fim - inicio);

	printf("Valor de verificação C1[0] = %.2f\n", C1[0]);
	printf("Valor de verificação C2[0] = %.2f\n", C2[0]);

	free(A);
	free(B);
	free(C1);
	free(C2);

	return 0;
}
