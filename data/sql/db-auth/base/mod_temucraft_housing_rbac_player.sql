-- mod-temucraft-housing: its commands (all under .temucraft) as RBAC permissions 100850-100899. Every command is admin only by
-- default: linked to the administrator roles 192 and 1014. To give one to players, link it to 195 and 1010 here (or
-- grant it per account with .rbac account grant <account> <id>). "Temucraft: use ..." permissions are not
-- commands: they stay linked to the player roles so the items keep working.
DELETE FROM `rbac_linked_permissions` WHERE `linkedId` BETWEEN 100850 AND 100899;
DELETE FROM `rbac_permissions` WHERE `id` BETWEEN 100850 AND 100899;
INSERT INTO `rbac_permissions` (`id`, `name`) VALUES
(100850, 'Command: temucraft guildhouse');
INSERT INTO `rbac_linked_permissions` (`id`, `linkedId`) VALUES
(192, 100850),
(1014, 100850);
