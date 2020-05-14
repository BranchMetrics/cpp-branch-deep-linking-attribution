// ColorPicker.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "ColorPicker.h"

#include "commctrl.h"
#include "commdlg.h"

#include <BranchIO/Branch.h>
#include <BranchIO/Event/StandardEvent.h>
#include <BranchIO/LinkInfo.h>

#define MAX_LOADSTRING 100

#define ID_CTRL_BASE 1000
#define ID_SHARECOLOR   ID_CTRL_BASE + 0
#define ID_STATUS       ID_CTRL_BASE + 1

// Global Variables:
HINSTANCE hInst;                                // current instance
HWND hwndMain;									// The Main Window
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void createLayout(HWND);
void chooseColor(HWND);
void shareColor(HWND);
void setBackgroundColor(COLORREF color);
void drawBackgroundColor(HDC, PRECT);
void initializeBranch();
void installApp();
void openBranchSession();
void closeBranchSession();

int ColorRefToInt(COLORREF cr);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_COLORPICKER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_COLORPICKER));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass(HINSTANCE)
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_COLORPICKER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_COLORPICKER);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    // Initialize Branch
    initializeBranch();

    hwndMain = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hwndMain)
    {
        return FALSE;
    }

    ShowWindow(hwndMain, nCmdShow);
    UpdateWindow(hwndMain);

    // Layout our window
    createLayout(hwndMain);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        // Start the Branch Session
        openBranchSession();
        break;

    case WM_COMMAND:
    {
        // Parse the menu selections:
        switch (LOWORD(wParam))
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_PICKACOLOR:
            chooseColor(hWnd);
            break;
        case IDM_INSTALL:
            installApp();
            break;
        case ID_SHARECOLOR:
            shareColor(hWnd);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        drawBackgroundColor(hdc, &ps.rcPaint);

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        // End the Branch Session
        closeBranchSession();

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
            break;

        default:
            break;
        }

        break;
    }
    return (INT_PTR)FALSE;
}


#define BYTESIZE(str_len) (sizeof(TCHAR) * str_len)
/**
 * Set the registry up to allow for this app to be opened via. a web click
 */
void installApp()
{
    int bytesize = sizeof(TCHAR);

    HKEY hKey;
    LPCTSTR keyBase = TEXT("SOFTWARE\\Classes\\color");

    // Step 1:  Create the Base Key and default values
    LONG rc = RegCreateKeyEx(HKEY_CURRENT_USER, keyBase, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
    if (rc != ERROR_SUCCESS) {
        OutputDebugStringA("Unable to create registry key");
        return;
    }

    LPCTSTR baseDefaultValue = TEXT("URL:Color Picker");
    rc = RegSetValueEx(hKey, TEXT(""), 0, REG_SZ, (LPBYTE)baseDefaultValue, BYTESIZE(_tcslen(baseDefaultValue) + 1));

    LPCTSTR keyProtocol = TEXT("URL Protocol");
    LPCTSTR valProtocol = TEXT("");
    rc = RegSetValueEx(hKey, keyProtocol, 0, REG_SZ, (LPBYTE)valProtocol, BYTESIZE(_tcslen(valProtocol) + 1));

    // Step 2:  Set the executable location
    TCHAR path[MAX_PATH];

#ifdef UNICODE
    HMODULE hModule = GetModuleHandleW(NULL);
    GetModuleFileNameW(hModule, path, MAX_PATH);
#else
    HMODULE hModule = GetModuleHandleA(NULL);
    GetModuleFileNameA(hModule, path, MAX_PATH);
#endif

    HKEY hKeyShell;
    LPCTSTR keyShell = TEXT("shell\\open\\command");
    rc = RegCreateKeyEx(hKey, keyShell, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKeyShell, NULL);

    TCHAR valShell[MAX_PATH];
    valShell[0] = 0;
        lstrcat(valShell, TEXT("\""));
        lstrcat(valShell, path);
        lstrcat(valShell, TEXT("\" \"%1\""));

    rc = RegSetValueEx(hKeyShell, TEXT(""), 0, REG_SZ, (LPBYTE)valShell, BYTESIZE(_tcslen(valShell) + 1));
    rc = RegCloseKey(hKeyShell);

    // Step 3: Set the default icon
    HKEY hKeyIcon;
    LPCTSTR keyIcon = TEXT("DefaultIcon");
    rc = RegCreateKeyEx(hKey, keyIcon, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKeyIcon, NULL);

    valShell[0] = 0;
        lstrcat(valShell, path);  // Specifically do *not* quote the icon path, as that doesn't work.
        lstrcat(valShell, TEXT(",1"));

    rc = RegSetValueEx(hKeyIcon, TEXT(""), 0, REG_SZ, (LPBYTE)valShell, BYTESIZE(_tcslen(valShell) + 1));
    rc = RegCloseKey(hKeyIcon);

    // Step 4: Clean Up
    rc = RegCloseKey(hKey);
}

// ============================================================================

#define BRANCH_KEY "key_live_efTsR1fbTucbHvX3N5RsOaamDtlPFLap"

BranchIO::Branch* _branchInstance;
BranchIO::AppInfo _appInfo;

HWND hwndShare;
HWND hwndStatus;

#define DEBUGLOG(TAG,STR) \
   OutputDebugStringA(TAG); \
   OutputDebugStringA("\t "); \
   OutputDebugStringA(STR);  \
   OutputDebugStringA("\n");

class MyRequestCallback : public BranchIO::IRequestCallback {
protected:
    virtual void onSuccess(int id, BranchIO::JSONObject jsonResponse)
    {
        DEBUGLOG("Callback Success!  Response: ", jsonResponse.stringify().c_str());
    }

    virtual void onError(int id, int error, std::string description)
    {
        DEBUGLOG("Callback Error!", description.c_str());
    }

    virtual void onStatus(int id, int error, std::string description)
    {
        DEBUGLOG("Status Updated:", description.c_str());
    }
};

class MyOpenCallback : public MyRequestCallback
{
    virtual void onSuccess(int id, BranchIO::JSONObject jsonResponse)
    {
        MyRequestCallback::onSuccess(id, jsonResponse);

        // Dig out the color parameter if it is there
        if (jsonResponse.has("data"))
        {
            BranchIO::JSONObject data = jsonResponse.extract("data");
            if (data.has("extra_color"))
            {
                std::string extra_color = data.get("extra_color");
                DEBUGLOG("Extra Color:", extra_color.c_str());

                COLORREF cr = static_cast<COLORREF>(std::stod(extra_color));

                // There are issues around non-solid colors.  Find a pallette index that is close.
                // Further, Windows Colors are backwards (BGR) instead of RGB...
                // Note the use of creating the RGB from BGR here.
                COLORREF crFixup = PALETTERGB(GetBValue(cr), GetGValue(cr), GetRValue(cr));

                setBackgroundColor(crFixup);
            }
        }
    }
};

class MyShareCallback : public MyRequestCallback
{
public:
    ~MyShareCallback() {
        _linkInfo.cancel();
    }

    void onSuccess(int id, BranchIO::JSONObject jsonResponse)
    {
        MyRequestCallback::onSuccess(id, jsonResponse);

        // Dig out the color parameter if it is there
        if (jsonResponse.has("url"))
        {
            std::string url = jsonResponse.get("url");
            SetWindowTextA(hwndStatus, url.c_str());
        }
    }

    BranchIO::LinkInfo& linkInfo() {
        return _linkInfo;
    }

private:
    BranchIO::LinkInfo _linkInfo;
};

BranchIO::IRequestCallback* _branchCallback = new MyRequestCallback();
MyShareCallback* _shareCallback = new MyShareCallback();

void initializeBranch()
{
    OutputDebugStringW(L"initializeBranch()");

    _appInfo
        .setAppVersion("1.0")
        .setCountryCode("US")
        .setDeveloperIdentity("Branch Metrics")
        .setEnvironment("FULL_APP")
        .setLanguage("en");

    _branchInstance = BranchIO::Branch::create(BRANCH_KEY, &_appInfo);
}

void openBranchSession()
{
    OutputDebugStringW(L"openBranchSession()");

    _branchInstance->openSession("", new MyOpenCallback);
}

void closeBranchSession()
{
    OutputDebugStringW(L"closeBranchSession()");

    _branchInstance->closeSession(_branchCallback);
    delete _branchInstance;
}

// ============================================================================

COLORREF _defaultBackground = RGB(0x00, 0x128, 0x64);
COLORREF _colorBackground = _defaultBackground;
HBRUSH _hBrushBackground = CreateSolidBrush(_colorBackground);

void createLayout(HWND hwnd)
{
    int padding = 10;
    RECT rect;
    ::GetWindowRect(hwnd, &rect);
    ::OffsetRect(&rect, -padding, -padding);

    // Create a share button
    hwndShare = CreateWindowEx(WS_EX_TRANSPARENT, TEXT("Button"), TEXT("Share"),
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, rect.left, rect.top, 60, 60, hwndMain, (HMENU)ID_SHARECOLOR, NULL, NULL);

    // Create a simple text view
    hwndStatus = CreateWindowEx(WS_EX_TRANSPARENT, TEXT("Edit"), TEXT(""),
        WS_CHILD | WS_VISIBLE, rect.left + 60 + padding, rect.top + padding, 260, 20, hwndMain, (HMENU)ID_STATUS, NULL, NULL);

    InvalidateRect(hwnd, NULL, TRUE);

    // Post a message to show the color dialog
    PostMessage(hwnd, WM_COMMAND, MAKELPARAM(IDM_PICKACOLOR, 0), 0);
}

void setBackgroundColor(COLORREF color)
{
    _colorBackground = color;
    _hBrushBackground = CreateSolidBrush(_colorBackground);
    InvalidateRect(hwndMain, NULL, TRUE);
}

/**
 * Paint the rectangle with the currently selected color
 */
void drawBackgroundColor(HDC hdc, PRECT pRect)
{
    HRGN hRgn;

    // Fill the client area with a brush
    hRgn = CreateRectRgnIndirect(pRect);
    FillRgn(hdc, hRgn, _hBrushBackground);
}

/**
 * Show a ColorDialog, and allow the user to pick their color
 */
void chooseColor(HWND hwnd)
{
    static CHOOSECOLOR cc;
    static COLORREF    crCustColors[16];

    ZeroMemory(&cc, sizeof(CHOOSECOLOR));
    cc.lStructSize = sizeof(CHOOSECOLOR);
    cc.hwndOwner = hwnd;
    cc.rgbResult = _colorBackground;
    cc.lpCustColors = crCustColors;
    cc.Flags = CC_RGBINIT | CC_FULLOPEN;

    if (ChooseColor(&cc))
    {
        setBackgroundColor(cc.rgbResult);

        // Just for fun, let's create a Branch event to say the user successfully picked a new color
        BranchIO::StandardEvent event(BranchIO::StandardEvent::Type::COMPLETE_TUTORIAL);
        _branchInstance->sendEvent(event, _branchCallback);
    }
}

void shareColor(HWND hwnd)
{
    // Create a new LinkInfo each time through. It has to outlive the request,
    // so can't create it on the stack here.
    BranchIO::LinkInfo& linkInfo(_shareCallback->linkInfo());

    // Note that Windows Colors are backwards (BGR) instead of RGB...
    // Note the use of creating the RGB from BGR here.

    linkInfo.setFeature("testing");
    linkInfo.addControlParameter("extra_color", ColorRefToInt(_colorBackground));

    // Seed the box with a long URL
    std::string longUrl = linkInfo.createLongUrl(_branchInstance);
    SetWindowTextA(hwndStatus, longUrl.c_str());

    // Create the short URL
    linkInfo.createUrl(_branchInstance, _shareCallback);
}

int ColorRefToInt(COLORREF cr) {
    // Again, COLORREF is BGR.  Switch back to RGB
    COLORREF crFixup = PALETTERGB(GetBValue(cr), GetGValue(cr), GetRValue(cr));
    return static_cast<int>(crFixup);
}



