#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

class ValueChecker {
private:
    int currentValue;
    std::atomic<bool> flag;
    std::chrono::milliseconds interval;
    std::chrono::milliseconds timeout;
    std::thread checkerThread;
    bool running;

    void checkValue() {
        int lastValue = currentValue;
        auto lastChange = std::chrono::steady_clock::now();

        while (running) {
            if (currentValue != lastValue) {
                lastValue = currentValue;
                lastChange = std::chrono::steady_clock::now();
            } else if (std::chrono::steady_clock::now() - lastChange >= timeout) {
                flag.store(true);
            }

            std::this_thread::sleep_for(interval);
        }
    }

public:
    ValueChecker(int interval_ms, int timeout_ms)
        : interval(interval_ms), timeout(timeout_ms), running(false) {
        flag.store(false);
    }

    ~ValueChecker() {
        if (checkerThread.joinable()) {
            stopChecking();
            checkerThread.join();
        }
    }

    void startChecking() {
        running = true;
        checkerThread = std::thread(&ValueChecker::checkValue, this);
    }

    void stopChecking() {
        running = false;
    }

    void setValue(int value) {
        currentValue = value;
    }

    bool getFlag() const {
        return flag.load();
    }

    void resetFlag() {
        flag.store(false);
    }
};

// int main() {
//     ValueChecker checker(1000, 5000);  // check every 1 second, set flag if no change after 5 seconds

//     checker.setValue(5);
//     checker.startChecking();

//     std::this_thread::sleep_for(std::chrono::milliseconds(3000));
//     checker.setValue(6);

//     std::this_thread::sleep_for(std::chrono::milliseconds(4000));
//     std::cout << "Flag status after 7 seconds: " << checker.getFlag() << std::endl;

//     std::this_thread::sleep_for(std::chrono::milliseconds(2000));
//     std::cout << "Flag status after 9 seconds: " << checker.getFlag() << std::endl;

//     checker.stopChecking();
//     return 0;
// }
