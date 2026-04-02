#include <mpi.h>
#include <stdio.h>
int main(int argc, char *argv[]) {
    int rank, valor;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        valor = 50;
        MPI_Send(&valor, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        printf("Enviado: %d\n", valor);
    }
    if (rank == 1) {
        MPI_Recv(&valor, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Recebido: %d\n", valor);
    }
    MPI_Finalize();
}
