#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

std::mutex mtx;
std::condition_variable cv;
int i = 0;

void ThreadFunc(int id) {
    std::unique_lock<std::mutex> lk(mtx);
    cv.wait(lk, [] { return i == 1; });
    std::cout << "Povidomlennya z potoku " << id << "\n";
}

void Notify() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    {
        std::lock_guard<std::mutex> lk(mtx);
        i = 1;
        std::cout << "[Notify] Vstanovleno i = 1, vyklyk notify_one()\n";
    }
    cv.notify_one();
}

int main() {
    std::thread t1(ThreadFunc, 1);
    std::thread t2(ThreadFunc, 2);
    std::thread t3(ThreadFunc, 3);

    // Vid'yednuyemo potoky, shchob main mig zavershytysya pislya Notify,
    // oskilky 2 potoky zalyshatsya v stani ochikuvannya
    t1.detach();
    t2.detach();
    t3.detach();

    std::thread notify_thread(Notify);
    notify_thread.join();

    // Korotka zatrymka, shchob probudzhenyy potik vstyh vyvesty tekst u konsol
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    return 0;
}
