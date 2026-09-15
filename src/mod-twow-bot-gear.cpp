// mod-twow-bot-gear
//
// Re-gears managed bots when they ding into a new level band, log in already
// at a new band, or via the `.bgi` chat command.
//
// Gear selection (class/spec/slot/quality) is handled by PlayerbotFactory.
// This module only decides WHEN and WHICH bots to re-gear, plus the quality roll.

#include "botpch.h"

#include "ScriptObjects.h"
#include "ScriptMgr.h"
#include "Player.h"
#include "Log.h"
#include "Config/Config.h"

#include "GearRefresh.h"

namespace
{
    // World lifecycle: load config on startup and reload
    class BotGearWorldScript : public WorldScript
    {
    public:
        BotGearWorldScript()
            : WorldScript("mod-twow-bot-gear_world",
                          { WORLDHOOK_ON_STARTUP, WORLDHOOK_ON_AFTER_CONFIG_LOAD })
        {}

        void OnStartup() override
        {
            ModBotGear::GearRefresh::LoadConfig();
            sLog.outString("[mod-twow-bot-gear] loaded. Enable=%d BandSize=%u MinLevel=%u Random=%d Owned=%d",
                           ModBotGear::GearRefresh::IsEnabled() ? 1 : 0,
                           ModBotGear::GearRefresh::GetBandSize(),
                           ModBotGear::GearRefresh::GetMinLevel(),
                           ModBotGear::GearRefresh::IsRefreshRandom() ? 1 : 0,
                           ModBotGear::GearRefresh::IsRefreshOwned() ? 1 : 0);
        }

        void OnAfterConfigLoad(bool /*reload*/) override
        {
            ModBotGear::GearRefresh::LoadConfig();
        }
    };

    // Player lifecycle: on login, catch bots already at a new band (retroactive)
    class BotGearPlayerScript : public PlayerScript
    {
    public:
        BotGearPlayerScript()
            : PlayerScript("mod-twow-bot-gear_player",
                           { PLAYERHOOK_ON_LOGIN, PLAYERHOOK_ON_LEVEL_CHANGED })
        {}

        void OnLogin(Player* player) override
        {
            ModBotGear::GearRefresh::TryRefresh(player, "login");
        }

        void OnLevelChanged(Player* player, uint8 /*oldLevel*/) override
        {
            ModBotGear::GearRefresh::TryRefresh(player, "level");
        }
    };
}

void Addmod_twow_bot_gearScripts()
{
    new BotGearWorldScript();
    new BotGearPlayerScript();

    extern void AddModBotGearCommandScript();
    AddModBotGearCommandScript();
}
