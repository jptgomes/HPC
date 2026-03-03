#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

long long contador = 0;
std::mutex m;

void incrementa() {
    for (int i = 0; i < 1000000; i++) {
        std::lock_guard<std::mutex> g(m);
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
