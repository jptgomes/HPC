#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

class ThreadSimples {
private:
    void Mensagem(int thread_no) {
        this_thread::sleep_for(chrono::milliseconds(200 * (thread_no % 5)));
        cout << "Olá da thread: " << thread_no << endl;
    }

public:
    int N_THREADS = 20;

    thread spawn(int thread_no) {
        return thread([this, thread_no]() {
            Mensagem(thread_no);
        });
    }
};

int main() {
    ThreadSimples thread_simples;
    thread threads[20];

    for (int i = 0; i < thread_simples.N_THREADS; i++) {
        threads[i] = thread_simples.spawn(i);
    }
    for (int i = 0; i < thread_simples.N_THREADS; i++) {
        threads[i].join();
    }
    return 0;
}
