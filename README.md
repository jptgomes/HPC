**🖥 Repositório de Códigos – Disciplina de HPC**

Este repositório contém os códigos utilizados como exemplo na disciplina de Computação de Alto Desempenho (HPC).

**🐧 Ambiente Utilizado**

Durante as aulas utilizaremos o sistema operacional Linux, preferencialmente o Ubuntu Server, embora qualquer distribuição baseada em Debian (ou outra distribuição Linux) possa ser utilizada.

O Linux é amplamente adotado em ambientes de HPC devido à sua:

- Estabilidade
- Desempenho
- Segurança
- Flexibilidade

Amplo suporte a compiladores e bibliotecas científicas

**🔧 Compiladores**

Utilizaremos o compilador GCC (GNU Compiler Collection) para códigos em C e o g++ para códigos em C++.

Na maioria das distribuições Linux, o GCC já vem instalado. Caso não esteja disponível, pode ser instalado com:

sudo apt update
sudo apt install build-essential

O pacote build-essential instala:

- gcc (compilador C)
- g++ (compilador C++)
- make
- Bibliotecas padrão de desenvolvimento

**📌 Exemplo 1 – Hello World em C**

- Crie o arquivo: nano helloworld.c

    Insira o código:

#include <stdio.h>

int main() {
    printf("Olá, mundo!\n");
    return 0;
}

**▶ Compilação (C – gcc)**

gcc helloworld.c -o helloworld

Onde:

- helloworld.c → arquivo fonte
- -o helloworld → nome do executável gerado

**▶ Execução:** ./helloworld

**📌 Exemplo 2 – Hello World em C++**

Crie o arquivo:

nano helloworld.cpp

Código:

```cpp
#include <iostream>

int main() {
    std::cout << "Olá, mundo!" << std::endl;
    return 0;
}
```

**▶ Compilação (C++ – g++)**

g++ helloworld.cpp -o helloworld

**▶ Execução:** ./helloworld

**Observação Importante (HPC e Threads)**

Para programas que utilizam threads (std::thread), é necessário adicionar a flag:

g++ arquivo.cpp -o programa -pthread

A opção -pthread habilita suporte adequado a multithreading no Linux.

**🎯 Objetivo**

Com esses passos você possui o básico necessário para:

- Criar arquivos fonte
- Compilar programas em C e C++
- Executar aplicações no ambiente Linux

Esses fundamentos serão utilizados ao longo da disciplina de Computação de Alto Desempenho (HPC) para:

- Desenvolvimento de códigos paralelos
- Medições de desempenho
- Análise de speedup
- Aplicação da Lei de Amdahl
- Implementação com MPI
