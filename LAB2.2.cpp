#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>

std::mutex mtx;
std::condition_variable cv;
int i = 0;

void Waits(int id) {
    std::unique_lock<std::mutex> lk(mtx);
    std::cout << "[Potik " << id << "] Vkhodyt u stan ochikuvannya...\n";
    
    cv.wait(lk, [] { return i == 1; });
    
    std::cout << "[Potik " << id << "] Ochikuvannya zaversheno (i == 1).\n";
}

void Awake() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    {
        std::lock_guard<std::mutex> lk(mtx);
        std::cout << "[Awake] Pershe spovishchennya (i = 0)...\n";
    }
    cv.notify_all();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    {
        std::lock_guard<std::mutex> lk(mtx);
        i = 1;
        std::cout << "[Awake] Povtorne spovishchennya (i = 1)...\n";
    }
    cv.notify_all();
}

int main() {
    std::vector<std::thread> threads;
    for (int id = 1; id <= 3; ++id) {
        threads.emplace_back(Waits, id);
    }

    std::thread awake_thread(Awake);
    awake_thread.join();

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
