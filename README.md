# Easy logger

'base lib is using spdlog library'

```cpp
using namespace logger;

void logInThread(int id) {
    for (int i = 0; i < 100; ++i) {
        INFO_LOG("Thread " << id << ": Iteration " << i);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    Logger::initialize();

    {
        TRACE_FUNCTION;
        DEBUG_LOG("This is a debug message.");
        TRACE_LOG("This is a trace message.");
        INFO_LOG("This is an info message.");
        WARN_LOG("This is a warning message.");
        ERROR_LOG("This is an error message.");
        CRITICAL_LOG("This is a critical message.");
    }

    // Test in a multi-threaded scenario
    std::thread thread1(logInThread, 1);
    std::thread thread2(logInThread, 2);
    std::thread thread3(logInThread, 3);

    thread3.join();
    thread1.join();
    thread2.join();

    return 0;
}

```