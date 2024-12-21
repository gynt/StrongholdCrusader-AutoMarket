#include "pch.h"

#include "ASM/Hooks.h"
#include "AutoMarket/AutoMarket.h"
#include "Game/Data.h"

#include <lua.hpp>
#include "ucp3.hpp"
#include <string>

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

        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

bool setAddressForName(std::string const & name, DWORD const value) {
    if (name == "SellResource") {
        Game::Invoke::SellResource = (void(__stdcall*)(size_t player, size_t resource, size_t amount)) value;
        return true;
    }

    return false;
}

int luaSetAddresses(lua_State* L) {
    luaL_argcheck(L, lua_type(L, 1) == LUA_TTABLE, 1, "should be a table");
    if (lua_gettop(L) != 1) return luaL_error(L, "there should be only one argument, %s given", lua_gettop(L));
    
    lua_pushnil(L);
    while (lua_next(L, 1) != 0) {
        if (lua_type(L, -2) != LUA_TSTRING) {
            return luaL_error(L, "keys must be strings, received a %s", lua_typename(L, lua_type(L, -2)));
        }
        std::string key = lua_tostring(L, -2);
        
        if (lua_type(L, -1) != LUA_TNUMBER) {
            return luaL_error(L, "values must be integers, received a %s for key '%s'", lua_typename(L, lua_type(L, -1)), key.c_str());
        }

        DWORD value = lua_tointeger(L, -1);
        if (!setAddressForName(key, value)) {
            return luaL_error(L, "An error occurred while setting key: '%s' (unknown key?)", key.c_str());
        }

        /* removes 'value'; keeps 'key' for next iteration */
        lua_pop(L, 1);
    }

    // Return nothing
    return 0;
}

bool initialized = false;

int luaInitialize(lua_State* L) {
    if (!initialized) {
        Initialize();
        initialized = true;
    }
    else {
        return luaL_error(L, "dll can only be initialized once");
    }

    lua_pushboolean(L, initialized);
    return 1;
}

int luaopen_crusaderautomarket(lua_State* L) {
    

    // "debug" log level message
    ucp_log(ucp_NamedVerbosity::Verbosity_1, "CrusaderAutoMarket.dll activated");

    lua_createtable(L, 0, 1);

    lua_pushcfunction(L, luaSetAddresses);
    lua_setfield(L, -2, "setAddresses");

    lua_pushcfunction(L, luaInitialize);
    lua_setfield(L, -2, "initialize");
    
    // Returns the table to lua side. Becomes result of call require('crusaderautomarket.dll')
    return 1;
}