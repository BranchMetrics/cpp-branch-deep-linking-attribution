#pragma once
#include "Window.h"

class Button :
    public Window
{
public:
    Button(LPCWSTR windowName, int x, int y, int width, int height, HWND parent);
};

