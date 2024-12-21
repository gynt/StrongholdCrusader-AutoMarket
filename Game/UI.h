#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Game::UI
{
static HWND const      hWindow   = *(HWND const*)0x00F983E4; // 0x00F98864
static HINSTANCE const hInstance = (HINSTANCE)GetWindowLongPtr(hWindow, GWLP_HINSTANCE);
}
