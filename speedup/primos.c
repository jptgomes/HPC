#include <mpi.h>
#include <stdio.h>
#include <math.h>

int eh_primo(int n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;

    int limite = (int)sqrt((double)n);
    for (int i = 3; i <= limite; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    int rank, size;
    int N = 300000;
    int cont_local = 0, cont_global = 0;
    double inicio, fim;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    inicio = MPI_Wtime();

    if (rank == 0 && N >= 2) {
        cont_local = 1; // conta o 2
    }

    for (int n = 3 + 2 * rank; n <= N; n += 2 * size) {
        if (eh_primo(n)) cont_local++;
    }

    MPI_Reduce(&cont_local, &cont_global, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    fim = MPI_Wtime();

    if (rank == 0) {
        printf("Quantidade de primos ate %d = %d\n", N, cont_global);
        printf("Tempo total                = %.6f segundos\n", fim - inicio);
    }

    MPI_Finalize();
    return 0;
}
