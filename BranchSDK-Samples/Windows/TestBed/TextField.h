#pragma once
#include "Window.h"

class TextField :
    public Window
{
public:
    TextField(LPCWSTR windowName, int x, int y, int width, int height, HWND parent = nullptr);
};

