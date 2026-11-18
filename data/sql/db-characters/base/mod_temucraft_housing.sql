-- mod-temucraft-housing: which guild owns which house
CREATE TABLE IF NOT EXISTS `mod_temucraft_housing` (
  `guild_id` INT UNSIGNED NOT NULL,
  `location_id` INT UNSIGNED NOT NULL COMMENT 'mod_temucraft_housing_location.id (world)',
  PRIMARY KEY (`guild_id`),
  UNIQUE KEY `location` (`location_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
