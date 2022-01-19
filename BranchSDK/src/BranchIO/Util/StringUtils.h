// Copyright (c) 2019-21 Branch Metrics, Inc.

#ifndef BRANCHIO_UTIL_STRINGUTILS_H__
#define BRANCHIO_UTIL_STRINGUTILS_H__

#include <codecvt>
#include <string>
#include <Windows.Foundation.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Storage.Streams.h>

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
    static std::string wstring_to_utf8(const std::wstring& wide_string) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
        return myconv.to_bytes(wide_string);
    }

    /**
    * Converts string to URL Encoded format.
    * @param std::string sSrc - input string
    * @return std::string - URL Encoded string
    */
    static std::string UriEncode(const std::string& sSrc)
    {
        const std::string illegal = "%<>{}|\\\"^`!*'()$,[]";
        std::string& encodedstr = std::string();
        const std::string reserved = "?#/:;+@&=";

        for (auto c : sSrc)
        {
            if ((c >= 'a' && c <= 'z') ||
                (c >= 'a' && c <= 'z') ||
                (c >= '0' && c <= '9') ||
                c == '-' || c == '_' ||
                c == '.' || c == '~')
            {
                encodedstr += c;
            }
            else if (c <= 0x20 || c >= 0x7f || illegal.find(c) != std::string::npos || reserved.find(c) != std::string::npos)
            {
                encodedstr += '%';
                std::string result;
                const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
                std::string sss = std::string(1, DEC2HEX[c >> 4]);
                sss.append(std::string(1, (DEC2HEX[c & 0x0F])));
                encodedstr += sss;
            }
            else encodedstr += c;
        }
        return encodedstr;
    }

    /**
    * Converts string to Base64 Encoded format.
    * @param Data - input Data
    * @param DataSize - input Data Size
    * @return std::string - Base64 Encoded string
    */
    static std::string EncodeBase64(const BYTE* Data, SIZE_T DataSize)
    {
        winrt::Windows::Storage::Streams::Buffer Buffer(static_cast<UINT32>(DataSize));
        std::memcpy(Buffer.data(), Data, DataSize);
        Buffer.Length(static_cast<UINT32>(DataSize));
        const winrt::hstring Text = winrt::Windows::Security::Cryptography::CryptographicBuffer::EncodeToBase64String(Buffer);
        return to_string(Text);
    }



};

}  // namespace BranchIO

#endif  // BRANCHIO_UTIL_MACADDRESS_H__
