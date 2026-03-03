#include <stdio.h>
#include <unistd.h>

int global = 10;

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        global += 5;
        printf("Filho: global = %d\n", global);
    } else {
        sleep(1);
        printf("Pai: global = %d\n", global);
    }

    return 0;
}
