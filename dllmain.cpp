#include "pch.h"

#include "ASM/Hooks.h"
#include "AutoMarket/AutoMarket.h"
#include "Game/Data.h"

#include <lua.hpp>
#include "ucp3.hpp"
#include <string>
#include <Game/UI.h>

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
    if (Game::status->isIngame &&
        !Game::status->isPaused &&
        *Game::playerIndex)
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

std::string fileName = "ucp-automarket.json";

void Initialize()
{
    //HANDLE const hProcess = GetCurrentProcess();
    g_market.Load(fileName);
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
    if (name == "BuyResource") {
        Game::Invoke::BuyResource = (int(__stdcall*)(size_t player, size_t resource, size_t amount)) value;
        return true;
    }
    if (name == "GetResourceCost") {
        Game::Invoke::GetResourceCost = (int(__thiscall*)(Game::U0* self, size_t player, size_t resource, size_t amount)) value;
        return true;
    }
    if (name == "GetResourceValue") {
        Game::Invoke::GetResourceValue = (int(__thiscall*)(Game::U0 * self, size_t player, size_t resource, size_t amount)) value;
        return true;
    }
    if (name == "GetResourceSpace") {
        Game::Invoke::GetResourceSpace = (int(__thiscall*)(Game::U1 * self, size_t player, size_t resource)) value;
        return true;
    }

    // Data
    if (name == "playerIndex") {
        Game::playerIndex = (const size_t *) value;
        return true;
    }
    if (name == "playerData") {
        Game::playerData = (Game::PlayerData *)value;
        return true;
    }
    if (name == "isIngame") {
        Game::isIngame = (const size_t*)value;
        return true;
    }
    if (name == "isPaused") {
        Game::isPaused = (const size_t*)value;
        return true;
    }
    if (name == "ingameTime") {
        Game::ingameTime = (const size_t*)value;
        return true;
    }
    if (name == "ctrlModifier") {
        Game::ctrlModifier = (const size_t*)value;
        return true;
    }
    if (name == "shiftModifier") {
        Game::shiftModifier = (const size_t*)value;
        return true;
    }
    if (name == "altModifier") {
        Game::altModifier = (const size_t*)value;
        return true;
    }
    if (name == "u0") {
        Game::u0 = (Game::U0*)value;
        return true;
    }
    if (name == "u1") {
        Game::u1 = (Game::U1*)value;
        return true;
    }
    if (name == "hWindow") {
        Game::UI::hWindow = (HWND const *) value;
        return true;
    }
    return false;
}


int luaSetConfig(lua_State* L) {
    luaL_argcheck(L, lua_type(L, 1) == LUA_TTABLE, 1, "should be a table");
    if (lua_gettop(L) != 1) return luaL_error(L, "there should be only one argument, %s given", lua_gettop(L));

    lua_pushnil(L);
    while (lua_next(L, 1) != 0) {
        if (lua_type(L, -2) != LUA_TSTRING) {
            return luaL_error(L, "keys must be strings, received a %s", lua_typename(L, lua_type(L, -2)));
        }
        std::string key = lua_tostring(L, -2);

        if (key == "file") {
            std::string candidate = lua_tostring(L, -1);
            if (candidate.size() > 0) {
                fileName = candidate;
            }
            else {
                return luaL_error(L, "value for 'file' not appropriate");
            }
            
        }
        else {
            return luaL_error(L, "An error occurred while setting key: '%s' (unknown key?)", key.c_str());
        }

        /* removes 'value'; keeps 'key' for next iteration */
        lua_pop(L, 1);
    }

    // Return nothing
    return 0;
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
        if (value == 0) {
            return luaL_error(L, "Cannot set address to 0. key: '%s'", key.c_str());
        }
        if (!setAddressForName(key, value)) {
            return luaL_error(L, "An error occurred while setting key: '%s' (unknown key?)", key.c_str());
        }

        /* removes 'value'; keeps 'key' for next iteration */
        lua_pop(L, 1);
    }

    // Return nothing
    return 0;
}

int luaGetAddresses(lua_State* L) {
    lua_createtable(L, 0, 0);

    lua_pushinteger(L, (DWORD) & EscapeCallback);
    lua_setfield(L, -2, "EscapeCallback");

    lua_pushinteger(L, (DWORD)&UpdateCallback);
    lua_setfield(L, -2, "UpdateCallback"); // Triggered after game loop (not every simulated day, but every tick).

    lua_pushinteger(L, (DWORD)&SetIngameStatusCallback);
    lua_setfield(L, -2, "SetIngameStatusCallback"); // Triggered when starting or leaving a game.

    lua_pushinteger(L, (DWORD)&MouseCallback);
    lua_setfield(L, -2, "MouseCallback"); // Triggered on mouse input.

    lua_pushinteger(L, (DWORD)&SystemKeyCallback);
    lua_setfield(L, -2, "SystemKeyCallback");

    return 1;
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

extern "C" int __declspec(dllexport) luaopen_crusaderautomarket(lua_State * L) {
    

    // "debug" log level message
    ucp_log(ucp_NamedVerbosity::Verbosity_1, "CrusaderAutoMarket.dll activated");

    lua_createtable(L, 0, 1);

    lua_pushcfunction(L, luaSetConfig);
    lua_setfield(L, -2, "setConfig");

    lua_pushcfunction(L, luaSetAddresses);
    lua_setfield(L, -2, "setAddresses");

    lua_pushcfunction(L, luaGetAddresses);
    lua_setfield(L, -2, "getAddresses");

    lua_pushcfunction(L, luaInitialize);
    lua_setfield(L, -2, "initialize");
    
    // Returns the table to lua side. Becomes result of call require('crusaderautomarket.dll')
    return 1;
}