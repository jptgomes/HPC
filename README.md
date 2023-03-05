# aulas
Repositório de códigos utilizados como exemplo na disciplina de HPC

Em nossas aulas vamos utilizar o SO Linux, mais precisamente o Ubuntu Server, mas pode ser qualquer outra distribuição baseada no Debian.
O Linux é o melhor SO para programação em C. Sua estabilidade, leveza e segurança são incomparáveis em relação aos outros sistemas operacionais.
Vamos usar seu compilador  GCC para os códigos exemplo nas disciplinas de HPC e SD.
O GCC está nativo nas distribuições Linux (maioria pelo menos), mas se for necessário instalar,  digite o seguinte comando:  apt-get install gcc

Vamos testar e compilar o primeiro código de teste. Abra o terminal e crie um arquivo:

nano helloworld.c

#include <stdio.h>

int main()

{

printf ("Hello, world!\n");

return 0;

}


Salve o arquivo.
Em seguida vamos compilar:  gcc helloworld.c  -o  helloworld

O programa.c é o arquivo .c onde está o código fonte do seu programa, já -o programa será o arquivo de saída, ou seja, o executável que o GCC irá criar.
Para executar, digite: ./programa

Com isso você tem o básico para compilar e executar qualquer código em C no Linux!
