#pragma once

class Player;

namespace ModBotGear
{
    // Re-gear orchestration shared by the login hook, level hook, and chat command.
    namespace GearRefresh
    {
        // Load config from sConfig. Safe to call multiple times (e.g. on reload).
        void LoadConfig();

        // Returns true if the bot is a managed bot and eligible under config.
        // isManual=true bypasses the random/owned toggles AND the band check —
        // used by the .bgi chat command (always re-gears).
        bool IsEligible(Player* bot, bool isManual = false);

        // Runs the actual re-gear (roll quality, UpgradeGearBest) and stamps
        // GearState. Assumes IsEligible() returned true.
        void ApplyRefresh(Player* bot);

        // Convenience: IsEligible + ApplyRefresh. Returns true if applied.
        // `context` is used only for log messages ("login", "level", "command").
        // isManual=true is passed through to IsEligible.
        bool TryRefresh(Player* bot, const char* context, bool isManual = false);

        // Config accessors (for logging / diagnostics)
        bool   IsEnabled();
        uint32 GetBandSize();
        uint32 GetMinLevel();
        bool   IsRefreshRandom();
        bool   IsRefreshOwned();
    }
}
