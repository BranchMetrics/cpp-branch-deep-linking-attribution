#pragma once
#include "Window.h"

class TextField :
    public Window
{
public:
    TextField(LPCWSTR text, int x, int y, int width, int height, HMENU menu, HWND parent = nullptr);
    TextField(LPCWSTR text, int x, int y, int width, int height, long long menu, HWND parent = nullptr);

    void setText(const std::wstring& text);
    std::wstring getText() const;

    // maxLength limits the character count in the Edit control to avoid growing without bound.
    void appendText(const std::wstring& text, size_t maxLength = 100000);
};
