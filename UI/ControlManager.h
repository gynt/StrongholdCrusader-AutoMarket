#pragma once

#include <windef.h>

namespace UI
{
class Control;
}

namespace UI
{

class ControlManager
{
public:
	static ControlManager& Get();

	bool WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	HWND GetMainWindow() const;

	void     SetFocus(Control* control);
	Control* GetFocus() const { return m_focus; }

private:
	bool OnKeyDown(int key);
	bool OnChar(int key);

	Control* m_focus;
};

}
