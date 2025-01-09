#include "pch.h"
#include "UI/Controls/Control.h"

#include "UI/ControlManager.h"
#include "Game/UI.h"

namespace UI
{

static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Control* control;
    if (uMsg == WM_CREATE)
    {
        control = (Control*)((LPCREATESTRUCT)lParam)->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)control);
    }
    else
    {
        control = (Control*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    }

    if (!control || !control->OnEvent(uMsg, wParam, lParam))
    {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return TRUE;
}

static HWND ConstructWindow(Control& control, RECT const& rect, HWND parent)
{
    static constexpr wchar_t s_className[] = L"CustomControl";

    static bool s_registered = false;
    if (!s_registered)
    {
        WNDCLASS wc = { 0 };
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpszClassName = s_className;
        wc.hInstance = (HINSTANCE)GetWindowLongPtr(*Game::UI::hWindow, GWLP_HINSTANCE);
        wc.lpfnWndProc = WndProc;
        wc.hCursor = NULL;
        if (!RegisterClass(&wc))
        {
            return NULL;
        }

        s_registered = true;
    }

    HWND window = CreateWindow(
        s_className,
        NULL,
        WS_VISIBLE | WS_CHILD,
        rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
        parent,
        NULL,
        NULL,
        &control);


    return window;
}

ControlManager& Control::GetControlManager()
{
    return ControlManager::Get();
}

Control::Control(RECT const& rect, HWND parent)
    : m_window(ConstructWindow(*this, rect, parent))
    , m_inDeletion(EDeletion::None)
    , m_isHovered(false)
{
}

Control::~Control()
{
    if (m_inDeletion == EDeletion::None)
    {
        m_inDeletion = EDeletion::Operator;
        DestroyWindow(m_window);
    }

    if (HasFocus())
    {
        GetControlManager().SetFocus(nullptr);
    }

    m_window = NULL;
}

RECT Control::GetRect() const
{
    RECT result;
    GetWindowRect(m_window, &result);
    return result;
}

RECT Control::GetLocalRect() const
{
    RECT rect;
    GetClientRect(m_window, &rect);
    return rect;
}

void Control::SetRect(RECT const& rect)
{
    MoveWindow(m_window, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, true);
}

Control* Control::GetParent() const
{
    if (!m_window)
    {
        return nullptr;
    }

    HWND hParent = ::GetParent(m_window);
    if (!hParent)
    {
        return nullptr;
    }

    if (hParent == GetControlManager().GetMainWindow())
    {
        return nullptr;
    }

    return (Control*)GetWindowLongPtr(hParent, GWLP_USERDATA);
}

bool Control::IsDescendant(Control const& ancestor) const
{
    Control const* current = this;
    while (Control const* parent = current->GetParent())
    {
        if (parent == &ancestor)
        {
            return true;
        }
        current = parent;
    }
    return false;
}

bool Control::HasFocus() const
{ 
    return GetControlManager().GetFocus() == this;
}

bool Control::OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        break;
    case WM_DESTROY:
        if (m_inDeletion == EDeletion::None)
        {
            m_inDeletion = EDeletion::Parent;
            delete this;
        }
        break;
    case WM_MOUSEMOVE:
        if (!m_isHovered)
        {
            m_isHovered = true;
            OnMouseEnter();

            if (m_window)
            {
                TRACKMOUSEEVENT tme = {};
                tme.cbSize = sizeof(TRACKMOUSEEVENT);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = m_window;
                tme.dwHoverTime = HOVER_DEFAULT;
                TrackMouseEvent(&tme);
            }
        }
        break;
    case WM_MOUSELEAVE:
        if (m_isHovered)
        {
            m_isHovered = false;
            OnMouseLeave();
        }
        break;
    case WM_LBUTTONDOWN:
        GetControlManager().SetFocus(this);
        OnMouseDown(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_LBUTTONUP:
        OnMouseUp(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_KEYDOWN:
        OnKeyDown(wParam);
        break;
    case WM_PAINT:
        if (m_window)
        {
            PAINTSTRUCT ps;
            if (HDC hdc = BeginPaint(m_window, &ps))
            {
                OnPaint(hdc, GetLocalRect());
            }
            EndPaint(m_window, &ps);
        }
        break;
    default:
        return false;
    }
    return true;
}

void Control::OnMouseEnter()
{
}

void Control::OnMouseLeave()
{
}

void Control::OnMouseDown(int x, int y)
{
}

void Control::OnMouseUp(int x, int y)
{
}

bool Control::OnKeyDown(int key)
{
    return false;
}

bool Control::OnChar(int ch)
{
    return false;
}

void Control::OnPaint(HDC hdcDest, RECT const& rect) const
{
}

void Control::OnSetFocus()
{
}

void Control::OnLoseFocus()
{
}

void Control::Redraw()
{
    if (!m_window)
    {
        return;
    }

    // Redraw the root parent control to handle transparency correctly.
    Control* topWindow = this;
    while (Control* parent = topWindow->GetParent())
    {
        topWindow = parent;
    }

    RECT rect;
    GetWindowRect(m_window, &rect);
    MapWindowPoints(HWND_DESKTOP, topWindow->GetHandle(), (LPPOINT)&rect, 2);

    // Invalidate the root, but only our rect.
    InvalidateRect(topWindow->GetHandle(), &rect, TRUE);
}

}
