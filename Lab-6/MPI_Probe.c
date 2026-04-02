#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int rank;
    MPI_Status status;
    int quantidade;
    int *buffer;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        int n = 6;
        int dados[6] = {1, 2, 3, 4, 5, 6};
        MPI_Send(dados, n, MPI_INT, 1, 7, MPI_COMM_WORLD);
    }

    if (rank == 1) {
        MPI_Probe(0, 7, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &quantidade);

        buffer = (int*) malloc(quantidade * sizeof(int));
        MPI_Recv(buffer, quantidade, MPI_INT, 0, 7, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Recebi %d elementos: ", quantidade);
        for (int i = 0; i < quantidade; i++) {
            printf("%d ", buffer[i]);
        }
        printf("\n");

        free(buffer);
    }

    MPI_Finalize();
    return 0;
}
