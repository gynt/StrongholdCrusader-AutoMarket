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

void     OpenUI();
void     CloseUI();
void     ToggleUI();

}