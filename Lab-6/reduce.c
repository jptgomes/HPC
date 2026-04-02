#include <mpi.h>
#include <stdio.h>
int main(int argc, char *argv[]) {
    int rank, soma, valor;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    valor = rank + 1;
    MPI_Reduce(&valor, &soma, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0)
        printf("Soma total: %d\n", soma);
    MPI_Finalize();
}
