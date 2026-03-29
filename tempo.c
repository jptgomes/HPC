#include <mpi.h>
#include <stdio.h>
int main(int argc, char *argv[]) {
    int rank;
    double inicio, fim;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    inicio = MPI_Wtime();
    for (long i = 0; i < 1000000000; i++);
    fim = MPI_Wtime();
    printf("Processo %d tempo: %f segundos\n", rank, fim - inicio);
    MPI_Finalize();
    return 0;
}
