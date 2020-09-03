#pragma once
#include "Window.h"

class TextField :
    public Window
{
public:
    TextField(LPCWSTR windowName, int x, int y, int width, int height, HMENU menu, HWND parent = nullptr);
    TextField(LPCWSTR windowName, int x, int y, int width, int height, int menu, HWND parent = nullptr);

    HWND create(HWND parent);

    void setText(const std::wstring& text);
    std::wstring getText() const;
};

