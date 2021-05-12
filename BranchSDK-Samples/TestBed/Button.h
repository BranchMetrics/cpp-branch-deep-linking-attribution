#pragma once

#include <functional>

#include "Window.h"
#include "ScopeLock.h"

class Button :
    public Window
{
public:
    Button(LPCWSTR text, int x, int y, int width, int height, HMENU menu, HWND parent = nullptr);
    Button(LPCWSTR text, int x, int y, int width, int height, long long menu, HWND parent = nullptr);

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

    void setText(const std::wstring& text);

private:
    std::function<void()> m_buttonPressCallback;
};

