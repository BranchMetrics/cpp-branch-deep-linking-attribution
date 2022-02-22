#pragma once
#include "LogChannel.h"
#include <mutex>
#include <Windows.h>

namespace BranchIO {

    /**
     * FileLogChannel creates a channel for writing to text log files
     * It dumps simple text messages to log file and adds a newline character at the end.
     * It also supports log file backup and rotation.
     */
    class FileLogChannel : public LogChannel
    {
    public:

        /**
         * Creates the FileLogChannel at the specified path.
         */
        FileLogChannel(const std::wstring& path);
        
        /**
         * Destructor - close and clean everything.
         */
        ~FileLogChannel();

        /**
         * Opens the channel.
         */
        bool open();

        /**
         * Closes the channel.
         */
        void close();

        /**
         * Logs messages to the channel.
         */
        void log(const std::string& message);

        /**
         * Sets log file rotation count. File is rotated when its max size is exceeded.
         * When the number of backup files is exceeded, they are deleted.
         */
        void setLogFileRotationCount(int count);

        /**
         * Sets max size of the log file. When its exceeded, they are rotated.
         * Suffix the value with 'K' (100K) or 'M' (90M) to enter values in KB or MB.
         */
        void setLogFileMaxSize(const std::string rotation);

    private:
        std::wstring  _path;
        std::mutex   _mutex;
        HANDLE      _hFile;
        std::int64_t   _maxFileSize;
        int  _logFileRotationCount;

        bool ifFileExists(std::wstring filePath);
        bool mustRotate();
        void rotateFile();
        bool exists(const std::wstring& name);
    };
}

