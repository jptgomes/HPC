#include <mpi.h>
#include <stdio.h>
int main(int argc, char *argv[]) {
    int rank, size;
    long long int i, n = 100000000;
    double h, sum = 0.0, x, pi, local_sum = 0.0;
    double start, end;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    h = 1.0 / (double)n;
    start = MPI_Wtime();
    for (i = rank; i < n; i += size) {
        x = h * ((double)i + 0.5);
        local_sum += 4.0 / (1.0 + x * x);
    }
    MPI_Reduce(&local_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    end = MPI_Wtime();
    if (rank == 0) {
        pi = h * sum;
        printf("Valor aproximado de pi: %.10f\n", pi);
        printf("Tempo de execucao: %f segundos\n", end - start);
        printf("Processos MPI: %d\n", size);
    }
    MPI_Finalize();
    return 0;
}
