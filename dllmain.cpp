#include "pch.h"

#include "ASM/Hooks.h"
#include "AutoMarket/AutoMarket.h"
#include "Game/Data.h"

HMODULE g_module = NULL;

AutoMarket::Manager g_market;

static void ToggleAutoMarket()
{
    if (g_market.IsOpened())
    {
        g_market.Close();
    }
    else
    {
        size_t playerIndex = *Game::playerIndex;
        if (playerIndex && Game::status->isIngame && Game::playerData[playerIndex].hasMarket)
        {
            g_market.Open();
        }
    }
}

void __cdecl UpdateCallback(ASM::HookRegisters)
{
    size_t playerIndex = *Game::playerIndex;
    if (playerIndex &&
        Game::status->isIngame &&
        !Game::status->isPaused &&
        Game::playerData[playerIndex].hasMarket)
    {
        size_t const time = Game::status->ingameTime;
        g_market.Update(time);
    }
}

void __cdecl SystemKeyCallback(ASM::HookRegisters registers)
{
    if (Game::input->altModifier)
    {
        switch (registers.esi)
        {
        case 'M':
            ToggleAutoMarket();
            break;
        default:
            break;
        }
    }
}

void __cdecl SetIngameStatusCallback(ASM::HookRegisters)
{
    g_market.Close();
    g_market.Reset();
}

void __cdecl StartGameCallback(ASM::HookRegisters)
{
    g_market.Close();
    g_market.Reset();
}

void __cdecl EscapeCallback(ASM::HookRegisters)
{
    g_market.Close();
}

void __cdecl MouseCallback(ASM::HookRegisters registers)
{
    int const* args = ((int const*)registers.esp) + 1;
    //short const x = args[0];
    //short const y = args[1];
    int const type = args[2];
    if (type >= 1 && type <= 6)  // Mouse button was pressed?
    {
        g_market.Close();
    }
}

void Initialize()
{
    HANDLE const hProcess = GetCurrentProcess();

    ASM::Hook((LPVOID)0x004B354D, 5, &EscapeCallback); // Triggered when hitting escape ingame. // 0x004B36BD
    ASM::Hook((LPVOID)0x0057C3B9, 6, &UpdateCallback); // Triggered after game loop (not every simulated day, but every tick). // 0x0057C7E9
    ASM::Hook((LPVOID)0x00468030, 5, &MouseCallback); // Triggered on mouse input. // 0x00468250
    ASM::Hook((LPVOID)0x004B480B, 8, &SystemKeyCallback); // 0x004B497B

    ASM::Hook((LPVOID)0x00512438, 6, &SetIngameStatusCallback); // Triggered when leaving (or starting?) a game. // 0x005127B8
    ASM::Hook((LPVOID)0x00474A20, 5, &StartGameCallback); // Triggered when starting a game.

    // Don't disable the market the pop-up for the player.
    {
        SIZE_T lpNumberOfBytesWritten;
        char lpBuffer[] = { '\x90', '\x90', '\x90', '\x90', '\x90', '\x90' };
        WriteProcessMemory(hProcess, (LPVOID)0x0040A07D, lpBuffer, sizeof(lpBuffer), &lpNumberOfBytesWritten); // 0x0040A08D
    }

    g_market.Load();
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        g_module = hModule;
        Initialize();
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
