#include <iostream>
#include <thread>
#include <vector>

long long contador = 0;

void incrementa() {
    for (int i = 0; i < 1000000; i++) {
        contador++;  
    }
}

int main() {
    std::vector<std::thread> ts;

    for (int i = 0; i < 4; i++)
        ts.emplace_back(incrementa);

    for (auto &t : ts)
        t.join();

    std::cout << "Valor final: " << contador << std::endl;
}
