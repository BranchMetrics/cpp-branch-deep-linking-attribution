// Copyright (c) 2019-21 Branch Metrics, Inc.

#ifndef BRANCHIO_UTIL_STRINGUTILS_H__
#define BRANCHIO_UTIL_STRINGUTILS_H__

#include <codecvt>
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
        std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
        return myconv.from_bytes(string);
    }

    /**
    * Converts wstring to UTF-8 string.
    * @param wstring wide_string - input
    * @return string - converted string if not empty. If empty or error occurs it returns ""
    */
    // convert wstring to UTF-8 string
    static std::string wstring_to_utf8(const std::wstring& wide_string) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
        return myconv.to_bytes(wide_string);
    }
};

}  // namespace BranchIO

#endif  // BRANCHIO_UTIL_MACADDRESS_H__
