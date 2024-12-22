#include "pch.h"
#include "AutoMarket/UI/EnableButton.h"
#include "AutoMarket/AutoMarket.h"
#include "AutoMarket/UI/Utils.h"
#include "ResourceEdit.h"

// EnableButton is just a normal button with WndProc reroute.
namespace AutoMarket::UI
{

static bool IsKeyAcceptable(int key)
{
    return (key >= '0' && key <= '9') || (key >= VK_PRIOR && key <= VK_HELP) || key == VK_DELETE || key == VK_BACK;
}

static LRESULT CALLBACK ResourceEditWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WNDPROC wndProc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    if (uMsg == WM_CHAR || uMsg == WM_KEYDOWN)
    {
        if (!IsKeyAcceptable(wParam))
        {
            if (HWND hParent = GetParent(hWnd))
            {
                // reroute to parent
                WNDPROC parentWndProc = (WNDPROC)GetWindowLongPtr(hParent, GWL_WNDPROC);
                return CallWindowProc(parentWndProc, hParent, uMsg, wParam, lParam);
            }
            return ERROR_SUCCESS;
        }
    }
    return CallWindowProc(wndProc, hWnd, uMsg, wParam, lParam);
}

HWND CreateResourceEdit(size_t value, Manager& market, int x, int y, size_t w, size_t h, HWND hParent, size_t id)
{
    wchar_t buf[16];
    _ultow_s(value, buf, 10);
    buf[15] = L'\0';

    HWND hEdit = CreateWindow(
        L"Edit", buf,
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_EX_NOPARENTNOTIFY,
        x, y, w, h,
        hParent,
        (HMENU)id,
        NULL, NULL);

    // Reroute the WndProc call.
    WNDPROC wndProc = (WNDPROC)SetWindowLongPtr(hEdit, GWL_WNDPROC, (LONG_PTR)&ResourceEditWndProc);
    SetWindowLongPtr(hEdit, GWLP_USERDATA, (LONG)wndProc);
    return hEdit;
}

size_t GetResourceEditValue(HWND hEdit)
{
    wchar_t buf[16];
    GetWindowText(hEdit, buf, 16);
    return wcstoul(buf, nullptr, 10);
}

void SetResourceEditValue(HWND hWnd, size_t value)
{
    wchar_t buf[16];
    _ultow_s(value, buf, 10);
    buf[15] = L'\0';
    SetWindowText(hWnd, buf);
}

}
