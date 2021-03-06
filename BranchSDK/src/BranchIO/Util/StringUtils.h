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
};

}  // namespace BranchIO

#endif  // BRANCHIO_UTIL_MACADDRESS_H__
