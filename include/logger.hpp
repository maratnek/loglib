#ifndef _LOGGER_HPP_
#define _LOGGER_HPP_

#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace logger {

enum class LogLevel { DEBUG, TRACE, INFO, WARN, ERROR, CRITICAL, OFF };

class Logger {
public:
  Logger(const char *functionName, const char *fileName, int lineNumber)
      : functionName(functionName), fileName(fileName), lineNumber(lineNumber) {
    Logger::initialize();
    logger->trace("Entering function {} in file {}:{}", functionName,
                  fileName, lineNumber);
  }

  ~Logger() {
    logger->trace("Exiting function {} in file {}:{}", functionName,
                  fileName, lineNumber);
  }

  static void initialize() {
    static bool initialized = false;
    if (!initialized) {
      std::lock_guard<std::mutex> lock(Logger::getMutex());
      if (!initialized) {
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
            "logfile.log", true);
        auto console_sink =
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        // spdlog::set_pattern("[%H:%M:%S.%e] [%^%l%$] [File: %s] [Function: %!] "
                            // "[Line: %!] [Thread: %t] %v");
        logger = std::make_shared<spdlog::logger>(
            "", spdlog::sinks_init_list({file_sink, console_sink}));
        logger->set_pattern("[%^%L%$][%H:%M:%S.%e] "
                            "[Thread: %t] %v");

        // Set the logger level based on the CMake LOG_LEVEL variable
#ifdef LOG_LEVEL
        std::cout << "Setting log level to " << LOG_LEVEL << std::endl;
        spdlog::level::level_enum spdlogLevel;
        if (LOG_LEVEL == "DEBUG") {
          spdlogLevel = spdlog::level::debug;
        } else if (LOG_LEVEL == "TRACE") {
          spdlogLevel = spdlog::level::trace;
        } else if (LOG_LEVEL == "INFO") {
          spdlogLevel = spdlog::level::info;
        } else if (LOG_LEVEL == "WARN") {
          spdlogLevel = spdlog::level::warn;
        } else if (LOG_LEVEL == "ERROR") {
          spdlogLevel = spdlog::level::err;
        } else if (LOG_LEVEL == "CRITICAL") {
          spdlogLevel = spdlog::level::critical;
        } else if (LOG_LEVEL == "OFF") {
          spdlogLevel = spdlog::level::off;
        } else {
          spdlogLevel = spdlog::level::info; // Default to INFO
        }
        logger->set_level(spdlogLevel);
#else

        logger->set_level(spdlog::level::trace);
#endif
        initialized = true;
      }
    }
  }

  static std::shared_ptr<spdlog::logger> get() {
    initialize();
    return logger;
  }

  // TODO: should be deleted
  static void setLogLevel(LogLevel level) {
    spdlog::level::level_enum spdlogLevel;
    switch (level) {
    case LogLevel::DEBUG:
      spdlogLevel = spdlog::level::debug;
      break;
    case LogLevel::TRACE:
      spdlogLevel = spdlog::level::trace;
      break;
    case LogLevel::INFO:
      spdlogLevel = spdlog::level::info;
      break;
    case LogLevel::WARN:
      spdlogLevel = spdlog::level::warn;
      break;
    case LogLevel::ERROR:
      spdlogLevel = spdlog::level::err;
      break;
    }
    std::lock_guard<std::mutex> lock(Logger::getMutex());
    logger->set_level(spdlogLevel);
  }

  static std::mutex& getMutex();

private:
  const char *functionName;
  const char *fileName;
  int lineNumber;
  static std::shared_ptr<spdlog::logger> logger;

};

#define TRACE_FUNCTION Logger loggerInstance(__FUNCTION__, __FILE__, __LINE__);

#define LOG_MSG(level, message)                                   \
do                                                                \
{                                                                 \
  std::ostringstream logStream;                                   \
  logStream << message;                                           \
  {                                                               \
    std::lock_guard<std::mutex> lock(logger::Logger::getMutex()); \
    logger::Logger::get()->level(logStream.str());                \
  }                                                               \
} while (false)

// logStream << "[" << extractFileName(__FILE__) << ":" << __LINE__ << " " << __FUNCTION__ << "] " << message; \

// Function to extract the file name from the full path
inline const char* extractFileName(const char* filePath) {
    const char* fileName = strrchr(filePath, '/');
    if (fileName) {
        return fileName + 1;
    } else {
        fileName = strrchr(filePath, '\\');
        return fileName ? fileName + 1 : filePath;
    }
}

#define DEBUG_LOG(message) LOG_MSG(debug, message)
#define TRACE_LOG(message) LOG_MSG(trace, message)
#define INFO_LOG(message) LOG_MSG(info, message)
#define WARN_LOG(message) LOG_MSG(warn, message)
#define ERROR_LOG(message) LOG_MSG(error, message)
#define CRITICAL_LOG(message) LOG_MSG(critical, message)

#define TRACE_FUNCTION_ENTRY \
do                           \
{                            \
  Logger::initialize();      \
  TRACE_FUNCTION;            \
} while (false)

#define D_LOG_ST LogStream(Logger::get(), LogLevel::DEBUG)
#define T_LOG_ST LogStream(Logger::get(), LogLevel::TRACE)
#define I_LOG_ST LogStream(Logger::get(), LogLevel::INFO)
#define W_LOG_ST LogStream(Logger::get(), LogLevel::WARN)
#define E_LOG_ST LogStream(Logger::get(), LogLevel::ERROR)
#define C_LOG_ST LogStream(Logger::get(), LogLevel::CRITICAL)

} // namespace logger

#endif // _LOGGER_HPP_