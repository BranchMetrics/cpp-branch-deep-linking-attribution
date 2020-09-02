#pragma once
#include "Window.h"

class Button :
    public Window
{
public:
    Button(LPCWSTR windowName, int x, int y, int width, int height, HMENU menu, HWND parent = nullptr);
    Button(LPCWSTR windowName, int x, int y, int width, int height, int menu, HWND parent = nullptr);

    void onPress();
};

