#include <BranchIO/String.h>

#include <gtest/gtest.h>

using namespace std;
using namespace BranchIO;

TEST(StringTest, DefaultInitialization) {
    String s;
    ASSERT_EQ(s.str(), "");
}

TEST(StringTest, StringInitialization) {
    string expected("abc");
    String s(expected);
    ASSERT_EQ(s.str(), expected);
}

TEST(StringTest, PointerInitialization) {
    const char* expected("abc");
    String s(expected);
    ASSERT_EQ(s.str(), expected);
}

#ifdef WIN32

TEST(StringTest, WideStringInitialization) {
    wstring expected(L"abc");
    String s(expected);
    ASSERT_EQ(s.wstr(), expected);
}

TEST(StringTest, WidePointerInitialization) {
    const wchar_t* expected(L"abc");
    String s(expected);
    ASSERT_EQ(s.wstr(), expected);
}

TEST(StringTest, ConvertUTF16ToUTF8) {
    wstring original(L"abc");
    String s(original);

    ASSERT_EQ(s.str(), "abc");
}

TEST(StringTest, ConvertUTF8ToUTF16) {
    string original("abc");
    String s(original);

    ASSERT_EQ(s.wstr(), L"abc");
}

#endif  // WIN32
