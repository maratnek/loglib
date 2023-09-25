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

  void flush()
  {
    if (!logStream.str().empty())
    {
      log(logLevel, logStream.str());
      logStream.str("");
    }
  }

  static void initialize(std::string const& pattern = "") {
    static bool initialized = false;
    if (!initialized) {
      std::lock_guard<std::mutex> lock(Logger::getMutex());
      if (!initialized) {
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
            "logfile.log", true);
        auto console_sink =
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        logger = std::make_shared<spdlog::logger>(
            "", spdlog::sinks_init_list({file_sink, console_sink}));

        // spdlog::set_pattern("[%H:%M:%S.%e] [%^%l%$] [File: %s] [Function: %!] "
                            // "[Line: %!] [Thread: %t] %v");
        // logger->set_pattern("[%^%L%$][%H:%M:%S.%e] [Td: %t] %v");
        if (pattern.empty()) {
          logger->set_pattern("[%^%L%$][%H:%M:%S.%e] [Td: %t] %v");
        } else {
          logger->set_pattern(pattern);
        }

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

  static void log(LogLevel level, const std::string &message)
  {
    initialize();
    switch (level)
    {
    case LogLevel::DEBUG:
      logger->debug(message);
      break;
    case LogLevel::TRACE:
      logger->trace(message);
      break;
    case LogLevel::INFO:
      logger->info(message);
      break;
    case LogLevel::WARN:
      logger->warn(message);
      break;
    case LogLevel::ERROR:
      logger->error(message);
      break;
    case LogLevel::CRITICAL:
      logger->critical(message);
      break;
    default:
      break;
    }
  }

  std::ostringstream &getStream()
  {
    return logStream;
  }

  void setLevel(LogLevel level)
  {
    logLevel = level;
  }

private:
  const char *functionName;
  const char *fileName;
  int lineNumber;
  std::ostringstream logStream;
  LogLevel logLevel = LogLevel::INFO;
  static std::shared_ptr<spdlog::logger> logger;

};

#define TRACE_FUNCTION Logger loggerInstance(__FUNCTION__, __FILE__, __LINE__);

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

#define LOG_MSG(level, message)                                                                               \
do                                                                                                            \
{                                                                                                             \
  std::ostringstream logStream;                                                                               \
  logStream << "[" << extractFileName(__FILE__) << ":" << __LINE__ << " " << __FUNCTION__ << "] " << message; \
  {                                                                                                           \
    std::lock_guard<std::mutex> lock(logger::Logger::getMutex());                                             \
    logger::Logger::get()->level(logStream.str());                                                            \
  }                                                                                                           \
} while (false)

  // logStream << message;                                                                                       \

#define DEBUG_LOG(message) LOG_MSG(debug, message)
#define TRACE_LOG(message) LOG_MSG(trace, message)
#define INFO_LOG(message) LOG_MSG(info, message)
#define WARN_LOG(message) LOG_MSG(warn, message)
#define ERROR_LOG(message) LOG_MSG(error, message)
#define CRITICAL_LOG(message) LOG_MSG(critical, message)

// @class For stream logging 
class LogStream {
public:
    LogStream(Logger& logger, LogLevel level)
        : logger(logger), level(level) {
    }

    ~LogStream() {
        logger.flush();
    }

    template <typename T>
    LogStream& operator<<(const T& value) {
        logger.setLogLevel(level);
        logger.getStream() << value;
        return *this;
    }

private:
    Logger& logger;
    LogLevel level;
};

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