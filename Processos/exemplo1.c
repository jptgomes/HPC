#include <stdio.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        printf("Sou o processo FILHO\n");
    } else {
        printf("Sou o processo PAI\n");
    }

    return 0;
}
