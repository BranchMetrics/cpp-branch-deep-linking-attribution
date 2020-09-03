#pragma once

#include <functional>

#include "Window.h"
#include "ScopeLock.h"

class Button :
    public Window
{
public:
    Button(LPCWSTR windowName, int x, int y, int width, int height, HMENU menu, HWND parent = nullptr);
    Button(LPCWSTR windowName, int x, int y, int width, int height, int menu, HWND parent = nullptr);

    void setButtonPressCallback(const std::function<void()>& buttonPressCallback)
    {
        ScopeLock l(m_lock);
        m_buttonPressCallback = buttonPressCallback;
    }

    void onPress()
    {
        ScopeLock l(m_lock);
        m_buttonPressCallback();
    }

private:
    std::function<void()> m_buttonPressCallback;
};

