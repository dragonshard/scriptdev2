/* UTGARDE PINNACLE */
UPDATE `instance_template` SET `script`='instance_pinnacle' WHERE `map`=575;
UPDATE `creature_template` SET `ScriptName`='boss_palehoof' WHERE `entry`='26687';
UPDATE `creature_template` SET `ScriptName`='boss_skadi', `unit_flags`=0 WHERE `entry`='26693';
DELETE FROM `creature` WHERE map = 575 AND `id`='26893';
UPDATE `creature_template` SET `ScriptName`='boss_svala' WHERE `entry`='29281';
UPDATE `creature_template` SET `ScriptName`='boss_ymiron' WHERE `entry`='26861';
UPDATE `creature_template` SET `ScriptName`='mob_massive_jormungar' WHERE `entry`='26685';
UPDATE `creature_template` SET `ScriptName`='mob_ferocious_rhino' WHERE `entry`='26686';
UPDATE `creature_template` SET `ScriptName`='mob_ravenous_furbolg' WHERE `entry`='26684';
UPDATE `creature_template` SET `ScriptName`='mob_frenzied_worgen' WHERE `entry`='26683';
UPDATE `creature_template` SET `ScriptName`='boss_svala_sorrowgrave',`minlevel` = 80, `maxlevel` = 81, `minhealth` = 315000, `maxhealth` = 315000, `faction_A`=21, `faction_H`=21 WHERE `entry`='26668';
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 81, `minhealth` = 431392, `maxhealth` = 431392, `faction_A`=21, `faction_H`=21 WHERE `entry`='30810'; -- heroic
UPDATE `creature_template` SET `ScriptName`='mob_ritual_channeler',`minlevel` = 80, `maxlevel` = 81, `minhealth` = 12600, `maxhealth` = 12600, `minmana` = 15976, `maxmana` = 15976, `faction_A`=21, `faction_H`=21 WHERE `entry`='27281';
UPDATE `creature_template` SET `unit_flags`=2 WHERE `entry`='27327';
UPDATE `creature` SET `spawndist`=0,`MovementType`=0 WHERE `id`='27327';
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 81, `minhealth` = 3938, `maxhealth` = 4073, `faction_A`=974, `faction_H`=974 WHERE `entry`='27228';
UPDATE `creature_template` SET `unit_flags`=33554434 WHERE `entry` IN (26687,26683,26684,26685,26686);
UPDATE `creature_template` SET AIName='EventAI',`minlevel` = 79, `maxlevel` = 79, `minhealth` = 25200, `maxhealth` = 25200, `faction_A`=21, `faction_H`=21 WHERE `entry`='26690';
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81, `minhealth` = 26066, `maxhealth` = 26066, `faction_A`=21, `faction_H`=21 WHERE `entry`='30822'; -- heroic
UPDATE `creature_template` SET AIName='EventAI',`minlevel` = 79, `maxlevel` = 79, `minhealth` = 18262, `maxhealth` = 18262, `minmana` = 3893, `maxmana` = 3893, `faction_A`=21, `faction_H`=21 WHERE `entry`='26691';
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81, `minhealth` = 26066, `maxhealth` = 26066, `minmana` = 4081, `maxmana` = 4081, `faction_A`=21, `faction_H`=21 WHERE `entry`='30823'; -- heroic
UPDATE `creature_template` SET AIName='EventAI',`minlevel` = 79, `maxlevel` = 79, `minhealth` = 18262, `maxhealth` = 18262, `faction_A`=21, `faction_H`=21 WHERE `entry`='26692';
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81, `minhealth` = 26066, `maxhealth` = 26066, `faction_A`=21, `faction_H`=21 WHERE `entry`='30819'; -- heroic
DELETE FROM creature_ai_scripts WHERE creature_id IN (26690,26691,26692);
INSERT INTO `creature_ai_scripts` VALUES
('266901', '26690', '0', '0', '100', '1', '5000', '10000', '10000', '20000', '11', '48639', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Hamstring'),
('266902', '26690', '0', '0', '100', '1', '6000', '12000', '10000', '20000', '11', '48640', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Strike'),
('266911', '26691', '0', '0', '100', '3', '5000', '10000', '5000', '10000', '11', '49084', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Shadow Bolt'),
('266912', '26691', '0', '0', '100', '3', '10000', '20000', '10000', '20000', '11', '59246', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Shrink'),
('266913', '26691', '0', '0', '100', '5', '5000', '10000', '5000', '10000', '11', '49084', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Shadow Bolt'),
('266914', '26691', '0', '0', '100', '5', '10000', '20000', '10000', '20000', '11', '59247', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Shrink'),
('266921', '26692', '0', '0', '100', '1', '10000', '20000', '10000', '20000', '11', '49092', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Net'),
('266922', '26692', '0', '0', '100', '1', '5000', '10000', '5000', '10000', '11', '49091', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Throw');
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80, `minhealth` = 3000, `maxhealth` = 3000, `faction_A`=21, `faction_H`=21 WHERE `entry`='27386';
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80, `minhealth` = 12000, `maxhealth` = 12000, `faction_A`=21, `faction_H`=21 WHERE `entry`='30756'; -- heroic
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80, `minhealth` = 1000, `maxhealth` = 1000, `minmana` = 5000, `maxmana` = 5000, `faction_A`=21, `faction_H`=21 WHERE `entry` IN (27339,30808);
