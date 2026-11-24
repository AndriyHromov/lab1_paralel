#include <iostream>
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

void runExperiment(int n, std::launch policy, const std::string& modeName) {
    std::cout << "\n=== Rezhym: " << modeName << " ===\n";
    
    std::future<long long> fut = std::async(policy, getNthPrime, n);

    int choice;
    std::cout << "Poky yde obchyslennya, oberit funktsiyu vid n (" << n << "):\n"
              << "1 - Kvadratnyy korin (sqrt)\n"
              << "2 - Synus (sin)\n"
              << "3 - Naturalnyy logaryfm (ln)\n"
              << "Vash vybir: ";
    std::cin >> choice;

    double math_res = 0.0;
    if (choice == 1) math_res = std::sqrt(n);
    else if (choice == 2) math_res = std::sin(n);
    else if (choice == 3) math_res = std::log(n);
    std::cout << "Znachennya math-funktsiyi: " << math_res << "\n";

    std::cout << "Ochikuvannya rezultatu prostogo chysla...\n";
    auto start = std::chrono::high_resolution_clock::now();
    
    long long prime_res = fut.get();
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> wait_time = end - start;

    std::cout << n << "-te proste chyslo = " << prime_res << "\n";
    std::cout << "Chas ochikuvannya na fut.get(): " << wait_time.count() << " ms\n";
}

int main() {
    int n;
    std::cout << "Vvedit nomer prostogo chysla n (napryklad, 300000): ";
    std::cin >> n;

    runExperiment(n, std::launch::deferred, "std::launch::deferred");
    runExperiment(n, std::launch::async, "std::launch::async");

    return 0;
}
