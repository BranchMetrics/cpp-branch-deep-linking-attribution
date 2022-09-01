#pragma once
#include "LogChannel.h"
#include <mutex>
#include <Windows.h>

namespace BranchIO {

    /**
     * ConsoleLogChannel creates a channel for writing to console.
     * It dumps simple text messages to console and adds a newline character at the end.
     */
    class ConsoleLogChannel : public LogChannel
    {
    public:
        void log(const std::string& message);
    private:
        std::mutex   _mutex;
    };
}

