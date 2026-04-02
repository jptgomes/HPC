#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int dados[8], local[2], soma_local;
    int somas[4];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        for (int i = 0; i < 8; i++) dados[i] = i + 1;
    }

    MPI_Scatter(dados, 2, MPI_INT, local, 2, MPI_INT, 0, MPI_COMM_WORLD);

    soma_local = local[0] + local[1];
    printf("Processo %d recebeu [%d, %d] e somou %d\n", rank, local[0], local[1], soma_local);

    MPI_Gather(&soma_local, 1, MPI_INT, somas, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Somas reunidas no raiz: ");
        for (int i = 0; i < size; i++) printf("%d ", somas[i]);
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}
