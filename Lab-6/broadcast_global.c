#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, n;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) n = 1000000;

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    printf("Processo %d recebeu n = %d\n", rank, n);

    MPI_Finalize();
    return 0;
}
