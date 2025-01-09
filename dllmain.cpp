#include "pch.h"

#include "ASM/Hooks.h"
#include "AutoMarket/AutoMarket.h"
#include "UI/ControlManager.h"
#include "Game/UI.h"
#include "Game/Data.h"

#include <lua.hpp>
#include "ucp3.hpp"
#include <string>
#include <Game/UI.h>

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

std::string fileName = "ucp-automarket.json";

void Initialize()
{
    //HANDLE const hProcess = GetCurrentProcess();
    g_market.Load(fileName);
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

bool setAddressForName(std::string const & name, DWORD const value) {
    if (name == "SellResource") {
        Game::Invoke::SellResource = (void(__stdcall*)(size_t player, size_t resource, size_t amount)) value;
        ucp_log(Verbosity_2, "SellResource set");
        return true;
    }
    if (name == "BuyResource") {
        Game::Invoke::BuyResource = (int(__stdcall*)(size_t player, size_t resource, size_t amount)) value;
        ucp_log(Verbosity_2, "BuyResource set");
        return true;
    }
    if (name == "GetResourceCost") {
        Game::Invoke::GetResourceCost = (int(__thiscall*)(Game::U0* self, size_t player, size_t resource, size_t amount)) value;
        ucp_log(Verbosity_2, "GetResourceCost set");
        return true;
    }
    if (name == "GetResourceValue") {
        Game::Invoke::GetResourceValue = (int(__thiscall*)(Game::U0 * self, size_t player, size_t resource, size_t amount)) value;
        ucp_log(Verbosity_2, "GetResourceValue set");
        return true;
    }
    if (name == "GetResourceSpace") {
        Game::Invoke::GetResourceSpace = (int(__thiscall*)(Game::U1 * self, size_t player, size_t resource)) value;
        ucp_log(Verbosity_2, "GetResourceSpace set");
        return true;
    }

    // Data
    if (name == "playerIndex") {
        Game::playerIndex = (const size_t *) value;
        ucp_log(Verbosity_2, "playerIndex set");
        return true;
    }
    if (name == "playerData") {
        Game::playerData = (Game::PlayerData *)value;
        ucp_log(Verbosity_2, "playerData set");
        return true;
    }
    if (name == "ingameTime") {
        Game::status = (const Game::Status*)value;
        ucp_log(Verbosity_2, "ingameTime set");
        return true;
    }
    if (name == "ctrlModifier") {
        Game::input = (const Game::Input*)value;
        ucp_log(Verbosity_2, "ctrlModifier set");
        return true;
    }
    if (name == "u0") {
        Game::u0 = (Game::U0*)value;
        ucp_log(Verbosity_2, "u0 set");
        return true;
    }
    if (name == "u1") {
        Game::u1 = (Game::U1*)value;
        ucp_log(Verbosity_2, "u1 set");
        return true;
    }
    if (name == "hWindow") {
        Game::UI::hWindow = (HWND *) value;
        ucp_log(Verbosity_2, "hWindow set");
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