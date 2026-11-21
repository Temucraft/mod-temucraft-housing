-- mod-temucraft-housing: its player commands (all under .temucraft) as RBAC permissions 100850-100899. Linked to the stock player
-- role 195 and the Temucraft player role 1010, so every account has them by default. To take one away, remove its
-- link here (or deny it per account with .rbac account deny <account> <id>); to give it to staff only, link it to
-- 1011-1014 instead.
DELETE FROM `rbac_linked_permissions` WHERE `linkedId` BETWEEN 100850 AND 100899;
DELETE FROM `rbac_permissions` WHERE `id` BETWEEN 100850 AND 100899;
INSERT INTO `rbac_permissions` (`id`, `name`) VALUES
(100850, 'Command: temucraft guildhouse');
INSERT INTO `rbac_linked_permissions` (`id`, `linkedId`) VALUES
(195, 100850),
(1010, 100850);
