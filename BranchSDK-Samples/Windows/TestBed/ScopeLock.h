#pragma once

#include <Windows.h>

class ScopeLock
{
public:
	ScopeLock(CRITICAL_SECTION& lock) : m_lock(lock)
	{
		EnterCriticalSection(&m_lock);
	}

	~ScopeLock()
	{
		LeaveCriticalSection(&m_lock);
	}

private:
	CRITICAL_SECTION& m_lock;
};