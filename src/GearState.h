#pragma once

#include <cstdint>

class Player;

namespace ModBotGear
{
    // Persistent per-bot band state (mod_twow_bot_gear_state table).
    // All calls are safe for any player GUID; absent rows return 0.
    namespace GearState
    {
        uint32 GetLastBand(uint32 characterGuid);
        void   SetLastBand(uint32 characterGuid, uint32 band);
    }
}
