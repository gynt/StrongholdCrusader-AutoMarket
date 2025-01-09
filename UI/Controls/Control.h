#pragma once

#include "UI/ControlPointer.h"

#include <windef.h>

namespace UI
{
class ControlManager;
}

namespace UI
{

// Automatically destroyed by WinApi parent.
class Control : public ControlPointerOwner
{
	enum class EDeletion
	{
		None,
		Operator,
		Parent,
	};

public:
	static ControlManager& GetControlManager();

	Control(RECT const& rect, HWND parent = NULL);
	~Control();

	RECT GetRect() const;
	RECT GetLocalRect() const;
	void SetRect(const RECT& rect);

	HWND GetHandle() const { return m_window; }
	operator HWND() const  { return GetHandle(); }

	Control* GetParent() const;
	bool     IsDescendant(Control const& ancestor) const;

	bool IsHovered() const { return m_isHovered; }
	bool HasFocus() const;

	bool OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual void OnMouseEnter();
	virtual void OnMouseLeave();
	virtual void OnMouseDown(int x, int y);
	virtual void OnMouseUp(int x, int y);

	virtual bool OnKeyDown(int key);
	virtual bool OnChar(int ch);

	virtual void OnPaint(HDC hdcDest, RECT const& rect) const;
	virtual void OnSetFocus();
	virtual void OnLoseFocus();

	void Redraw();

private:
	HWND      m_window;
	EDeletion m_inDeletion;

	bool      m_isHovered;

	friend class ControlManager;
};

}
