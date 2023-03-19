#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[]) {
int ranque, num_procs; 
int aux, erro;
double tempo_inicial, tempo_final;
char maquina[MPI_MAX_PROCESSOR_NAME];

    /* Inicia o MPI. Em caso de erro aborta o programa */
    erro = MPI_Init(&argc, &argv);
    if (erro != MPI_SUCCESS) {
        printf("Erro ao iniciar o programa MPI. Abortando programa.\n");
        MPI_Abort(MPI_COMM_WORLD, erro);
    }

	/*tempo de execução inicial*/
	tempo_inicial = MPI_Wtime();

    /* Obtém o ranque e número de processos em execução */
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &ranque);
    
    /* Define o nome do computador onde o processo está executando */
    MPI_Get_processor_name(maquina, &aux);
    printf("Número de tarefas = %d Meu ranque = %d Executando em  %s\n", num_procs, ranque, maquina);
    
	/*tempo de execução final*/
	tempo_final = MPI_Wtime(); 
	printf ("Foram gastos %3.6f segundos com precisão %3.3e segundos\n", tempo_final - tempo_inicial, MPI_Wtick ());
   
    /* Finaliza o MPI */
    MPI_Finalize();
    
    return(0);
}

