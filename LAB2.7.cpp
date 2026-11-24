#include <iostream>
#include <thread>
#include <future>
#include <cmath>
#include <chrono>

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

void FirstWorker(int n,
                 std::promise<long long> prom_nth,
                 std::promise<bool> prom_signal,
                 std::promise<long long> prom_10nth) {
    // 1. Obchyslennya ta peredacha n-go prostogo chysla
    long long res1 = getNthPrime(n);
    prom_nth.set_value(res1);

    // 2. Sygnal drugomu potoku pro pochatok yogo roboty
    prom_signal.set_value(true);

    // 3. Obchyslennya ta peredacha (n*10)-go prostogo chysla
    long long res2 = getNthPrime(n * 10);
    prom_10nth.set_value(res2);
}

void SecondWorker(int n, std::future<bool> fut_signal) {
    // Ochikuvannya sygnalu vid pershogo potoku
    bool start_flag = fut_signal.get();
    if (start_flag) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "[Druhyy potik] Kvadratnyy korin z " << n
                  << " = " << std::sqrt(n) << "\n";
    }
}

int main() {
    int n;
    std::cout << "Vvedit nomer prostogo chysla n: ";
    std::cin >> n;

    std::promise<long long> prom_nth;
    std::future<long long> fut_nth = prom_nth.get_future();

    std::promise<bool> prom_signal;
    std::future<bool> fut_signal = prom_signal.get_future();

    std::promise<long long> prom_10nth;
    std::future<long long> fut_10nth = prom_10nth.get_future();

    std::thread t1(FirstWorker, n,
                   std::move(prom_nth),
                   std::move(prom_signal),
                   std::move(prom_10nth));

    std::thread t2(SecondWorker, n, std::move(fut_signal));

    // Otrymannya rezultativ u golovnomu pototsi po miri yikh gotovnosti
    std::cout << "[Golovnyy potik] " << n << "-te proste chyslo = "
              << fut_nth.get() << "\n";

    std::cout << "[Golovnyy potik] " << (n * 10) << "-te proste chyslo = "
              << fut_10nth.get() << "\n";

    t1.join();
    t2.join();

    return 0;
}
