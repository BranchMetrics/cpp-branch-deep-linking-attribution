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
	static void setupSDKLogging(const std::string& filename);
	static void openURL(const std::wstring& url);
	static void closeSession();
	static void initBranch(const std::wstring& branchKey, const std::wstring& uriScheme, const std::wstring& windowClass, const std::wstring& initialUrl, TextField* textField);
	static void login(const std::wstring& username);
	static void logout();
	static void logStandardEvent();
	static void logCustomEvent();
	static void getShortURL();
	static void getLongURL();
	static void shutDownBranch();
	static void showInitializationMessage();
	static std::wstring getTrackingButtonLabel();
	static void toggleTracking();
	static std::wstring getIdentity();
	static std::wstring getSessionInfo();

	static BranchIO::Branch* branch;
	static TextField* outputTextField;

private:
	static std::wstring s_branchKey;
	static std::wstring s_uriScheme;
};

