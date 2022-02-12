#include "FileLogChannel.h"
#include "StringUtils.h"

using namespace std;

namespace BranchIO
{

    FileLogChannel::FileLogChannel(const std::wstring& path):_path(path), _hFile(INVALID_HANDLE_VALUE), _logFileRotationCount(0), _maxFileSize(0)
    {
            
    }

    bool FileLogChannel::ifFileExists(std::wstring filePath)
    {   
        DWORD attr = GetFileAttributes(filePath.c_str());
        if (attr == INVALID_FILE_ATTRIBUTES)
            return false;
        else
            return true;
    }

    FileLogChannel::~FileLogChannel()
    {
        try
        {
            close();
        }
        catch (...)
        {
        }
    }

    bool FileLogChannel::open()
    {
        _hFile = CreateFileW(_path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (_hFile == INVALID_HANDLE_VALUE) 
            return false;
        SetFilePointer(_hFile, 0, 0, FILE_END);
        return true;
    }

    void FileLogChannel::close()
    {
        CloseHandle(_hFile);
        _hFile = INVALID_HANDLE_VALUE;
    }

    void FileLogChannel::log(const std::string& message)
    {
        scoped_lock _l(_mutex);

        if (mustRotate())
        {
            rotateFile();
        }

        if (_hFile == INVALID_HANDLE_VALUE)
        {
            open();
        }

        std::string logText;
        logText.reserve(message.size() + 16); // keep some reserve for \n -> \r\n and terminating \r\n
        for (char c : message)
        {
            if (c == '\n')
                logText += "\r\n";
            else
                logText += c;
        }
        logText += "\r\n";

        DWORD bytesWritten;
        BOOL res = WriteFile(_hFile, logText.data(), static_cast<DWORD>(logText.size()), &bytesWritten, NULL);
        string errorMsg = system_category().message(GetLastError());
        if (!res)
        {
            string errorMsg = system_category().message(GetLastError()) + "\n";
            OutputDebugString(StringUtils::utf8_to_wstring(errorMsg).c_str());
            FlushFileBuffers(_hFile);
        }
    }


    void FileLogChannel::setLogFileRotationCount(int count)
    {
        _logFileRotationCount = count;
    }

    void FileLogChannel::setLogFileMaxSize(const string rotation)
    {
        string::const_iterator it = rotation.begin();
        string::const_iterator end = rotation.end();
        int64_t n = 0;
        while (it != end && isspace(*it)) ++it;
        while (it != end && isdigit(*it)) { n *= 10; n += *it++ - '0'; }
        while (it != end && isspace(*it)) ++it;
        std::string unit;
        while (it != end && isalpha(*it)) unit += *it++;

        if (unit == "K")
            _maxFileSize = n * 1024;
        else if (unit == "M")
            _maxFileSize = n * 1024 * 1024;
        else if (unit.empty())
            _maxFileSize = n;
        else
            _maxFileSize = 0;

    }

    bool FileLogChannel::mustRotate()
    {
        // Check if the current log file has exceeded the maxfilesize limit
        WIN32_FILE_ATTRIBUTE_DATA fad;
        if (GetFileAttributesExW(_path.c_str(), GetFileExInfoStandard, &fad) != 0)
        {
            LARGE_INTEGER li;
            li.LowPart = fad.nFileSizeLow;
            li.HighPart = fad.nFileSizeHigh;

            if (_maxFileSize && (li.QuadPart >= _maxFileSize))
                return true;
        }
        return false;
    }

    void  FileLogChannel::rotateFile()
    {
        std::wstring basePath = _path;
        std::wstring path;
        int n = -1;

        close();

        //Find out what files are already present in the directory
        do
        {
            path = basePath;
            path.append(L".");
            path.append(to_wstring(++n));
        } while (ifFileExists(path));

        while (n >= 0)
        {
            std::wstring oldPath = basePath;

            // Delete last log file if rotation count has exceeded
            // else move the file to new file with 'n' as new suffix (logfile.log.5)
            if (n == (_logFileRotationCount - 1))
            {
                oldPath.append(L".");
                oldPath.append(to_wstring(n - 1));
                DeleteFile(oldPath.c_str());
            }
            else
            {
                if (n > 0)
                {
                    oldPath.append(L".");
                    oldPath.append(to_wstring(n - 1));
                }
                std::wstring newPath = basePath;
                newPath.append(L".");
                newPath.append(to_wstring(n));
                MoveFile(oldPath.c_str(), newPath.c_str());
            }
            --n;
        }   
    }
}