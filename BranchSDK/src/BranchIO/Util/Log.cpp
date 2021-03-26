// Copyright (c) 2019-21 Branch Metrics, Inc.

#include "Log.h"

#include <Poco/DateTimeFormatter.h>
#include <Poco/Message.h>
#include <Poco/Process.h>
#include <Poco/String.h>
#include <Poco/Thread.h>
#include <Poco/Timestamp.h>

#ifdef WIN32
#include <Poco/EventLogChannel.h>
#include <Poco/WindowsConsoleChannel.h>
#else
#include <Poco/ConsoleChannel.h>
#include <Poco/SyslogChannel.h>
#endif  // WIN32

#include <Poco/FileChannel.h>

#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>

using namespace std;
using namespace Poco;

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
    using Poco::icompare;
    string svalue;
    s >> svalue;
    if (icompare(svalue, ERROR_LEVEL_NAME) == 0) {
        level = Log::Error;
    } else if (icompare(svalue, WARNING_LEVEL_NAME) == 0) {
        level = Log::Warning;
    } else if (icompare(svalue, INFO_LEVEL_NAME) == 0) {
        level = Log::Info;
    } else if (icompare(svalue, DEBUG_LEVEL_NAME) == 0) {
        level = Log::Debug;
    } else if (icompare(svalue, VERBOSE_LEVEL_NAME) == 0) {
        level = Log::Verbose;
    } else {
        throw std::runtime_error("Invalid log level");
    }

    return s;
}

namespace BranchIO {

#ifdef WIN32
typedef WindowsColorConsoleChannel ConsoleLoggingChannel;
typedef EventLogChannel SystemLoggingChannel;
#else
typedef ColorConsoleChannel ConsoleLoggingChannel;
typedef SyslogChannel SystemLoggingChannel;
#endif  // WIN32

Log&
Log::instance() {
    static Log _instance;
    return _instance;
}

Log::Log() :
    _logger(Poco::Logger::get("BranchIO")) {
    // Set up the channel
    _logger.setLevel(getLoggerPriority(getDefaultLogLevel()));

    string logFile(getDefaultLogFile());
    if (logFile.empty()) {
        _logger.setChannel(makeConsoleLoggingChannel());
    } else {
        _logger.setChannel(makeFileLoggingChannel(logFile));
    }
}

Log::~Log() {
}

Log&
Log::setLevel(Level level) {
    Message::Priority priority(getLoggerPriority(level));
    instance().getLogger().setLevel(priority);
    return instance();
}

Log&
Log::enableFileLogging(const std::string& path) {
    instance().getLogger().setChannel(makeFileLoggingChannel(path));
    return instance();
}

Log&
Log::enableSystemLogging() {
    instance().getLogger().setChannel(makeSystemLoggingChannel());
    return instance();
}

Log&
Log::enableConsoleLogging(bool enableColors) {
    instance().getLogger().setChannel(makeConsoleLoggingChannel(enableColors));
    return instance();
}

void
Log::error(const std::string& message, const char* func, const char* file, int line) {
    poco_error(_logger, buildMessage(Error, message, func, file, line));
}

void
Log::warning(const std::string& message, const char* func, const char* file, int line) {
    poco_warning(_logger, buildMessage(Warning, message, func, file, line));
}

void
Log::info(const std::string& message, const char* func, const char* file, int line) {
    poco_information(_logger, buildMessage(Info, message, func, file, line));
}

void
Log::debug(const std::string& message, const char* func, const char* file, int line) {
    poco_debug(_logger, buildMessage(Debug, message, func, file, line));
}

void
Log::verbose(const std::string& message, const char* func, const char* file, int line) {
    poco_trace(_logger, buildMessage(Verbose, message, func, file, line));
}

// @todo(jdee): Configure the Poco logger to do this formatting for us.
std::string
Log::buildMessage(Level level, const std::string& message, const char* func, const char* file, int line) {
    string path(file ? file : "");

    static const char* const separator(
#ifdef WIN32
        "\\"
#else
        "/"
#endif  // WIN32
    );  // NOLINT(whitespace/parens)

    // Just show the last path component
    string::size_type offset = path.find_last_of(separator);
    if (offset != string::npos) path = path.substr(offset + 1);

    ostringstream oss;
    Timestamp now;
    // https://pocoproject.org/docs/Poco.DateTimeFormatter.html
    oss << DateTimeFormatter::format(now, "%Y-%m-%d-%H:%M:%s") << "|";
    oss << Poco::Process::id() << "|";
    oss << Thread::currentTid() << "|";
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

    string::size_type offset = 0;
    while (offset < copy.length() && (offset = copy.find_first_of("%", offset)) != string::npos) {
        copy.insert(offset, "%");
        offset += 2;
    }

    return copy;
}

Poco::Channel*
Log::makeFileLoggingChannel(const std::string& path) {
    auto* channel = new FileChannel(path);
    // Roll log files based on size.
    // Keep up to 10 archived log files (e.g. %LocalAppData%\Branch\testbed.log.[0-9]) up to 100 kB each.
    // https://pocoproject.org/docs/Poco.FileChannel.html
    channel->setProperty(FileChannel::PROP_ROTATION, "100K");
    channel->setProperty(FileChannel::PROP_PURGECOUNT, "10");
    return channel;
}

Poco::Channel*
Log::makeSystemLoggingChannel() {
    SystemLoggingChannel* channel = new SystemLoggingChannel;
    channel->setProperty("name", "BranchIO");
    return channel;
}

Poco::Channel*
Log::makeConsoleLoggingChannel(bool enableColors) {
    ConsoleLoggingChannel* channel = new ConsoleLoggingChannel;
    channel->setProperty("enableColors", enableColors ? "true" : "false");
    channel->setProperty("errorColor", "magenta");
    channel->setProperty("warningColor", "yellow");
    channel->setProperty("informationColor", "cyan");
    channel->setProperty("debugColor", "default");
    channel->setProperty("traceColor", "lightBlue");
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

/*
 * Convert a Log::Level to a Poco::Message::Priority
 */
Poco::Message::Priority
Log::getLoggerPriority(Level level) {
    switch (level) {
    case Error:
        return Message::PRIO_ERROR;
    case Warning:
        return Message::PRIO_WARNING;
    case Info:
        return Message::PRIO_INFORMATION;
    case Debug:
        return Message::PRIO_DEBUG;
    case Verbose:
        return Message::PRIO_TRACE;
    default:
        return Message::PRIO_ERROR;
    }
}

}  // namespace BranchIO
