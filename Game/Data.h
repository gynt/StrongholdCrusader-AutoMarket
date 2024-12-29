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

struct Status
{
    size_t ingameTime;
    size_t u0;
    size_t u1;
    size_t isIngame;

    char u3[0x229C];

    size_t isPaused;
};

struct Input
{
    size_t ctrlModifier;
    size_t shiftModifier;
    size_t altModifier;
};

struct U0
{
};

struct U1
{
};

static PlayerData*     playerData  = (PlayerData*)0x0115BFA4;   // 0x011EEBE4
static size_t const* playerIndex = (size_t const*)0x01A275DC; // 0x024BAADC
static Status const* status      = (Status const*)0x01FE7DA8; // 0x02A7B2A8
static Input const*   input       = (Input const*)0x00F224EC;  // 0x00F2296C

static U0* u0 = (U0*)0x0112B0B8; // 0x0112B538
static U1* u1 = (U1*)0x00F98520; // 0x00F989A0

namespace Invoke
{
static auto SellResource     = (void(__stdcall*)(size_t player, size_t resource, size_t amount))0x004CBFA0; //0x004CC1F0
static auto BuyResource      = (int(__stdcall*)(size_t player, size_t resource, size_t amount))0x004CC000;  //0x004CC250
static auto GetResourceCost  = (int(__thiscall*)(U0* self, size_t player, size_t resource, size_t amount))0x004588A0; // 0x00458AD0
static auto GetResourceValue = (int(__thiscall*)(U0* self, size_t player, size_t resource, size_t amount))0x00458910; // 0x00458B40
static auto GetResourceSpace = (int(__thiscall*)(U1* self, size_t player, size_t resource))0x0040C1F0; // 0x0040C200

}

}
