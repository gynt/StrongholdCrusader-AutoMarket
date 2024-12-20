#include "pch.h"
#include "AutoMarket/UI/Button.h"

namespace AutoMarket::UI
{

HWND CreateButton(wchar_t const* text, int x, int y, size_t w, size_t h, HWND hParent, size_t id)
{
    return CreateWindow(
        L"Button",
        text,
        BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD,
        x, y, w, h,
        hParent,
        (HMENU)id,
        NULL, NULL);
}

}
