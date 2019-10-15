// Copyright (c) 2019 Branch Metrics, Inc.

#ifndef BRANCHIO_LOG_H__
#define BRANCHIO_LOG_H__

#include <Poco/Logger.h>

#include <string>
#include <sstream>

#include "BranchIO/dll.h"

namespace BranchIO {

/**
 * (Internal) Log API
 *
 ```
 #include <BranchIO/Log.h>

 using BranchIO::Log;

 Log::setLevel(Log::Info);

 // Automatically includes file, line number and function from which the
 // message was logged.
 BRANCH_LOG_D("Received " << n << " responses");

 // Does not include file, line number or function. Can't use streams.
 Log::d("Hello");
 ```
 */
class BRANCHIO_DLL_EXPORT Log {
 public:
    /**
     * Log-level enumeration
     */
    enum Level {
        /// Error level
        Error,
        /// Warning level
        Warning,
        /// Info level
        Info,
        /// Debug level
        Debug,
        /// Verbose level
        Verbose
    };

    /**
     * Log an Error-level message
     * @param message the message to log
     * @param func (optional) the function from which the message was logged (set by BRANCH_LOG_E macro)
     * @param file (optional) the file from which the message was logged (set by BRANCH_LOG_E macro)
     * @param line (optional) the line from which the message was logged (set by BRANCH_LOG_E macro)
     */
    static void e(const std::string& message, const char* func = NULL, const char* file = NULL, int line = 0) {
        instance().error(message, func, file, line);
    }

    /**
     * Log a Warning-level message
     * @param message the message to log
     * @param func (optional) the function from which the message was logged (set by BRANCH_LOG_W macro)
     * @param file (optional) the file from which the message was logged (set by BRANCH_LOG_W macro)
     * @param line (optional) the line from which the message was logged (set by BRANCH_LOG_W macro)
     */
    static void w(const std::string& message, const char* func = NULL, const char* file = NULL, int line = 0) {
        instance().warning(message, func, file, line);
    }

    /**
     * Log an Info-level message
     * @param message the message to log
     * @param func (optional) the function from which the message was logged (set by BRANCH_LOG_I macro)
     * @param file (optional) the file from which the message was logged (set by BRANCH_LOG_I macro)
     * @param line (optional) the line from which the message was logged (set by BRANCH_LOG_I macro)
     */
    static void i(const std::string& message, const char* func = NULL, const char* file = NULL, int line = 0) {
        instance().info(message, func, file, line);
    }

    /**
     * Log a Debug-level message (compiled out in Release builds)
     * @param message the message to log
     * @param func (optional) the function from which the message was logged (set by BRANCH_LOG_D macro)
     * @param file (optional) the file from which the message was logged (set by BRANCH_LOG_D macro)
     * @param line (optional) the line from which the message was logged (set by BRANCH_LOG_D macro)
     */
    static void d(const std::string& message, const char* func = NULL, const char* file = NULL, int line = 0) {
        instance().debug(message, func, file, line);
    }

    /**
     * Log a Verbose-level message (compiled out in Release builds)
     * @param message the message to log
     * @param func (optional) the function from which the message was logged (set by BRANCH_LOG_V macro)
     * @param file (optional) the file from which the message was logged (set by BRANCH_LOG_V macro)
     * @param line (optional) the line from which the message was logged (set by BRANCH_LOG_V macro)
     */
    static void v(const std::string& message, const char* func = NULL, const char* file = NULL, int line = 0) {
        instance().verbose(message, func, file, line);
    }

    /**
     * @return an instance of the Branch Log.
     */
    static Log& instance();

    /**
     * Set the current log level. Default is Error for Release builds and
     * Verbose for Debug builds.
     * @param level a Log::Level value to use
     * @return the singleton instance
     */
    static Log& setLevel(Level level);

    /**
     * Send log output to the console with optional color
     * @param enableColors whether to generate colored output
     * @return the singleton instance
     */
    static Log& enableConsoleLogging(bool enableColors = true);

    /**
     * Send log output to a file. Appends to any existing file.
     * Rolls over log files when they exceed a maximum size.
     * @param path a valid output file path
     * @return the singleton instance
     */
    static Log& enableFileLogging(const std::string& path);

    /**
     * Send log output to the system log (only partly working)
     * @return the singleton instance
     */
    static Log& enableSystemLogging();

 private:
    Log();
    Log(const Log& o) : _logger(Poco::Logger::root()) {}
    ~Log();
    Log& operator=(const Log& o) { return *this; }

    void error(const std::string& message, const char* func, const char* file, int line);
    void warning(const std::string& message, const char* func, const char* file, int line);
    void info(const std::string& message, const char* func, const char* file, int line);
    void debug(const std::string& message, const char* func, const char* file, int line);
    void verbose(const std::string& message, const char* func, const char* file, int line);

    Poco::Logger& getLogger() {
        return _logger;
    }

    std::string buildMessage(Level level, const std::string& message, const char* func, const char* file, int line);

    static Level getDefaultLogLevel();
    static std::string getDefaultLogFile();
    static Poco::Message::Priority getLoggerPriority(Level level);

    static Poco::Channel* makeFileLoggingChannel(const std::string& path);
    static Poco::Channel* makeSystemLoggingChannel();
    static Poco::Channel* makeConsoleLoggingChannel(bool enableColors = true);

    Poco::Logger& _logger;
};

}  // namespace BranchIO

#define BRANCH_LOG_E(m) \
    { \
        std::ostringstream oss; \
        oss << m; \
        BranchIO::Log::e(oss.str(), __func__, __FILE__, __LINE__); \
    }

#define BRANCH_LOG_W(m) \
    { \
        std::ostringstream oss; \
        oss << m; \
        BranchIO::Log::w(oss.str(), __func__, __FILE__, __LINE__); \
    }

#define BRANCH_LOG_I(m) \
    { \
        std::ostringstream oss; \
        oss << m; \
        BranchIO::Log::i(oss.str(), __func__, __FILE__, __LINE__); \
    }

#ifdef DEBUG
    #define BRANCH_LOG_D(m) \
        { \
            std::ostringstream oss; \
            oss << m; \
            BranchIO::Log::d(oss.str(), __func__, __FILE__, __LINE__); \
        }

    #define BRANCH_LOG_V(m) \
        { \
            std::ostringstream oss; \
            oss << m; \
            BranchIO::Log::v(oss.str(), __func__, __FILE__, __LINE__); \
        }
#else
    // Debug and Verbose compiled out in Release builds.
    #define BRANCH_LOG_D(m)
    #define BRANCH_LOG_V(m)
#endif  // DEBUG

std::ostream&
operator<<(std::ostream& s, BranchIO::Log::Level level);

std::istream&
operator>>(std::istream& s, BranchIO::Log::Level& level);

#endif  // BRANCHIO_LOG_H__
