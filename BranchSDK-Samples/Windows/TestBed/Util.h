#pragma once

#include <functional>
#include <string>

#include <Windows.h>

struct Util
{
	static bool isInstanceRunning();
	static void sendToRunningInstance(const std::wstring& windowClass, const std::wstring& payload);

private:
	static constexpr wchar_t const* const MutexName = L"68A98988-CF40-41B6-83A7-AA7B17AC9CD5";
	static HANDLE namedMutex;
};
