#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <utility>

namespace UI
{

// RAII for Windows GDI objects.
template<typename THandle>
class UniqueObject
{
public:
	UniqueObject(THandle handle = NULL)
		: m_handle(handle)
	{
	}

	UniqueObject(UniqueObject&& other)
		: m_handle(NULL)
	{
		std::swap(m_handle, other.m_handle);
	}

	UniqueObject& operator=(UniqueObject&& other)
	{
		std::swap(m_handle, other.m_handle);
		return *this;
	}

	UniqueObject(const UniqueObject& other) = delete;
	UniqueObject& operator=(const UniqueObject& other) = delete;

	~UniqueObject()
	{
		DeleteObject(m_handle);
	}

	bool IsValid() const { return m_handle != NULL; }
	THandle Get() const { return m_handle; }

	operator bool() const { return IsValid(); }
	operator THandle() const { return Get(); }

private:
	THandle m_handle;
};

}