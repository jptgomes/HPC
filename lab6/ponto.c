#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, valor;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        valor = 500;
        MPI_Send(&valor, 1, MPI_INT, 1, 10, MPI_COMM_WORLD);
        printf("Processo 0 enviou %d\n", valor);
    } else if (rank == 1) {
        MPI_Recv(&valor, 1, MPI_INT, 0, 10, MPI_COMM_WORLD, &status);
        printf("Processo 1 recebeu %d do processo %d\n", valor, status.MPI_SOURCE);
    }

    MPI_Finalize();
    return 0;
}
