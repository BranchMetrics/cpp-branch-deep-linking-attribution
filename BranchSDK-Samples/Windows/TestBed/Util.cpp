#include "Util.h"

using namespace std;

HANDLE Util::namedMutex(nullptr);

bool
Util::isInstanceRunning()
{
	// Automatically closed when process exits
	namedMutex = CreateMutexW(nullptr, FALSE, MutexName);
	DWORD error = GetLastError();
	return namedMutex && error == ERROR_ALREADY_EXISTS;
}

void
Util::sendToRunningInstance(const wstring& windowClass, const wstring& payload)
{
	COPYDATASTRUCT cds;
	cds.lpData = (void*)payload.c_str();
	cds.cbData = (DWORD)(payload.length() + 1) * sizeof(wchar_t);
	cds.dwData = 'BNC';

	HWND other = FindWindowW(windowClass.c_str(), NULL);
	if (!other)
	{
		return;
	}

	SendMessage(other, WM_COPYDATA, (WPARAM)other, (LPARAM)&cds);
	SetForegroundWindow(other);
}
