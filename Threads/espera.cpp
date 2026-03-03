#include <iostream>
#include <thread>

bool pronto = false;

void espera() {
    while (!pronto) {
        // esperando...
    }
    std::cout << "Continuando...\n";
}

int main() {
    std::thread t(espera);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    pronto = true;

    t.join();
}
