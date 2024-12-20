#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <xutility>

namespace AutoMarket::UI
{

HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent);

HWND    FindChildWindowById(HWND hParent, HMENU id);

}
