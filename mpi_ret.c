#include "mpi.h"
#include <stdio.h>

/* parametros */

#define f(x) ((x) * (x))
#define num_ret 50
#define lim_inferior 2.0
#define lim_superior 5.0

int main( int argc, char * argv[ ] ){
/*Variáveis MPI */
int dest, n_Proc, proc_Id, src, tag; MPI_Status status;
/* Variáveis do problema */
int i;
double area, no, altura, base, largura, total, range;
/* Inicializacção do MPI */
MPI_Init(&argc, &argv);
MPI_Comm_size(MPI_COMM_WORLD, &n_Proc);
MPI_Comm_rank(MPI_COMM_WORLD, &proc_Id);
/*Ajustando o tamanho para o subproblema*/
range = (lim_superior - lim_inferior) / n_Proc;
largura = range / num_ret;
base = lim_inferior + range * proc_Id;
/*calculando área para o subproblema*/
area = 0.0;
for (i = 0; i < num_ret; i++) {
no = base + i * largura + largura / 2.0;
altura = f(no);
area = area + largura * altura;
}
/* coletando informações e imprimindo resultado */
tag = 0;
if (proc_Id == 0)
/* Se o rank é zero ele coleta os resultados */
{ total = area;
for (src=1; src < n_Proc; src++) {
MPI_Recv(&area, 1, MPI_DOUBLE, src, tag, MPI_COMM_WORLD,&status);
total = total + area;
 }
fprintf(stderr, "A area entre %f e %f e: %f\n", lim_inferior, lim_superior,total);
 }
else
 /* Todos os outros processos somente enviam os resultados */
{
 dest = 0;
MPI_Send(&area, 1, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD);
 };
 /* Finalizando */
 MPI_Finalize( );
 return 0;
}

 
