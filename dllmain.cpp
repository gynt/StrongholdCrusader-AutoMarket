#include "pch.h"

#include "ASM/Hooks.h"
#include "AutoMarket/AutoMarket.h"
#include "Game/Data.h"

HMODULE g_module = NULL;

AutoMarket::Manager g_market;

void __cdecl UpdateCallback(ASM::HookRegisters)
{
    if (*Game::isIngame &&
        !*Game::isPaused &&
        *Game::playerIndex)
    {
        size_t const time = *Game::ingameTime;
        g_market.Update(time);
    }
}

void __cdecl SystemKeyCallback(ASM::HookRegisters registers)
{
    if (*Game::altModifier)
    {
        switch (registers.esi)
        {
        case 'M':
            g_market.Toggle();
            break;
        default:
            break;
        }
    }
}

void __cdecl SetIngameStatusCallback(ASM::HookRegisters registers)
{
    if (registers.edi) // is now ingame?
    {
        g_market.Reset();
    }
    else
    {
        g_market.Close();
    }
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

    ASM::Hook((LPVOID)0x004B354D, 5, &EscapeCallback); // Triggered when hitting escape ingame.
    ASM::Hook((LPVOID)0x0057C3B9, 6, &UpdateCallback); // Triggered after game loop (not every simulated day, but every tick).
    ASM::Hook((LPVOID)0x00512438, 6, &SetIngameStatusCallback); // Triggered when starting or leaving a game.
    ASM::Hook((LPVOID)0x00468030, 5, &MouseCallback); // Triggered on mouse input.
    ASM::Hook((LPVOID)0x004B480B, 8, &SystemKeyCallback);

    // Don't disable the market the pop-up for the player.
    {
        SIZE_T lpNumberOfBytesWritten;
        char lpBuffer[] = { '\x90', '\x90', '\x90', '\x90', '\x90', '\x90' };
        WriteProcessMemory(hProcess, (LPVOID)0x0040A07D, lpBuffer, sizeof(lpBuffer), &lpNumberOfBytesWritten);
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
