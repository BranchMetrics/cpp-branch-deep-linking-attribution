#include "TextField.h"

#include <cassert>
#include <vector>

#include "Windowsx.h"
#include "Winuser.h"

using namespace std;

TextField::TextField(LPCWSTR text, int x, int y, int width, int height, HMENU menu, HWND parent) :
	Window(WS_EX_TRANSPARENT,
		L"Edit",
		text,
		WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_SIZEBOX | ES_LEFT |
		ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
		x,
		y,
		width,
		height,
		menu,
		parent
	)
{

}

TextField::TextField(LPCWSTR text, int x, int y, int width, int height, long long menu, HWND parent) :
	Window(WS_EX_TRANSPARENT,
		L"Edit",
		text,
		WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_SIZEBOX | ES_LEFT |
		ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
		x,
		y,
		width,
		height,
		menu,
		parent
	)
{

}

void
TextField::setText(const std::wstring& text)
{
	// Prevent multiple threads from clobbering the text area at the same time.
	ScopeLock l(m_lock);
	HWND hwnd = getWindowHandle();
	// May not have been created yet in some cases.
	if (!hwnd) return;
	Edit_SetText(hwnd, text.c_str());
	SendMessage(hwnd, WM_VSCROLL, SB_BOTTOM, 0);
}

std::wstring
TextField::getText() const
{
	HWND hwnd = getWindowHandle();
	DWORD length = Edit_GetTextLength(hwnd);
	// include space for null terminator
	vector<wchar_t> buffer(length + 1);
	Edit_GetText(hwnd, &buffer[0], length + 1);
	return wstring(&buffer[0], &buffer[length]);
}

void
TextField::appendText(const std::wstring& text, size_t maxLength)
{
	// Prevent multiple threads from clobbering the text area at the same time.
	ScopeLock l(m_lock);
	wstring newText = getText() + L"\r\n" + text;
	// Limit text length
	const wstring::size_type total = newText.length();
	if (total > maxLength)
	{
		const size_t offset = total - maxLength;
		newText = newText.substr(offset, maxLength - 1);
	}
	assert(newText.length() <= maxLength);
	setText(newText);
}