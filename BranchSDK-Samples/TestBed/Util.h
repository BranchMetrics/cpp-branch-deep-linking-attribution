#pragma once

#include <string>

#include <Windows.h>

struct Util
{
	static bool isInstanceRunning();
	static void sendToRunningInstance(const std::wstring& windowClass, const std::wstring& payload);

private:
	static constexpr wchar_t const* const MutexName = MUTEX_NAME;
	static HANDLE namedMutex;
};
