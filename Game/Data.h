#pragma once

#include "Game/Resources.h"

#include <stdint.h>

namespace Game
{

struct PlayerData
{
    union
    {
        char rawData[0x39F4];
        struct
        {
            size_t hasMarket;

            char u0[0x320];

            size_t resources[Game::Resource::Max];
        };
    };
};

struct U0
{
};

struct U1
{
};

static size_t const* const playerIndex   = (size_t const*)0x1A275DC;
static PlayerData* const   playerData    = (PlayerData*)0x115BFA4;

static size_t const* const isIngame      = (size_t const*)0x01FE7DB4;
static size_t const* const isPaused      = (size_t const*)0x01FEA054;
static size_t const* const ingameTime    = (size_t const*)0x01FE7DA8;

static size_t const* const ctrlModifier  = (size_t const*)0xF224EC;
static size_t const* const shiftModifier = (size_t const*)0xF224F0;
static size_t const* const altModifier   = (size_t const*)0xF224F4;

static U0* const u0 = (U0*)0x0112B0B8;
static U1* const u1 = (U1*)0x00F98520;

namespace Invoke
{
static auto const SellResource     = (void(__stdcall*)(size_t player, size_t resource, size_t amount))0x004CBFA0;
static auto const BuyResource      = (int(__stdcall*)(size_t player, size_t resource, size_t amount))0x004CC000;
static auto const GetResourceCost  = (int(__thiscall*)(U0* self, size_t player, size_t resource, size_t amount))0x004588A0;
static auto const GetResourceValue = (int(__thiscall*)(U0* self, size_t player, size_t resource, size_t amount))0x00458910;
static auto const GetResourceSpace = (int(__thiscall*)(U1* self, size_t player, size_t resource))0x0040C1F0;
}

}
