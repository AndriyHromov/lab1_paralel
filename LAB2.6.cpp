#include <iostream>
#include <deque>
#include <future>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

long long getNthPrime(int n) {
    if (n <= 0) return 0;
    int count = 0;
    long long num = 1;
    while (count < n) {
        num++;
        bool prime = true;
        if (num > 2 && num % 2 == 0) prime = false;
        else {
            for (long long i = 3; i * i <= num; i += 2) {
                if (num % i == 0) {
                    prime = false;
                    break;
                }
            }
        }
        if (prime) count++;
    }
    return num;
}

std::deque<std::packaged_task<long long(int)>> tasks_deque;
std::deque<int> args_deque;
std::mutex mtx;
std::condition_variable cv;
bool stop_worker = false;

void WorkerThread() {
    while (true) {
        std::packaged_task<long long(int)> task;
        int n = 0;

        {
            std::unique_lock<std::mutex> lk(mtx);
            cv.wait(lk, [] { return !tasks_deque.empty() || stop_worker; });

            if (tasks_deque.empty() && stop_worker) {
                break;
            }

            task = std::move(tasks_deque.front());
            tasks_deque.pop_front();

            n = args_deque.front();
            args_deque.pop_front();
        }

        // Vykonannya zavdannya poza krytychnoyu sektsiyeyu
        task(n);
    }
}

int main() {
    std::thread worker(WorkerThread);

    std::vector<std::pair<int, std::future<long long>>> results;
    int n;

    std::cout << "Vvedit nomery prostykh chysel (dlya zavershennya vvedit '0' abo stop-symvol):\n";
    while (std::cin >> n && n > 0) {
        std::packaged_task<long long(int)> task(getNthPrime);
        results.emplace_back(n, task.get_future());

        {
            std::lock_guard<std::mutex> lk(mtx);
            tasks_deque.push_back(std::move(task));
            args_deque.push_back(n);
        }
        cv.notify_one();
    }

    // Sygnalizuyemo rochomu potoku pro zavershennya pislya vykonannya vsikh zadach
    {
        std::lock_guard<std::mutex> lk(mtx);
        stop_worker = true;
    }
    cv.notify_one();

    // Ochikuvannya zavershennya roboty nezalezhnogo potoku
    worker.join();

    std::cout << "\nRezultaty obchyslen:\n";
    for (auto& item : results) {
        std::cout << item.first << "-te proste chyslo = " << item.second.get() << "\n";
    }

    return 0;
}
