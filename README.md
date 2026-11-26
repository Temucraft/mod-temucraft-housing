# mod-temucraft-housing

Guild houses for AzerothCore 3.3.5a. 35 quiet corners of Azeroth and Outland (more can be added in the
`mod_temucraft_housing_location` table) can each belong to one guild. The guild master buys a free one at the
Housing Steward; every member can then travel there from the steward or with `.temucraft guildhouse`. The guild master can sell
the house back for half the price (configurable). A disbanded guild loses its house.

## Use

- Spawn the steward in your capitals: `.npc add 608802`.
- `.temucraft guildhouse`: travel to your guild house (not in combat, battlegrounds, arenas, instances or on flights; every 30
  minutes by default).
- The steward's "go to the guild house" line asks "Teleport to: <house>" first (Accept / Cancel), from
  `TemucraftPlayerSettings.h` (same file as mod-temucraft-player-settings); players who turned "Ask before teleports and portals" off get it without the popup.
- `.temucraft guildhouse reload` (admin): reload locations and owners after editing the tables.

## Install

1. Put this folder in `modules/`, re-run CMake and build.
2. The SQL in `data/sql/` is applied by the worldserver on its next start.
3. Copy `conf/mod-temucraft-housing.conf.dist` to your configs folder as `mod-temucraft-housing.conf`.

## What it adds

| Database | Table | IDs |
|---|---|---|
| world | `mod_temucraft_housing_location` | ids 1-999 are this module's list; add your own above 1000 |
| world | `creature_template`, `creature_template_model` | 608802 (Housing Steward) |
| world | `npc_text` | 608802 |
| world | `module_string` | module `mod-temucraft-housing`, ids 1-18 |
| characters | `mod_temucraft_housing` | one row per guild with a house |

## Translations

All player-facing text is in `module_string`; add rows to `module_string_locale` for other languages. Missing rows
fall back to the English built into the script.

## Independence

Doesn't need any other module. No core or client changes.

Part of the Temucraft modules for AzerothCore 3.3.5a.
