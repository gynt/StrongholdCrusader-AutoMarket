#pragma once

#include <windef.h>
#include <functional>

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

	std::function<void(Control*)> onChangeFocus;

private:
	bool OnKeyDown(int key);
	bool OnChar(int key);

	Control* m_focus;
};

}
