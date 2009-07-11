/* SUNWELL PLATEAU */
UPDATE `creature_template` SET `ScriptName` = 'boss_brutallus', `unit_flags`=0 WHERE `entry`=24882;
UPDATE `creature_template` SET `ScriptName` = 'boss_felmyst' WHERE `entry`=25038;
UPDATE `creature_template` SET `ScriptName` = 'mob_felmyst_vapor' WHERE `entry`=25265;
UPDATE `creature_template` SET `ScriptName` = 'mob_felmyst_trail' WHERE `entry`=25267;
UPDATE `creature_template` SET `ScriptName` = 'boss_sacrolash' WHERE `entry`=25165;
UPDATE `creature_template` SET `ScriptName` = 'boss_alythess' WHERE `entry`=25166;
UPDATE `creature_template` SET `ScriptName` = 'mob_shadow_image' WHERE `entry`=25214;
UPDATE `creature_template` SET `ScriptName` = 'boss_muru' WHERE `entry`=25741;
UPDATE `creature_template` SET `ScriptName` = 'boss_entropius' WHERE `entry`=25840;
UPDATE `creature_template` SET `ScriptName` = 'dark_fiend' WHERE `entry`=25744;
UPDATE `creature_template` SET `ScriptName` = 'boss_kiljaeden', `minmana` = 1693500, `maxmana` = 1693500 WHERE `entry` = '25315';
UPDATE `creature_template` SET `ScriptName` = 'mob_kiljaeden_controller' WHERE `entry` = '25608';
UPDATE `creature_template` SET `ScriptName` = 'mob_hand_of_the_deceiver' WHERE `entry` = '25588';
UPDATE `creature_template` SET `ScriptName` = 'mob_felfire_portal' WHERE `entry` = '25603';
UPDATE `creature_template` SET `ScriptName` = 'mob_volatile_felfire_fiend' WHERE `entry` = '25598';
UPDATE `creature_template` SET `ScriptName` = 'mob_shield_orb',`minlevel` = 70, `maxlevel` = 70, `minhealth` = 13986, `maxhealth` = 13986, `faction_A`=14, `faction_H`=14 WHERE entry =25502;
UPDATE `creature_template` SET `ScriptName` = 'mob_kalecgos' WHERE `entry` = '25319';
UPDATE `gameobject_template` SET `ScriptName` = 'go_orb_of_the_blue_flight', `type`=10 WHERE `entry`=188415;
UPDATE `creature_template` SET `minlevel` = 70, `maxlevel` = 70, `minhealth` = 1000, `maxhealth` = 1000, `minmana` = 10000, `maxmana` = 10000, `InhabitType`=4 WHERE entry IN (26046,25319);
UPDATE `creature_template` SET `minlevel` = 70, `maxlevel` = 70, `minhealth` = 97805, `maxhealth` = 97805, `minmana` = 10000, `maxmana` = 10000, `spell1` = 45862, `spell2` = 45860, `spell3` = 45856, `spell4` = 45848 WHERE entry =25653;
DELETE FROM `creature` WHERE map = 580 AND `id` in (25038, 26046, 25319, 25770);
INSERT INTO `creature` (`id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) VALUES 
(25038, 580, 1, 0, 0, 1459.35, 636.81, 19.9428, 4.93474, 25, 0, 0, 6100000, 4164, 0, 0),
(26046, 580, 1, 0, 0, 1698.45, 628.03, 28.1989, 0.331613, 25, 0, 0, 1, 1, 0, 0),
(25319, 580, 1, 0, 0, 1698.45, 628.03, 90.20, 0.87, 25, 10, 0, 1, 1, 0, 1);
UPDATE `gameobject_template` SET `type` = 22, `data0` = 46019 WHERE `entry`=187055;
UPDATE gameobject SET `state`='1' WHERE `map`=580 AND `id` IN (188075,187766,187764,187990,188118,187765);
DELETE FROM `gameobject` WHERE `id` in (188415);
INSERT INTO `gameobject` (`id`, `map`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
('188415','580','1','1','1703.912','581.981','28.132','1.761','0.0','0.0','0.0','1.0','180','0','1'),
('188415','580','1','1','1747.589','621.556','28.05','3.111','0.0','0.0','0.0','1.0','180','0','1'),
('188415','580','1','1','1694.322','676.565','28.05','4.808','0.0','0.0','0.0','1.0','180','0','1'),
('188415','580','1','1','1651.385','635.547','28.142','6.163','0.0','0.0','0.0','1.0','180','0','1');
DELETE FROM spell_target_position WHERE `id` IN (46019, 46020);
INSERT INTO spell_target_position () VALUES (46019, 580, 1704.34, 928.17, -74.558, 0);
INSERT INTO spell_target_position () VALUES (46020, 580, 1704.34, 928.17, 53.079, 0);