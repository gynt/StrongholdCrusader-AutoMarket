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


extern PlayerData* playerData;
extern size_t const* playerIndex;
extern Status const* status;
extern Input const* input;

extern U0* u0;
extern U1* u1;

namespace Invoke
{
    extern void(__stdcall* SellResource)(size_t player, size_t resource, size_t amount);
    extern int(__stdcall* BuyResource)(size_t player, size_t resource, size_t amount);
    extern int(__thiscall* GetResourceCost)(U0* self, size_t player, size_t resource, size_t amount);
    extern int(__thiscall* GetResourceValue)(U0* self, size_t player, size_t resource, size_t amount);
    extern int(__thiscall* GetResourceSpace)(U1* self, size_t player, size_t resource);

}

}
