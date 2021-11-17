// Copyright (c) 2019-21 Branch Metrics, Inc.

#ifndef BRANCHIO_UTIL_STRINGUTILS_H__
#define BRANCHIO_UTIL_STRINGUTILS_H__

#include <string>


namespace BranchIO {

/**
 * C++ String Utilities
 */
class StringUtils {
 public:

    /**
     * Test if a string "ends with" another string.
     * @param str String input
     * @param suffix String to test
     * @return true if the input ends with the suffix
     */
    static bool endsWith(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
    }

    /**
     * Test if a string "starts with" another string.
     * @param str String input
     * @param prefix String to test
     * @return true if the input starts with the prefix
     */
    static bool startsWith(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
    }

    /**
     * Test if a string "contains" another string.
     * @param str String input
     * @param substring String to test
     * @return true if the input contains the substring
     */
    static bool contains(const std::string& str, const std::string& substring) {
        return str.find(substring) != std::string::npos;
    }

    /**
    * Converts UTF-8 string to wstring.
    * @param string string - input
    * @return wstring - converted string if not empty. If empty or error occurs it returns ""
    */
    static std::wstring utf8_to_wstring(const std::string& string) {
        if (string.empty()){
            return L"";
        }

        const auto size_needed = MultiByteToWideChar(CP_UTF8, 0, &string.at(0), (int)string.size(), nullptr, 0);
        if (size_needed <= 0){
            return L"";
        }

        std::wstring result(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &string.at(0), (int)string.size(), &result.at(0), size_needed);
        return result;
    }

    /**
    * Converts wstring to UTF-8 string.
    * @param wstring wide_string - input
    * @return string - converted string if not empty. If empty or error occurs it returns ""
    */
    // convert wstring to UTF-8 string
    static std::string wstring_to_utf8(const std::wstring& wide_string) {
        if (wide_string.empty()){
            return "";
        }

        const auto size_needed = WideCharToMultiByte(CP_UTF8, 0, &wide_string.at(0), (int)wide_string.size(), nullptr, 0, nullptr, nullptr);
        if (size_needed <= 0){
            return "";
        }

        std::string result(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wide_string.at(0), (int)wide_string.size(), &result.at(0), size_needed, nullptr, nullptr);
        return result;
    }
};

}  // namespace BranchIO

#endif  // BRANCHIO_UTIL_MACADDRESS_H__
