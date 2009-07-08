/* ULDUAR */
/* HALLS OF LIGHTNING */
UPDATE `instance_template` SET `script`='instance_halls_of_lightning' WHERE `map`=602;
UPDATE `creature_template` SET `ScriptName`='boss_bjarngrim' WHERE `entry`='28586';
UPDATE `creature_template` SET `ScriptName`='mob_stormforged_lieutenant', `minmana` = 20405, `maxmana` = 20405 WHERE `entry`='29240';
DELETE FROM `creature` WHERE map = 602 AND `id`='29240'; -- script spawns them
UPDATE `creature_template` SET `ScriptName`='boss_volkhan' WHERE `entry`='28587';
UPDATE `creature_template` SET `ScriptName`='npc_volkhans_anvil',`faction_A`=16, `faction_H`=16, `unit_flags`=33554434, `MovementType`=0, `InhabitType`=3 WHERE `entry`='28823'; -- change faction
UPDATE `creature` SET `spawndist`=0,`MovementType`=0 WHERE `id`='28823'; -- don't move
UPDATE `creature_template` SET `ScriptName`='mob_molten_golem', `minhealth` = 34000, `maxhealth` = 34000, `faction_A`=16, `faction_H`=16 WHERE `entry`='28695';
UPDATE `creature_template` SET `minhealth` = 42500, `maxhealth` = 42500, `faction_A`=16, `faction_H`=16 WHERE `entry`='30969'; -- heroic
UPDATE `creature_template` SET `ScriptName`='boss_ionar' WHERE `entry`='28546';
UPDATE `creature_template` SET `ScriptName`='npc_spark_of_ionar', `minhealth` = 12600, `maxhealth` = 12600,`faction_A`=16, `faction_H`=16, `rank`=3 WHERE `entry`='28926';
DELETE FROM `creature` WHERE map = 602 AND `id`='28947'; -- trigger, which will attack player
UPDATE `creature_template` SET `ScriptName`='boss_loken' WHERE `entry`='28923';
DELETE FROM `spell_script_target` WHERE `entry` in (52774,59160,52238,52654,52661,52387,59528);
insert into spell_script_target values (52774, 1, 28586);
insert into spell_script_target values (59160, 1, 28586);
insert into spell_script_target values (52238, 1, 28823);
insert into spell_script_target values (52654, 1, 28823);
insert into spell_script_target values (52661, 1, 28823);
insert into spell_script_target values (52387, 1, 28695);
insert into spell_script_target values (59528, 1, 28695);
UPDATE gameobject SET `state`='1' WHERE `map`=602 AND `id` IN (191416,191325,191326,191324); -- doors
UPDATE `creature_template` SET `unit_flags`=0 WHERE `entry` IN (28965, 28961); -- remove this if you don't like. Makes hallway mobs targetable before Ionar
