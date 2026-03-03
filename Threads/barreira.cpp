#include <iostream>
#include <thread>
#include <vector>
#include <barrier>

std::barrier sync_point(4);

void tarefa(int id) {
    std::cout << "Thread " << id << " fase 1\n";
    sync_point.arrive_and_wait();

    std::cout << "Thread " << id << " fase 2\n";
}

int main() {
    std::vector<std::thread> ts;

    for (int i = 0; i < 4; i++)
        ts.emplace_back(tarefa, i);

    for (auto &t : ts)
        t.join();
}
