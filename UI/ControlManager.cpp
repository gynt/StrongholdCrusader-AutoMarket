#include "pch.h"
#include "UI/ControlManager.h"

#include "Game/UI.h"
#include "UI/Controls/Control.h"

#include <memory>

namespace UI
{

ControlManager& ControlManager::Get()
{
    static std::unique_ptr<ControlManager> s_manager;
    if (!s_manager)
    {
        s_manager = std::make_unique<ControlManager>();
    }
    return *s_manager.get();
}

bool ControlManager::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_KEYDOWN: // Our controls never have WinApi focus, so reroute key presses here.
        if (OnKeyDown(wParam))
        {
            return true;
        }
        break;
    case WM_CHAR: // Our controls never have WinApi focus, so reroute char inputs here.
        if (OnChar(wParam))
        {
            return true;
        }
        break;
    case WM_LBUTTONDOWN: // Pressed outside our controls, so reset the focus.
        SetFocus(nullptr);
        break;
    }
    return false;
}

HWND ControlManager::GetMainWindow() const
{
    return *Game::UI::hWindow;
}

void ControlManager::SetFocus(Control* control)
{
    if (m_focus == control)
    {
        return;
    }

    Control* oldFocus = std::exchange(m_focus, control);
    if (oldFocus)
    {
        oldFocus->OnLoseFocus();
    }
    if (control)
    {
        control->OnSetFocus();
    }
}

bool ControlManager::OnKeyDown(int key)
{
    Control* control = m_focus;
    while (control)
    {
        if (control->OnKeyDown(key))
        {
            return true;
        }
        control = control->GetParent(); // Send key presses up the parent chain.
    }
    return false;
}

bool ControlManager::OnChar(int key)
{
    Control* control = m_focus;
    while (control)
    {
        if (control->OnChar(key))
        {
            return true;
        }
        control = control->GetParent(); // Send char inputs up the parent chain.
    }
    return false;
}

}
