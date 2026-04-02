#include <mpi.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    printf("Processo %d de %d chegou antes da barreira\n", rank, size);

    sleep(rank);  
    // só para simular que cada processo leva um tempo diferente

    MPI_Barrier(MPI_COMM_WORLD);

    printf("Processo %d passou pela barreira\n", rank);

    MPI_Finalize();
    return 0;
}
