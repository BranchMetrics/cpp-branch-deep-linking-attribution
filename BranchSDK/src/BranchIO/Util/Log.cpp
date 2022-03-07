// Copyright (c) 2019-21 Branch Metrics, Inc.

#include "Log.h"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include "FileLogChannel.h"
#include "ConsoleLogChannel.h"
#include "StringUtils.h"

using namespace std;

#define ERROR_LEVEL_NAME "Error"
#define WARNING_LEVEL_NAME "Warning"
#define INFO_LEVEL_NAME "Info"
#define DEBUG_LEVEL_NAME "Debug"
#define VERBOSE_LEVEL_NAME "Verbose"

std::ostream&
operator<<(std::ostream& s, BranchIO::Log::Level level) {
    using BranchIO::Log;
    switch (level) {
    case Log::Error:
        s << ERROR_LEVEL_NAME;
        break;
    case Log::Warning:
        s << WARNING_LEVEL_NAME;
        break;
    case Log::Info:
        s << INFO_LEVEL_NAME;
        break;
    case Log::Debug:
        s << DEBUG_LEVEL_NAME;
        break;
    case Log::Verbose:
        s << VERBOSE_LEVEL_NAME;
        break;
    default:
        s << "(unknown " << int(level) << ")";
        break;
    }

    return s;
}

std::istream&
operator>>(std::istream& s, BranchIO::Log::Level& level) {
    using BranchIO::Log;
    string svalue;
    s >> svalue;
    if (svalue.compare(ERROR_LEVEL_NAME) == 0) {
        level = Log::Error;
    } else if (svalue.compare(WARNING_LEVEL_NAME) == 0) {
        level = Log::Warning;
    } else if (svalue.compare(INFO_LEVEL_NAME) == 0) {
        level = Log::Info;
    } else if (svalue.compare(DEBUG_LEVEL_NAME) == 0) {
        level = Log::Debug;
    } else if (svalue.compare(VERBOSE_LEVEL_NAME) == 0) {
        level = Log::Verbose;
    } else {
        throw std::runtime_error("Invalid log level");
    }

    return s;
}

namespace BranchIO {

int Log::_level = Log::getDefaultLogLevel();
LogChannel* Log:: _channel = NULL;

Log&
Log::instance() {
    static Log _instance;
    return _instance;
}

Log::Log() {
    // Set up the channel

    string logFile(getDefaultLogFile());
    if (logFile.empty()) {
        _channel = makeConsoleLoggingChannel();
    } else {
        _channel = makeFileLoggingChannel(logFile);
    }
}

Log::~Log() {
}

Log&
Log::setLevel(Level plevel) {
    _level = plevel;
    return instance();
}

Log&
Log::enableFileLogging(const std::string& path) {
    _channel = makeFileLoggingChannel(path); 
    return instance();
}

Log&
Log::enableSystemLogging() {
    _channel = makeSystemLoggingChannel();
    return instance();
}

Log&
Log::enableConsoleLogging(bool enableColors) {
    _channel = makeConsoleLoggingChannel();
    return instance();
}

void
Log::error(const std::string& message, const char* func, const char* file, int line) {
    if (_level >= Log::Error && _channel)
        _channel->log(buildMessage(Error, message, func, file, line));
}

void
Log::warning(const std::string& message, const char* func, const char* file, int line) {
    if (_level >= Log::Warning && _channel)
        _channel->log(buildMessage(Warning, message, func, file, line));
}

void
Log::info(const std::string& message, const char* func, const char* file, int line) {
    if (_level >= Log::Info && _channel)
        _channel->log(buildMessage(Info, message, func, file, line));
}

void
Log::debug(const std::string& message, const char* func, const char* file, int line) {
    if (_level >= Log::Debug && _channel)
        _channel->log(buildMessage(Debug, message, func, file, line));
}

void
Log::verbose(const std::string& message, const char* func, const char* file, int line) {
    if (_level >= Log::Verbose && _channel)
        _channel->log(buildMessage(Verbose, message, func, file, line));
}

// @todo(jdee): Configure the Poco logger to do this formatting for us.
std::string
Log::buildMessage(Level level, const std::string& message, const char* func, const char* file, int line) {
    string path(file ? file : "");

    static const char* const separator("\\");

    // Just show the last path component
    string::size_type offset = path.find_last_of(separator);
    if (offset != string::npos) path = path.substr(offset + 1);

    ostringstream oss;
    oss << getTimeStamp();
    oss << GetCurrentProcessId() << "|";
    oss << GetCurrentThreadId() << "|";
    oss << level << "|";

    if (!path.empty()) {
        oss << path << ":" << line;
    }

    if (func) {
        oss << "[" << func << "]";
    }

    oss << "|" << message;

    return unescapeFormat(oss.str());
}

std::string
Log::unescapeFormat(const std::string& text) {
    string copy(text);

    /*
     * The logger may interpret any % sign as a printf-style format (e.g. %d), and fails
     * when logging things like URL encodings. This unescapes all % signs by doubling them to %%,
     * producing the correct output.
     */
    string::size_type offset = 0;
    while (offset < copy.length() && (offset = copy.find_first_of("%", offset)) != string::npos) {
        copy.insert(offset, "%");
        offset += 2;
    }

    return copy;
}
LogChannel*
Log::makeFileLoggingChannel(const std::string& path) {
    FileLogChannel* channel = new FileLogChannel(StringUtils::utf8_to_wstring(path).c_str());
    // Roll log files based on size.
    // Keep up to 10 archived log files (e.g. %LocalAppData%\Branch\testbed.log.[0-9]) up to 100 kB each.
    channel->setLogFileMaxSize("100K");
    channel->setLogFileRotationCount(5);
    channel->open();
    return channel;
}

LogChannel*
Log::makeSystemLoggingChannel() {
    // Its not supported.
    return NULL;
}

LogChannel*
Log::makeConsoleLoggingChannel(bool enableColors) {
    LogChannel* channel = new ConsoleLogChannel();
    return channel;
}

/*
 * Get the value of BRANCHIO_LOG_LEVEL, or the default log level
 * by build type (Error for Release, Debug for Debug).s
 */
Log::Level
Log::getDefaultLogLevel() {
    const char* const envValue(getenv("BRANCHIO_LOG_LEVEL"));
    static const Level DefaultLevel(
#ifdef DEBUG
            Debug
#else
            Error
#endif
            );  // NOLINT(whitespace/parens)

    if (!envValue) return DefaultLevel;

    try {
        Level level;
        istringstream iss(envValue);
        iss >> level;
        return level;
    } catch (const exception&) {
        // in case of an invalid level set in the env. var.
        return DefaultLevel;
    }
}

/*
 * Get the value of BRANCHIO_LOG_FILE, if any.
 */
std::string
Log::getDefaultLogFile() {
    const char* const envValue(getenv("BRANCHIO_LOG_FILE"));
    if (!envValue) return string();
    return envValue;
}

std::string Log::getTimeStamp()
{
    stringstream ss;
   
    auto tp1 = chrono::system_clock::now();
    auto time = chrono::system_clock::to_time_t(tp1);
    auto tp2 = chrono::system_clock::from_time_t(time);
    
    if (tp2 > tp1)
        time = chrono::system_clock::to_time_t(tp1 - chrono::seconds(1));
    
    ss << put_time(localtime(&time), "%Y-%m-%d %T") << "." << setfill('0') << setw(3)
        << (chrono::duration_cast<chrono::milliseconds>(tp1.time_since_epoch()).count() % 1000);

    return ss.str();
}

}  // namespace BranchIO
