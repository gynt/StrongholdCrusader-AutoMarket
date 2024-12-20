#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace AutoMarket
{
class Manager;
}

namespace AutoMarket::UI
{

HWND CreateEnableButton(Manager& market, int x, int y, size_t w, size_t h, HWND hParent, size_t id);

}
