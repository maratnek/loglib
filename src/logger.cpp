#include "logger.hpp"

using namespace logger;

// Global logger instance
std::shared_ptr<spdlog::logger> Logger::logger = nullptr;

// Initialize the mutex
std::mutex& Logger::getMutex() {
    static std::mutex mutex;
    return mutex;
}