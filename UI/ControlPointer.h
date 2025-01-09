#pragma once

#include <memory>

// Crude implementation akin to QT's QPointer functionality.
// If the object is deleted, the pointer will be nulled automatically.
namespace UI
{

class ControlPointerOwner
{
	struct Data {};

	std::shared_ptr<Data> GetData() const
	{
		if (!m_data)
		{
			m_data = std::make_shared<Data>();
		}
		return m_data;
	}

	mutable std::shared_ptr<Data> m_data;

	friend class ControlPointerBase;
};

class ControlPointerBase
{
protected:
	template<typename T, std::enable_if_t<std::is_base_of_v<ControlPointerOwner, T>, bool> = true>
	ControlPointerBase(T* control)
		: m_data(control ? control->GetData() : std::weak_ptr<ControlPointerOwner::Data>())
	{
	}

	std::weak_ptr<ControlPointerOwner::Data> m_data;
};

template<typename T>
class ControlPointer : ControlPointerBase
{
public:
	ControlPointer(T* control = nullptr)
		: ControlPointerBase(control)
		, m_ptr(control)
	{
	}

	bool IsNull() const { return m_data.expired(); }
	T*   Get() const { return m_ptr; }

	T* operator->() const { return Get(); }
	T& operator*() const { return *Get(); }
	operator T* () const { return !IsNull() ? Get() : nullptr; }

protected:
	T* m_ptr;
};

}
