#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, dados[10];
    MPI_Status status;
    int quantidade;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        int n = 4;
        int envio[4] = {10, 20, 30, 40};
        MPI_Send(envio, n, MPI_INT, 1, 99, MPI_COMM_WORLD);
    }

    if (rank == 1) {
        MPI_Recv(dados, 10, MPI_INT, 0, 99, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &quantidade);

        printf("Recebi %d elementos\n", quantidade);
        printf("Origem = %d | Tag = %d\n", status.MPI_SOURCE, status.MPI_TAG);
    }

    MPI_Finalize();
    return 0;
}
