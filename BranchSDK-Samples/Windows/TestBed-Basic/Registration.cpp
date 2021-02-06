#include <Windows.h>
#include "Registration.h"

using namespace std;

/**
 * Set the registry up to allow for this app to be opened via. a web click
 */
void registerApp(const std::wstring& windowClass)
{
    HKEY hKey(nullptr);
    wstring keyBase(L"SOFTWARE\\Classes\\" BRANCH_URI_SCHEME);

    // Step 1:  Create the Base Key and default values
    LONG rc = RegCreateKeyEx(HKEY_CURRENT_USER, keyBase.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
    if (rc != ERROR_SUCCESS) {
        OutputDebugStringA("Unable to create registry key");
        return;
    }

    wstring baseDefaultValue(L"URL:");
    baseDefaultValue += windowClass;
    (void)RegSetValueEx(hKey, TEXT(""), 0, REG_SZ, (LPBYTE)baseDefaultValue.c_str(), keyBase.length() + 1);
    (void)RegSetValueEx(hKey, L"URL Protocol", 0, REG_SZ, LPBYTE(L""), 0);

    // Step 2:  Set the executable location
    TCHAR path[MAX_PATH];
    HMODULE hModule = GetModuleHandleW(NULL);
    GetModuleFileNameW(hModule, path, MAX_PATH);

    HKEY hKeyShell;
    wstring keyShell(L"shell\\open\\command");
    rc = RegCreateKeyEx(hKey, keyShell.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKeyShell, NULL);

    wstring valShell(L"\"");
    valShell += wstring(path) + L"\" \"%1\"";

    rc = RegSetValueEx(hKeyShell, TEXT(""), 0, REG_SZ, LPBYTE(valShell.c_str()), valShell.length() + 1);
    rc = RegCloseKey(hKeyShell);

    // Step 3: Set the default icon
    HKEY hKeyIcon;
    LPCTSTR keyIcon = TEXT("DefaultIcon");
    rc = RegCreateKeyEx(hKey, keyIcon, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKeyIcon, NULL);

    valShell = path;
    valShell += L",1";

    rc = RegSetValueEx(hKeyIcon, TEXT(""), 0, REG_SZ, LPBYTE(valShell.c_str()), valShell.length() + 1);
    rc = RegCloseKey(hKeyIcon);

    // Step 4: Clean Up
    rc = RegCloseKey(hKey);
}