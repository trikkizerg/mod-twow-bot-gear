-- mod-twow-bot-gear: per-bot state
--
-- Tracks the highest level band a bot has been re-geared for, so we don't
-- redundantly re-gear on every login/level-change within the same band.
--
--   last_band  = (level / BandSize) * BandSize at the time of last refresh.
--                A refresh only happens when a bot's current band exceeds this.
--   refreshed_at = for diagnostics; not used in logic.

CREATE TABLE IF NOT EXISTS `mod_twow_bot_gear_state` (
  `character_guid`  INT(10) UNSIGNED NOT NULL,
  `last_band`       INT(10) UNSIGNED NOT NULL DEFAULT 0,
  `refreshed_at`    TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`character_guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;
