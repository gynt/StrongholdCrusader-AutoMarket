#include "pch.h"
#include "AutoMarket/UI/EnableButton.h"
#include "AutoMarket/AutoMarket.h"
#include "AutoMarket/UI/Utils.h"

// EnableButton is just a normal button with WndProc reroute.
namespace AutoMarket::UI
{

static LRESULT CALLBACK EnableButtonWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WNDPROC wndProc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    if (uMsg == WM_CHAR || uMsg == WM_KEYDOWN)
    {
        if (HWND hParent = GetParent(hWnd))
        {
            // reroute to parent
            WNDPROC parentWndProc = (WNDPROC)GetWindowLongPtr(hParent, GWL_WNDPROC);
            return CallWindowProc(parentWndProc, hParent, uMsg, wParam, lParam);
        }
    }
    return CallWindowProc(wndProc, hWnd, uMsg, wParam, lParam);
}

HWND CreateEnableButton(Manager& market, int x, int y, size_t w, size_t h, HWND hParent, size_t id)
{
    HWND hButton = CreateWindow(
        L"Button",
        L"Enabled",
        BS_AUTOCHECKBOX | BS_PUSHLIKE | WS_VISIBLE | WS_CHILD,
        x, y, w, h,
        hParent,
        (HMENU)id,
        NULL, NULL);
    SendMessage(hButton, BM_SETCHECK, market.IsEnabled(), NULL);

    // Reroute the WndProc call.
    WNDPROC wndProc = (WNDPROC)SetWindowLongPtr(hButton, GWL_WNDPROC, (LONG_PTR)&EnableButtonWndProc);
    SetWindowLongPtr(hButton, GWLP_USERDATA, (LONG)wndProc);
    return hButton;
}

}
