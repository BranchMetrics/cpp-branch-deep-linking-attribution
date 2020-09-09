#pragma once
#include "Window.h"

class TextField :
    public Window
{
public:
    TextField(LPCWSTR text, int x, int y, int width, int height, HMENU menu, HWND parent = nullptr);
    TextField(LPCWSTR text, int x, int y, int width, int height, int menu, HWND parent = nullptr);

    void setText(const std::wstring& text);
    std::wstring getText() const;

    void appendText(const std::wstring& text);
};
