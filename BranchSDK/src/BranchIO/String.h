// Copyright (c) 2020 Branch Metrics, Inc.

#ifndef BRANCHIO_STRING_H__
#define BRANCHIO_STRING_H__

#ifdef WIN32
#include <Windows.h>
#endif  // WIN32
#include <string>
#ifdef WIN32
#include <vector>
#endif  // WIN32

namespace BranchIO {

/**
 * Adapter class to convert UTF-8 to UTF-16 and vice versa. Converts from
 * any of the following types:
 *   std::string
 *   std::wstring
 *   const char*
 *   const wchar_t*
 * Use the str() method to generate a UTF-8 std::string or wstr() to
 * generate a UTF-16 std::wstring.
 */
class String {
 public:
    /**
     * Default constructor and conversion operator from std::string
     * @param s a UTF-8 string
     */
    String(const std::string& s = std::string()) : m_string(s) {}  // NOLINT

    /**
     * Conversion operator from const char*
     * @param s a UTF-8 string
     */
    String(const char* s) : m_string(s) {}  // NOLINT

    /**
     * Obtain a UTF-8 representation of this String
     * @return a UTF-8 string
     */
    std::string str() const { return m_string; }

#ifdef WIN32
    /**
     * Conversion operator from std::wstring
     * @param ws a UTF-16 string
     */
    String(const std::wstring& ws) : m_string(make_string(ws)) {}  // NOLINT

    /**
     * Conversion operator from const wchar_t*
     * @param s a UTF-16 string
     */
    String(const wchar_t* ws) : m_string(make_string(ws)) {}  // NOLINT

    /**
     * Obtain a UTF-16 representation of this String
     * @return a UTF-16 string
     */
    std::wstring wstr() const { return make_wstring(m_string); }

 protected:
    static std::string make_string(const std::wstring& wstr) {
        if (wstr.length() <= 0) {
            return std::string();
        }

        // Allow up to 4 chars per wide char for UTF-8 encoding, plus one for null termination
        std::vector<char> buffer(wstr.length() * 4 + 1);
        int length = WideCharToMultiByte(
            CP_UTF8,
            0,
            wstr.c_str(),
            static_cast<int>(wstr.length()),
            &buffer[0],
            static_cast<int>(buffer.size()) - 1,
            NULL,
            NULL);
        if (length <= 0) {
            return std::string();
        }

        return std::string(&buffer[0], &buffer[length]);
    }

    static std::wstring make_wstring(const std::string& str) {
        if (str.length() <= 0) return std::wstring();

        // Allow one wide char per char for UTF-8
        std::vector<wchar_t> buffer(str.length() + 1);
        int length = MultiByteToWideChar(
            CP_UTF8,
            0,
            str.c_str(),
            static_cast<int>(str.length()),
            &buffer[0],
            static_cast<int>(buffer.size()) - 1);
        if (length <= 0) {
            return std::wstring();
        }

        // look for null termination
        return std::wstring(&buffer[0], &buffer[length]);
    }
#endif  // WIN32

 private:
    std::string m_string;
};

}  // namespace BranchIO

#endif  // BRANCHIO_STRING_H__
