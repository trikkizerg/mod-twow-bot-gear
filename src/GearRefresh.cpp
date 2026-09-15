#include "botpch.h"

#include "GearRefresh.h"
#include "GearState.h"

#include "Player.h"
#include "Log.h"
#include "Util.h"
#include "Config/Config.h"

#include "Policies/Singleton.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotFactory.h"
#include "BotManager.h"

namespace ModBotGear { namespace GearRefresh
{

namespace
{
    struct BotGearConfig
    {
        bool   enable;
        uint32 bandSize;
        uint32 minLevel;
        uint32 minQuality;
        uint32 maxQuality;
        bool   refreshRandom;
        bool   refreshOwned;
        bool   logRefresh;
    };

    BotGearConfig g_cfg = {};

    uint32 RollQuality()
    {
        if (g_cfg.minQuality == 0)
            return 0;
        if (g_cfg.maxQuality < g_cfg.minQuality)
            return g_cfg.minQuality;
        if (g_cfg.minQuality == g_cfg.maxQuality)
            return g_cfg.minQuality;
        return urand(g_cfg.minQuality, g_cfg.maxQuality);
    }
}

void LoadConfig()
{
    g_cfg.enable        = sConfig.GetBoolDefault("mod-twow-bot-gear.Enable", true);
    g_cfg.bandSize      = sConfig.GetIntDefault("mod-twow-bot-gear.BandSize", 5);
    g_cfg.minLevel      = sConfig.GetIntDefault("mod-twow-bot-gear.MinLevel", 5);
    g_cfg.minQuality    = sConfig.GetIntDefault("mod-twow-bot-gear.MinQuality", 0);
    g_cfg.maxQuality    = sConfig.GetIntDefault("mod-twow-bot-gear.MaxQuality", 0);
    g_cfg.refreshRandom = sConfig.GetBoolDefault("mod-twow-bot-gear.RefreshRandomBots", false);
    g_cfg.refreshOwned  = sConfig.GetBoolDefault("mod-twow-bot-gear.RefreshOwnedBots", false);
    g_cfg.logRefresh    = sConfig.GetBoolDefault("mod-twow-bot-gear.LogGearRefresh", true);

    if (g_cfg.bandSize == 0)
        g_cfg.bandSize = 5;
}

bool   IsEnabled()       { return g_cfg.enable; }
uint32 GetBandSize()     { return g_cfg.bandSize; }
uint32 GetMinLevel()     { return g_cfg.minLevel; }
bool   IsRefreshRandom() { return g_cfg.refreshRandom; }
bool   IsRefreshOwned()  { return g_cfg.refreshOwned; }

bool IsEligible(Player* bot, bool isManual)
{
    if (!g_cfg.enable || !bot)
        return false;

    const uint32 newLevel = bot->GetLevel();
    if (newLevel < g_cfg.minLevel)
        return false;

    if (!sScriptMgr.IsBotManaged(bot))
        return false;

    // Manual (.bgi) bypasses the automatic toggles and band check —
    // it is the explicit "force re-gear now" path.
    if (isManual)
        return true;

    const bool isRandom = TortoiseBots::BotManager::Instance().IsRandomBot(bot->GetObjectGuid());
    if (isRandom && !g_cfg.refreshRandom)
        return false;
    if (!isRandom && !g_cfg.refreshOwned)
        return false;

    // Band check: only refresh if current band > stored band
    const uint32 currentBand = (newLevel / g_cfg.bandSize) * g_cfg.bandSize;
    if (currentBand == 0)
        return false;

    const uint32 lastBand = GearState::GetLastBand(bot->GetGUIDLow());
    if (currentBand <= lastBand)
        return false;

    return true;
}

void ApplyRefresh(Player* bot)
{
    const uint32 newLevel = bot->GetLevel();
    const uint32 band = (newLevel / g_cfg.bandSize) * g_cfg.bandSize;
    const uint32 quality = RollQuality();

    PlayerbotFactory factory(bot, newLevel, quality);
    factory.UpgradeGearBest();

    GearState::SetLastBand(bot->GetGUIDLow(), band);
}

bool TryRefresh(Player* bot, const char* context, bool isManual)
{
    if (!IsEligible(bot, isManual))
        return false;

    const uint32 band = (bot->GetLevel() / g_cfg.bandSize) * g_cfg.bandSize;
    const bool isRandom = TortoiseBots::BotManager::Instance().IsRandomBot(bot->GetObjectGuid());

    ApplyRefresh(bot);

    if (g_cfg.logRefresh)
        sLog.outString("[mod-twow-bot-gear] re-geared %s bot %s at level %u (band %u, context=%s)",
                       isRandom ? "random" : "owned",
                       bot->GetName(), bot->GetLevel(), band,
                       context ? context : "?");
    return true;
}

}} // namespace ModBotGear::GearRefresh
