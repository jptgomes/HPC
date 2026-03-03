#include <iostream>
#include <thread>
#include <vector>
#include <semaphore>

std::counting_semaphore<3> sem(3);  // 3 recursos

void tarefa(int id) {
    sem.acquire();
    std::cout << "Thread " << id << " entrou\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "Thread " << id << " saiu\n";
    sem.release();
}

int main() {
    std::vector<std::thread> ts;

    for (int i = 0; i < 8; i++)
        ts.emplace_back(tarefa, i);

    for (auto &t : ts)
        t.join();
}
