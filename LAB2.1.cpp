#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>
#include <cmath>

std::queue<int> data_queue;
std::mutex mtx;
bool data_ready = false;

bool isPrime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

void DataPreparation() {
    std::cout << "Vvedit tsili chysla (dlya zavershennya vvedit 'q'):\n";
    int val;
    while (std::cin >> val) {
        std::lock_guard<std::mutex> lk(mtx);
        data_queue.push(val);
    }
    // Ochyshchennya potoku vvodu pislya stop-symvolu
    std::cin.clear();
    std::cin.ignore(10000, '\n');

    std::lock_guard<std::mutex> lk(mtx);
    data_ready = true;
}

void DataProcessing() {
    std::unique_lock<std::mutex> lk(mtx);
    while (!data_ready) {
        lk.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        lk.lock();
    }

    std::cout << "Prosti chysla z chergy: ";
    while (!data_queue.empty()) {
        int val = data_queue.front();
        data_queue.pop();
        if (isPrime(val)) {
            std::cout << val << " ";
        }
    }
    std::cout << "\n";
}

int main() {
    std::thread prep_thread(DataPreparation);
    prep_thread.detach();

    std::thread proc_thread(DataProcessing);
    proc_thread.join();

    return 0;
}
