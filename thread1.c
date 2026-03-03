#include <iostream>
#include <thread>
#include <vector>

void teste_thread(int id) {
    std::cout << "Thread " << id << " executando\n";
}

int main() {
    std::vector<std::thread> threads;

    for (int i = 0; i < 10; i++) {
        threads.emplace_back(teste_thread, i);
    }

    for (auto &t : threads) {
        t.join();
    }

    return 0;
}
