#pragma once

#include <string>

namespace BranchIO
{
	class Branch;
}

class TextField;

struct BranchOperations
{
	static void setupSDKLogging();
	static void openURL(const std::wstring& url);
	static void closeSession();
	static void initBranch(const std::wstring& initialUrl, TextField* textField);
	static void login(const std::wstring& username);
	static void logout();
	static void logStandardEvent();
	static void logCustomEvent();
	static void getShortURL();
	static void shutDownBranch();
	static void showInitializationMessage();

	static BranchIO::Branch* branch;
	static TextField* outputTextField;

	static constexpr wchar_t const* const BRANCH_KEY = L"key_live_oiT8IkxqCmpGcDT35ttO1fkdExktZD1x";
	static constexpr wchar_t const* const BRANCH_URI_SCHEME = L"testbed:";
};

