#include "Window.h"

#include <WinUser.h>

Window::Window(DWORD exStyle, LPCWSTR className, LPCWSTR windowName, DWORD style, int x, int y, int width, int height, HWND parent) :
	m_exStyle(exStyle),
	m_className(className),
	m_windowName(windowName),
	m_style(style),
	m_x(x),
	m_y(y),
	m_width(width),
	m_height(height),
	m_parent(parent),
	m_self(nullptr)
{
	InitializeCriticalSection(&m_lock);
}

Window::~Window()
{
	destroy();

	DeleteCriticalSection(&m_lock);
}

void
Window::destroy()
{
	ScopeLock l(m_lock);
	if (m_self) {
		DestroyWindow(m_self);
		m_self = nullptr;
	}
}

HWND
Window::create()
{
	ScopeLock l(m_lock);
	if (m_self) return m_self;
	if (!m_parent) return nullptr;

	m_self = CreateWindowEx(m_exStyle, m_className.c_str(), m_windowName.c_str(), m_style, m_x, m_y, m_width, m_height, m_parent, NULL, NULL, NULL);
	return m_self;
}

HWND
Window::create(HWND parent)
{
	ScopeLock l(m_lock);
	if (m_self) return m_self;

	m_parent = parent;
	return create();
}
