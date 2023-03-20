#include <stdio.h>

/* Parâmetros */
#define f(x) ((x) * (x))
#define num_ret 50
#define lim_inferior 2.0
#define lim_superior 5.0

int main ( int argc, char * argv[ ] )
{
int i;
double area, no, altura, largura; area = 0.0;
largura = (lim_superior - lim_inferior) / num_ret;
for (i = 0; i < num_ret; i++)
{
no = lim_inferior + i * largura + largura / 2.0;
altura = f(no);
area = area + largura * altura;
}

printf("A area entre %f e %f e: %f\n", lim_inferior, lim_superior, area );
return 0;
}

