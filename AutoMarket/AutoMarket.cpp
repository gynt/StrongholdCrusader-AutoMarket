#include "pch.h"
#include "AutoMarket/AutoMarket.h"

#include "AutoMarket/Window.h"
#include "AutoMarket/Manager.h"
#include "Game/Data.h"
#include "UI/ControlPointer.h"
#include "UI/ControlManager.h"

namespace AutoMarket
{

static UI::ControlPointer<Window> s_ui;

Manager& GetManager()
{
    static std::unique_ptr<Manager> s_manager;
    if (!s_manager)
    {
        s_manager = std::make_unique<Manager>();
    }
    return *s_manager.get();
}

void OpenUI()
{
    if (s_ui)
    {
        return;
    }

    size_t playerIndex = *Game::playerIndex;
    if (!playerIndex || !Game::status->isIngame || !Game::playerData[playerIndex].hasMarket)
    {
        return;
    }

    UI::ControlManager& controlManager = Window::GetControlManager();
    s_ui = new Window(GetManager(), controlManager.GetMainWindow());
    s_ui->onExit = []() { GetManager().Save(); };
    controlManager.SetFocus(s_ui.Get());
}

void CloseUI()
{
    if (!s_ui)
    {
        return;
    }

    delete s_ui.Get();
}

void ToggleUI()
{
    if (s_ui)
    {
        CloseUI();
    }
    else
    {
        OpenUI();
    }
}

void Reset()
{
    CloseUI();
    GetManager().Reset();
}

void Update()
{
    size_t playerIndex = *Game::playerIndex;
    if (!playerIndex ||
        !Game::status->isIngame ||
        Game::status->isPaused ||
        !Game::playerData[playerIndex].hasMarket)
    {
        return;
    }

    GetManager().Update(Game::status->ingameTime);
}

}
