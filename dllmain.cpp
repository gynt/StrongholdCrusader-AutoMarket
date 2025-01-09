#include "pch.h"

#include "ASM/Hooks.h"
#include "AutoMarket/AutoMarket.h"
#include "UI/ControlManager.h"
#include "Game/UI.h"
#include "Game/Data.h"

HMODULE g_module = NULL;

static bool    s_initialized{ false };
static WNDPROC s_oldWndProc{ NULL };
static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_SYSKEYDOWN:        
        if (Game::playerIndex && Game::status->isIngame &&
            Game::input->altModifier && wParam == 'M')
        {
            AutoMarket::ToggleUI();
        }
        break;
    default:
        break;
    }

    if (UI::ControlManager::Get().WndProc(hWnd, uMsg, wParam, lParam))
    {
        return TRUE;
    }

    return CallWindowProc(s_oldWndProc, hWnd, uMsg, wParam, lParam);
}

static void Register()
{
    if (!s_initialized)
    {
        s_initialized = true;
        s_oldWndProc = (WNDPROC)SetWindowLongPtr(*Game::UI::hWindow, GWLP_WNDPROC, (LONG_PTR)&WndProc);
    }
}

static void Deregister()
{
    if (s_initialized)
    {
        SetWindowLongPtr(*Game::UI::hWindow, GWLP_WNDPROC, (LONG_PTR)s_oldWndProc);
    }
}

static void __cdecl UpdateGameCallback(ASM::HookRegisters)
{
    AutoMarket::Update();
}

static void __cdecl EnterLeaveGameCallback(ASM::HookRegisters)
{
    Register(); // Do late registering.

    AutoMarket::Reset();
}

static void InitializeHooks()
{
    HANDLE const hProcess = GetCurrentProcess();

    ASM::Hook((LPVOID)0x0057C3B9, 6, &UpdateGameCallback); // Triggered after game loop (not every simulated day, but every tick). // 0x0057C7E9
    ASM::Hook((LPVOID)0x00512438, 6, &EnterLeaveGameCallback); // Triggered when leaving (or starting?) a game. // 0x005127B8
    ASM::Hook((LPVOID)0x00474A20, 5, &EnterLeaveGameCallback); // Triggered when starting a game.

    // Don't disable the market the pop-up for the player.
    {
        SIZE_T lpNumberOfBytesWritten;
        char lpBuffer[] = { '\x90', '\x90', '\x90', '\x90', '\x90', '\x90' };
        WriteProcessMemory(hProcess, (LPVOID)0x0040A07D, lpBuffer, sizeof(lpBuffer), &lpNumberOfBytesWritten); // 0x0040A08D
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        g_module = hModule;
        InitializeHooks();
        if (Game::status->isIngame)
        {
            Register(); // Otherwise, we do late registering.
        }
        break;
    case DLL_PROCESS_DETACH:
        Deregister();
        break;
    }
    return TRUE;
}
