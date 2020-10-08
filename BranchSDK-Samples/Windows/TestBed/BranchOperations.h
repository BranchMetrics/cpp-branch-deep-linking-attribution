#pragma once

#include <string>

#include <Windows.h>

#include <BranchIO/JSONObject.h>

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
	static void waitForOpen(DWORD dwMilliseconds);
	static void login(const std::wstring& username);
	static void logout();
	static void logStandardEvent();
	static void logCustomEvent();
	static void getShortURL();
	static void shutDownBranch();
	static void showInitializationMessage();
	static BranchIO::JSONObject getOpenResponse();
	static std::wstring getTrackingButtonLabel();
	static void toggleTracking();
	static std::wstring getIdentity();
	static std::wstring getSessionInfo();

	static BranchIO::Branch* branch;
	static TextField* outputTextField;

	static constexpr wchar_t const* const BRANCH_KEY = L"key_live_oiT8IkxqCmpGcDT35ttO1fkdExktZD1x";
	static constexpr wchar_t const* const BRANCH_URI_SCHEME = L"testbed:";

private:
	static bool volatile openComplete;
	static BranchIO::JSONObject openResponse;
	static CRITICAL_SECTION lock;
	static CONDITION_VARIABLE openCompleteCondition;
};

