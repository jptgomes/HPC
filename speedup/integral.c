#include <mpi.h>
#include <stdio.h>

double f(double x) {
    return x * x;
}

int main(int argc, char *argv[]) {
    int rank, size;
    long long n = 100000000;   // numero de subintervalos
    double a = 0.0, b = 1.0;
    double h, soma_local = 0.0, soma_global = 0.0;
    double inicio, fim, integral;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    h = (b - a) / n;

    inicio = MPI_Wtime();

    for (long long i = rank; i < n; i += size) {
        double x = a + (i + 0.5) * h;  // metodo do ponto medio
        soma_local += f(x);
    }

    soma_local *= h;

    MPI_Reduce(&soma_local, &soma_global, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    fim = MPI_Wtime();

    if (rank == 0) {
        integral = soma_global;
        printf("Integral aproximada = %.12f\n", integral);
        printf("Valor esperado      = %.12f\n", 1.0 / 3.0);
        printf("Tempo total         = %.6f segundos\n", fim - inicio);
    }

    MPI_Finalize();
    return 0;
}
