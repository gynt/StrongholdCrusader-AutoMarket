#pragma once

namespace AutoMarket
{
class Manager;
}

namespace AutoMarket
{
Manager& GetManager();
void     Reset();
void     Update();

bool     OpenUI();
bool     CloseUI();
bool     ToggleUI();

}