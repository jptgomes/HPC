#include "mpi.h"
#include <stdio.h>

int main( int argc, char * argv[ ] )

{
        int processo;    /* rank dos processos */
        int n_Processos;  /* Número de processos */
        int tamanho_nome;     /* Tamanho do nome */
        char nome_computador[MPI_MAX_PROCESSOR_NAME];

        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &n_Processos);
        MPI_Comm_rank(MPI_COMM_WORLD, &processo);
        MPI_Get_processor_name(nome_computador, &tamanho_nome;);
        fprintf(stderr,"Olá para o processo %d no computador %s\n", processo, nome_computador);
        MPI_Finalize( );

        return 0;

}


