#include "GearState.h"

#include "Database/DatabaseEnv.h"
#include "Policies/Singleton.h"

namespace ModBotGear { namespace GearState
{

uint32 GetLastBand(uint32 characterGuid)
{
    std::unique_ptr<QueryResult> result(
        CharacterDatabase.PQuery("SELECT last_band FROM mod_twow_bot_gear_state WHERE character_guid = '%u'",
                                 characterGuid));
    if (!result)
        return 0;
    return result->Fetch()[0].GetUInt32();
}

void SetLastBand(uint32 characterGuid, uint32 band)
{
    CharacterDatabase.PExecute(
        "INSERT INTO mod_twow_bot_gear_state (character_guid, last_band) VALUES ('%u', '%u') "
        "ON DUPLICATE KEY UPDATE last_band = VALUES(last_band)",
        characterGuid, band);
}

}} // namespace ModBotGear::GearState
