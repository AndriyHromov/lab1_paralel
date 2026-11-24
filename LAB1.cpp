#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <string>
#include <chrono>

using namespace std;

// ==========================================
// Завдання 1.1: Базовий запуск потоків
// ==========================================
void t1_func1() { cout << "1 "; }
void t1_func2() { cout << "2 "; }

void runTask1_1() {
    cout << "\n--- Task 1.1: Basic Threads ---" << endl;
    thread t1(t1_func1);
    thread t2(t1_func2);
    t1.join();
    t2.join();
    cout << "\nMain finished." << endl;
}

// ==========================================
// Завдання 1.2: Використання метода detach
// ==========================================
void runTask1_2() {
    cout << "\n--- Task 1.2: Detached Threads ---" << endl;
    thread t1([] { cout << "1 (from detached thread 1)\n"; });
    thread t2([] { cout << "2 (from detached thread 2)\n"; });
    t1.detach();
    t2.detach();
    this_thread::sleep_for(chrono::milliseconds(50));
    cout << "Main finished." << endl;
}

// ==========================================
// Завдання 1.3: Потоки без синхронізації
// ==========================================
list<int> l_task3;

void addToList_13(int val) {
    for (int i = 0; i < 10; ++i) {
        l_task3.push_back(val + i);
        cout << "Added: " << (val + i) << endl;
        this_thread::sleep_for(chrono::milliseconds(5));
    }
}

void listContains_13(int val) {
    for (int i = 0; i < 10; ++i) {
        bool found = false;
        for (int elem : l_task3) {
            if (elem == val) { found = true; break; }
        }
        cout << "Check " << val << ": " << (found ? "Found" : "Not found") << endl;
        this_thread::sleep_for(chrono::milliseconds(5));
    }
}

void runTask1_3() {
    cout << "\n--- Task 1.3: Unsynchronized List Access ---" << endl;
    l_task3.clear();
    thread t1(addToList_13, 5);
    thread t2(listContains_13, 5);
    t1.join();
    t2.join();
}

// ==========================================
// Завдання 1.4: Захист сирим м'ютексом
// ==========================================
list<int> l_task4;
mutex mtx_task4;

void addToList_14(int val) {
    for (int i = 0; i < 10; ++i) {
        mtx_task4.lock();
        l_task4.push_back(val + i);
        cout << "Added (mutex): " << (val + i) << endl;
        mtx_task4.unlock();
        this_thread::sleep_for(chrono::milliseconds(5));
    }
}

void listContains_14(int val) {
    for (int i = 0; i < 10; ++i) {
        mtx_task4.lock();
        bool found = false;
        for (int elem : l_task4) {
            if (elem == val) { found = true; break; }
        }
        cout << "Check (mutex) " << val << ": " << (found ? "Found" : "Not found") << endl;
        mtx_task4.unlock();
        this_thread::sleep_for(chrono::milliseconds(5));
    }
}

void runTask1_4() {
    cout << "\n--- Task 1.4: Raw Mutex Protection ---" << endl;
    l_task4.clear();
    thread t1(addToList_14, 5);
    thread t2(listContains_14, 5);
    t1.join();
    t2.join();
}

// ==========================================
// Завдання 1.5: lock_guard та множинний запуск
// ==========================================
list<int> l_task5;
mutex mtx_task5;

void addToList_15(int val) {
    lock_guard<mutex> guard(mtx_task5);
    l_task5.push_back(val);
    cout << "Added once: " << val << endl;
}

void listContains_15(int val) {
    lock_guard<mutex> guard(mtx_task5);
    bool found = false;
    for (int elem : l_task5) {
        if (elem == val) { found = true; break; }
    }
    cout << "Checked once " << val << ": " << (found ? "Found" : "Not found") << endl;
}

void runTask1_5() {
    cout << "\n--- Task 1.5: lock_guard & Multiple Launches ---" << endl;
    l_task5.clear();
    for (int i = 0; i < 10; ++i) {
        thread t1(addToList_15, 100 + i);
        thread t2(listContains_15, 105);
        t1.detach();
        t2.detach();
    }
    this_thread::sleep_for(chrono::milliseconds(100));
}

// ==========================================
// Завдання 1.6 та 1.7: Класи someData, exchangePerson і Swap
// ==========================================
struct SomeData {
    string name;
    string surname;
    string address;
    int age;

    void print(const string& prefix) const {
        cout << prefix << " -> Name: " << name << ", Surname: " << surname 
             << ", Address: " << address << ", Age: " << age << endl;
    }
};

class ExchangePerson {
private:
    SomeData data;
    mutable mutex mtx;

public:
    ExchangePerson(string n, string s, string a, int ag) : data{n, s, a, ag} {}

    SomeData& getData() { return data; }
    mutex& getMutex() { return mtx; }

    static void JohnDoe(ExchangePerson& person) {
        lock_guard<mutex> guard(person.mtx);
        person.data.name = "John";
        person.data.surname = "Doe";
        person.data.address = "Unknown";
        person.data.age = 120;
    }

    static void JacobSmith(ExchangePerson& person) {
        lock_guard<mutex> guard(person.mtx);
        person.data.name = "Jacob";
        person.data.surname = "Smith";
        person.data.address = "Known";
        person.data.age = 1;
    }

    // Swap для завдання 1.6 (з std::lock та std::lock_guard з adopt_lock)
    static void Swap_LockGuard(ExchangePerson& p1, ExchangePerson& p2) {
        if (&p1 == &p2) return;
        std::lock(p1.mtx, p2.mtx);
        lock_guard<mutex> guard1(p1.mtx, adopt_lock);
        lock_guard<mutex> guard2(p2.mtx, adopt_lock);
        SomeData temp = p1.data;
        p1.data = p2.data;
        p2.data = temp;
    }

    // Swap для завдання 1.7 (з std::unique_lock та std::defer_lock)
    static void Swap_UniqueLock(ExchangePerson& p1, ExchangePerson& p2) {
        if (&p1 == &p2) return;
        unique_lock<mutex> lock1(p1.mtx, defer_lock);
        unique_lock<mutex> lock2(p2.mtx, defer_lock);
        std::lock(lock1, lock2);
        SomeData temp = p1.data;
        p1.data = p2.data;
        p2.data = temp;
    }
};

void runTask1_6() {
    cout << "\n--- Task 1.6: ExchangePerson & Swap (lock_guard) ---" << endl;
    ExchangePerson p1("Alice", "Brown", "Kyiv", 25);
    ExchangePerson p2("Bob", "Green", "Lviv", 30);
    
    p1.getData().print("P1 before");
    p2.getData().print("P2 before");

    thread t1(ExchangePerson::JohnDoe, ref(p1));
    thread t2(ExchangePerson::JacobSmith, ref(p2));
    t1.detach();
    t2.detach();
    this_thread::sleep_for(chrono::milliseconds(50));

    p1.getData().print("P1 after individual threads");
    p2.getData().print("P2 after individual threads");

    thread tSwap(ExchangePerson::Swap_LockGuard, ref(p1), ref(p2));
    tSwap.join();

    p1.getData().print("P1 after Swap");
    p2.getData().print("P2 after Swap");
}

void runTask1_7() {
    cout << "\n--- Task 1.7: ExchangePerson & Swap (unique_lock) ---" << endl;
    ExchangePerson p1("Alice", "Brown", "Kyiv", 25);
    ExchangePerson p2("Bob", "Green", "Lviv", 30);

    p1.getData().print("P1 before Swap");
    p2.getData().print("P2 before Swap");

    thread tSwap(ExchangePerson::Swap_UniqueLock, ref(p1), ref(p2));
    tSwap.join();

    p1.getData().print("P1 after Swap (unique_lock)");
    p2.getData().print("P2 after Swap (unique_lock)");
}

// ==========================================
// Головне меню програми
// ==========================================
int main() {
    int choice = 0;
    do {
        cout << "\n=========================================\n";
        cout << "       LABORATORY WORK NO. 1 MENU        \n";
        cout << "=========================================\n";
        cout << "1. Task 1.1 (Basic threads)\n";
        cout << "2. Task 1.2 (Detached threads)\n";
        cout << "3. Task 1.3 (Unsynchronized list)\n";
        cout << "4. Task 1.4 (Raw mutex list)\n";
        cout << "5. Task 1.5 (lock_guard list)\n";
        cout << "6. Task 1.6 (ExchangePerson & Swap with lock_guard)\n";
        cout << "7. Task 1.7 (ExchangePerson & Swap with unique_lock)\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        
        if (!(cin >> choice)) break;

        switch (choice) {
            case 1: runTask1_1(); break;
            case 2: runTask1_2(); break;
            case 3: runTask1_3(); break;
            case 4: runTask1_4(); break;
            case 5: runTask1_5(); break;
            case 6: runTask1_6(); break;
            case 7: runTask1_7(); break;
            case 0: cout << "Exiting program...\n"; break;
            default: cout << "Invalid choice! Please choose from 0 to 7.\n";
        }
    } while (choice != 0);

    return 0;
}
