-- mod-temucraft-housing: the places a guild can own, the Housing Steward and the module's texts
CREATE TABLE IF NOT EXISTS `mod_temucraft_housing_location` (
  `id` INT UNSIGNED NOT NULL,
  `map` SMALLINT UNSIGNED NOT NULL,
  `x` FLOAT NOT NULL,
  `y` FLOAT NOT NULL,
  `z` FLOAT NOT NULL,
  `o` FLOAT NOT NULL DEFAULT 0,
  `name` VARCHAR(100) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

DELETE FROM `mod_temucraft_housing_location` WHERE `id` BETWEEN 1 AND 999;
INSERT INTO `mod_temucraft_housing_location` (`id`, `map`, `x`, `y`, `z`, `o`, `name`) VALUES
(2, 1, -10711, 2483, 8, 0, 'Tauren village at the Veiled Sea (Silithus)'),
(3, 0, -8323, -343, 146, 0, 'Fishing spot above Northshire Abbey (Elwynn Forest)'),
(4, 1, 7368, -1560, 163, 0, 'Troll village in the mountains (Darkshore)'),
(5, 0, -4151, -1400, 198, 0, 'Dwarven village outside Ironforge (Wetlands)'),
(6, 0, -1840, -4233, 2.14, 0, 'Dwarven village by the Forbidding Sea (Arathi Highlands)'),
(8, 1, -723, -1076, 179, 0, 'Tauren camp at Red Rock (Mulgore)'),
(9, 0, -206, 1666, 80, 0, 'Outside Shadowfang Keep (Silverpine Forest)'),
(10, 0, -6374, 1262, 7, 0, 'Harbor house outside Stormwind (Elwynn Forest)'),
(12, 0, -4844, -1066, 502, 0, 'Old Ironforge'),
(13, 0, -4863, -1658, 503.5, 0, 'Ironforge Airport'),
(14, 37, 1146, -165, 313, 0, 'Azshara Crater (Alliance entrance)'),
(15, 37, -123, 858, 298, 0, 'Azshara Crater (Horde entrance)'),
(16, 0, 4303, -2760, 16.8, 0, 'Quel''Thalas Tower'),
(17, 0, -6161, -790, 423, 0, 'Crashed gnome airplane (Dun Morogh / Searing Gorge)'),
(18, 0, -11790, -1640, 54.7, 0, 'Outside Zul''Gurub (Stranglethorn Vale)'),
(19, 1, -11805, -4754, 6, 0, 'Goblin village by the South Seas (Tanaris)'),
(20, 0, -9296, 670, 132, 0, 'Bandit camp outside Stormwind (Elwynn Forest)'),
(21, 0, 3414, -3380, 142.2, 0, 'Outside Stratholme'),
(22, 1, 4654, -3772, 944, 0, 'The World Tree (Hyjal)'),
(23, 1, 2176, -4766, 55, 0, 'The Ring of Valor (Orgrimmar)'),
(24, 1, 1951.512, 1530.476, 247.288, 0, 'Stonetalon Logging Camp'),
(25, 1, 2813.661, 2248.553, 215.525, 0, 'Stonetalon Ruins'),
(28, 1, 9725.27, -21.43, 20.03, 0, 'Furbolg camp (Teldrassil)'),
(29, 0, -3855, -3479, 579, 0, 'Mountain camp (Wetlands)'),
(30, 0, -5362, -2540, 485, 0, 'Ortell''s Hideout'),
(31, 0, -12865, -1396, 115, 0, 'Secret cave (Stranglethorn Vale)'),
(32, 0, -11073, -1956, 39, 0, 'Karazhan hillside'),
(33, 0, -11084, -1801, 53, 0, 'Well of the Forgotten (Karazhan)'),
(34, 0, 1683.235, 286.459, -45.452, 0, 'Undercity top tier'),
(35, 0, -8521.3, 599.5, 101.399, 0, 'Cut-Throat Alley (Stormwind)'),
(36, 0, -5933, 452, 509, 0, 'Forgotten gnome camp'),
(37, 530, -920.231, 7096.489, 170.353, 0, 'Newton''s Remains (Nagrand)'),
(38, 530, -2140.502, 9142.688, 137.042, 0, 'Ancient tomb (Nagrand)'),
(39, 530, -483.402, 7461.945, 186.121, 0, 'Challe''s Home for Little Tykes (Nagrand)'),
(40, 530, 2387.754, 3191.757, 152.669, 0, 'Nova''s Shrine (Netherstorm)');

DELETE FROM `creature_template` WHERE `entry` = 608802;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`,
  `speed_walk`, `speed_run`, `rank`, `unit_class`, `unit_flags`, `type`, `type_flags`, `AIName`, `MovementType`, `HealthModifier`, `RegenHealth`,
  `flags_extra`, `ScriptName`) VALUES
(608802, 'Steward Aldric', 'Guild Houses', '', 0, 80, 80, 2, 35, 1, 1, 1.14286, 0, 1, 768, 7, 0, '', 0, 1, 1, 2, '');
DELETE FROM `creature_template_model` WHERE `CreatureID` = 608802;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`)
SELECT 608802, 0, `CreatureDisplayID`, 1, 1, 0 FROM `creature_template_model` WHERE `CreatureID` = 28701 AND `Idx` = 0;

DELETE FROM `npc_text` WHERE `ID` = 608802;
INSERT INTO `npc_text` (`ID`, `text0_0`, `text0_1`, `Probability0`) VALUES
(608802, 'A guild needs a hall to call its own. I keep the deeds to every quiet corner of Azeroth worth living in.', '', 1);

DELETE FROM `module_string` WHERE `module` = 'mod-temucraft-housing';
INSERT INTO `module_string` (`module`, `id`, `string`) VALUES
('mod-temucraft-housing', 1, 'Take me to our guild house ({}).'),
('mod-temucraft-housing', 2, 'Show me the houses for sale.'),
('mod-temucraft-housing', 3, 'Sell our guild house.'),
('mod-temucraft-housing', 4, 'You need to be in a guild.'),
('mod-temucraft-housing', 5, 'Only the guild master can buy or sell the guild house.'),
('mod-temucraft-housing', 6, 'Your guild already owns a house.'),
('mod-temucraft-housing', 7, 'Another guild bought that house first.'),
('mod-temucraft-housing', 8, 'Your guild now owns: {}.'),
('mod-temucraft-housing', 9, 'The guild house is sold; {} returned.'),
('mod-temucraft-housing', 10, 'A guild house costs {}.'),
('mod-temucraft-housing', 11, 'Your guild has no house.'),
('mod-temucraft-housing', 12, 'You can travel to the guild house again in {} minutes.'),
('mod-temucraft-housing', 13, 'You can''t travel to the guild house from here or right now.'),
('mod-temucraft-housing', 14, 'More houses...'),
('mod-temucraft-housing', 15, 'Buy {} for your guild?'),
('mod-temucraft-housing', 16, 'Sell your guild house for {}?'),
('mod-temucraft-housing', 17, 'Guild houses are turned off.'),
('mod-temucraft-housing', 18, 'Guild house: {}');
-- Translations go in `module_string_locale` (`module`, `id`, `locale`, `string`).
