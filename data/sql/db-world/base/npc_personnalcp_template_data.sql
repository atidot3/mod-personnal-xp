-- DATA
SET @ENTRY := 55000;
SET @NAME := 'DeathWings';
SET @SUBNAME := 'The Destroyer';
SET @TEXT := 'Here you can select a specific xp rate to level at your own speed.';
SET @DISPLAY_ID := 9583;

DELETE FROM `creature_template` WHERE `entry` = @ENTRY;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `unit_class`, `unit_flags`, `type`, `type_flags`, `RegenHealth`, `flags_extra`, `ScriptName`) VALUES
(@ENTRY, @NAME , @SUBNAME, 'Speak', 0, 80, 80, 35, 1, 1, 1.14286, 1, 0, 1, 2, 7, 138936390, 1, 2, 'creature_personnalxp_mode');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @ENTRY;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@ENTRY, 0, @DISPLAY_ID, 1, 1, 0);

DELETE FROM `npc_text` WHERE `ID` = @ENTRY;
INSERT INTO `npc_text` (`ID`, `text0_0`, `text0_1`) VALUES
(@ENTRY, @TEXT, @TEXT);

DELETE FROM `creature_template_movement` WHERE `CreatureId` = @ENTRY;
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`, `Chase`, `Random`, `InteractionPauseTimer`) VALUES
(@ENTRY, 1, 1, 0, 0, 0, 0, NULL);

