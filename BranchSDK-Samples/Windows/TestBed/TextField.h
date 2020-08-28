#pragma once
#include "Window.h"

class TextField :
    public Window
{
    TextField(LPCWSTR windowName, int x, int y, int width, int height, HWND parent);
};

