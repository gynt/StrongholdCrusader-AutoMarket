#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <functional>

namespace AutoMarket
{
class Manager;
}

namespace AutoMarket::UI
{
HWND Open(Manager& market, std::function<void()> onExit);
void Close(HWND hWnd);
}
