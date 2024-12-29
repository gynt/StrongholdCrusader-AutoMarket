#include "pch.h"
#include "Game/Data.h"

namespace Game
{

    PlayerData* playerData = (PlayerData*)0; // 0x0115BFA4;   // 0x011EEBE4
    size_t const* playerIndex = (size_t const*)0; // 0x01A275DC; // 0x024BAADC
    Status const* status = (Status const*)0; // 0x01FE7DA8; // 0x02A7B2A8
    Input const* input = (Input const*)0; // 0x00F224EC;  // 0x00F2296C

    U0* u0 = (U0*)0; // 0x0112B0B8; // 0x0112B538
    U1* u1 = (U1*)0; // 0x00F98520; // 0x00F989A0

    namespace Invoke
    {
        void(__stdcall* SellResource)(size_t player, size_t resource, size_t amount)  = 0; // 0x004CBFA0; //0x004CC1F0
        int(__stdcall* BuyResource)(size_t player, size_t resource, size_t amount)  = 0; // 0x004CC000;  //0x004CC250
        int(__thiscall* GetResourceCost)(U0* self, size_t player, size_t resource, size_t amount)  = 0; // 0x004588A0; // 0x00458AD0
        int(__thiscall* GetResourceValue)(U0* self, size_t player, size_t resource, size_t amount)  = 0; // 0x00458910; // 0x00458B40
        int(__thiscall* GetResourceSpace)(U1* self, size_t player, size_t resource)  = 0; // 0x0040C1F0; // 0x0040C200

    }

}
