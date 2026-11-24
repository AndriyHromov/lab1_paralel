#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

std::queue<int> data_queue;
std::mutex mtx;
std::condition_variable cv;
bool input_finished = false;

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
        {
            std::lock_guard<std::mutex> lk(mtx);
            data_queue.push(val);
        }
        cv.notify_one();
    }

    std::cin.clear();
    std::cin.ignore(10000, '\n');

    {
        std::lock_guard<std::mutex> lk(mtx);
        input_finished = true;
    }
    cv.notify_one();
}

void DataProcessing() {
    std::cout << "Prosti chysla: ";
    while (true) {
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait(lk, [] { return !data_queue.empty() || input_finished; });

        while (!data_queue.empty()) {
            int val = data_queue.front();
            data_queue.pop();
            lk.unlock(); // Rozblokovuyemo myuteks na chas obchyslen

            if (isPrime(val)) {
                std::cout << val << " ";
            }
            lk.lock();
        }

        if (input_finished && data_queue.empty()) {
            break;
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
