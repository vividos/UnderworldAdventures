--
-- Underworld Adventures - an Ultima Underworld remake project
-- Copyright (c) 2002,2003,2022 Underworld Adventures Team
--
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software
-- Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
--

--
-- createchar.lua - lua cutscene script; controls the character creation screen
-- by Dirk Manders
--

--
-- registered C functions:
--
-- cuts_do_action(actioncode, actionparam_1...actionparam_n)
--   actioncode is one of the values below; number and type of actionparams
--   depend on the action.
--

-- constants

-- actions (outgoing param values for C function cchar_do_action)
actionEnd = 0                 -- ends the character creation screen (no params)
actionSetInitValue = 1        -- sets init values (param1=stringblock, param2=buttongroup x-coord, param3=normal text color, param4=highlighted text color, param5=table with button image indexes)
actionSetUIButtonGroup = 2    -- sets the specified button group (param1=heading, param2=buttontype, param3=buttontable)
actionSetUIText = 3           -- sets the specified text at the specified location (param1=stringno, param2=x-coord, param3=y-coord, param4=alignment, [param5=stringblock])
actionSetUICustomText = 4     -- sets the specified custom text at the specified location (param1=text, param2=x-coord, param3=y-coord, param4=alignment)
actionSetUINumber = 5         -- sets the specifed number at the specified location (param1=number, param2=right x-coord, param3=y-coord)
actionSetUIImage = 6          -- sets the specified image at the specified location (param1=imagenumber, param2=center x-coord, param3=center y-coord)
actionUIClear = 7             -- clears all screen elements (but not the background)
actionUIUpdate = 8            -- updates the screen after a SetUIxxx/UIClear action (no params)
actionSetPlayerName = 9       -- sets the player name (param1=name)
actionSetPlayerAttribute = 10 -- sets a player attribute (param1=attribute, param2=value)
actionSetPlayerSkill = 11     -- sets a player skill (param1=skill, param2=value)

-- labels/button values, these must match entries in string table
-- starting at block charGenData.stringBlock
ccvNone = 0
ccvSex = 1
ccvHandedness = 2
ccvClass = 3
ccvSkill = 4
ccvDifficulty = 6
ccvName = 7
ccvKeep = 8
ccvMale = 9
ccvFemale = 10
ccvLeft = 11
ccvRight = 12
ccvStandard = 13
ccvEasy = 14
ccvYes = 15
ccvNo = 16
ccvStrc = 17
ccvDexc = 18
ccvIntc = 19
ccvVitc = 20
ccvManac = 21
ccvExpc = 22
ccvFighter = 23
ccvMage = 24
ccvBard = 25
ccvTinker = 26
ccvDruid = 27
ccvPaladin = 28
ccvRanger = 29
ccvShepard = 30
ccvAttack = 31
ccvDefence = 32
ccvUnarmed = 33
ccvSword = 34
ccvAxe = 35
ccvMace = 36
ccvMissile = 37
ccvMana = 38
ccvLore = 39
ccvCasting = 40
ccvTraps = 41
ccvSearch = 42
ccvTrack = 43
ccvSneak = 44
ccvRepair = 45
ccvCharm = 46
ccvPicklock = 47
ccvAcrobat = 48
ccvAppraise = 49
ccvSwimming = 50

-- button types
buttonText = 0       -- standard button with text from string table, buttons contains stringtable index
buttonImage = 1      -- square button with image, buttons contains index of image
buttonInput = 2      -- input area, buttons contains the stringtable index of the label of the input area
buttonTimer = 3      -- countdown timer, heading contains time in msec before virtual button 0 is pressed, values in buttons are ignored

-- text alignment types
alignLeft = 0
alignCenter = 1
alignRight = 2

-- end action types
endActionCancel = 0     -- end screen and go back to start menu
endActionNewGame = 1    -- end screen and go the in-game view with a new game


-- tables

-- button groups

charGenData = {
   stringBlock = 2,                 -- string block for labels/buttons
   buttonGroupXCoord = 240,         -- x-coord for center of buttongroup
   textcolorNormal = 73,            -- palette index of normal text (labels and normal buttons)
   textcolorHighlight = 68,         -- palette index of highlighted button text
   buttonImages = { 0, 2, 3, 5, 6 }, -- image indexes of buttons (textn,texth,imgn,imgh,inp)

   buttonGroups = -- table with all buttons groups (a.k.a. "screens")
   {
     -- general buttons
      [1] = { heading = ccvSex,  buttonType = buttonText,  buttons = { ccvMale, ccvFemale } },
      [2] = { heading = ccvHandedness, buttonType = buttonText, buttons = { ccvLeft, ccvRight } },
      [3] = { heading = ccvClass, buttonType = buttonText, buttons = { ccvFighter, ccvMage, ccvBard, ccvTinker, ccvDruid, ccvPaladin, ccvRanger, ccvShepard } },

     -- fighter skills
      [4] = { heading = ccvSkill, buttonType = btnText, buttons = { ccvAttack, ccvDefence } },
      [5] = { heading = ccvSkill, buttonType = btnText, buttons = { ccvUnarmed, ccvSword, ccvAxe, ccvMace, ccvMissile } },
      [6] = { heading = ccvSkill, buttonType = btnText, buttons = { ccvSwimming, ccvTraps, ccvSearch, ccvCharm, ccvAcrobat, ccvAppraise } },

      -- mage skills
      [7] = { heading = ccvSkill, buttonType = btnText, buttons = { ccvMana, ccvCasting } },

      -- bard skills
      [8]  = { heading = ccvSkill, buttonType = btnText, buttons = { ccvLore, ccvCharm } },
      [9]  = { heading = ccvSkill, buttonType = btnText, buttons = { ccvAppraise, ccvAcrobat, ccvSneak, ccvPicklock, ccvSearch, ccvSwimming } },
      [10] = { heading = ccvSkill, buttonType = btnText, buttons = { ccvMana, ccvCasting, ccvSword, ccvAxe, ccvMace, ccvUnarmed,  ccvMissile } },

      -- tinker skills
      -- first tinker skills page same as fighter skills page 2 (index 5)
      [11] = { heading = ccvSkill, buttonType = btnText, buttons = { ccvPicklock, ccvTraps, ccvSearch, ccvAppraise, ccvRepair } },

     -- druid skills
      [12] = { heading = ccvSkill, buttonType = btnText, buttons = { ccvTrack, ccvLore, ccvSearch } },

     -- paladin skills
      [13] = { heading = ccvSkill, buttonType = btnText, buttons = { ccvAppraise, ccvCharm, ccvAcrobat, ccvRepair } },
      -- second paladin skill page same as fighter skills page 2 (index 5)

     -- ranger skills
      [14] = { heading = ccvSkill, buttonType = btnText, buttons = { ccvTraps, ccvAcrobat, ccvSneak, ccvSearch, ccvSwimming, ccvRepair } },
      [15] = { heading = ccvSkill, buttonType = btnText, buttons = { ccvUnarmed, ccvSword, ccvAxe, ccvMace, ccvMissile, ccvAttack, ccvDefence, ccvTrack } },

     -- shepard skills  (note: the original game had the last 2 pages combined on one page with 10 buttons)
      [16] = { heading = ccvSkill, buttonType = btnText, buttons = { ccvUnarmed, ccvSword, ccvAxe, ccvMace, ccvMissile, ccvDefence } },
      [17] = { heading = ccvSkill, buttonType = btnText, buttons = { ccvTraps, ccvSneak, ccvAppraise, ccvTrack, ccvLore, ccvSearch, ccvAcrobat, ccvSwimming, ccvCasting, ccvMana } },

     -- appearance
      [18] = { heading = ccvNone, buttonType = buttonImage, buttons = {  7,  8,  9, 10, 11 } },
      [19] = { heading = ccvNone, buttonType = buttonImage, buttons = { 12, 13, 14, 15, 16 } },

     -- general buttons
      [20] = { heading = ccvDifficulty, buttonType = buttonText, buttons = { ccvStandard, ccvEasy } },
      [21] = { heading = ccvNone, buttonType = buttonInput, buttons = { ccvName }},
      [22] = { heading = ccvKeep, buttonType = buttonText, buttons = { ccvYes, ccvNo } },

      [23] = { heading = 1000, buttonType = buttonTimer, buttons = { } }
   },

   skillOrder = -- table with the skill button group order for every player class
   {
      [0] = { [0] =  4, [1] =  5, [2] =  6, [3] = 0 }, -- fighter skills
      [1] = { [0] =  7, [1] =  0, [2] =  0, [3] = 0 }, -- mage skills
      [2] = { [0] =  8, [1] =  9, [2] = 10, [3] = 0 }, -- bard skills
      [3] = { [0] =  5, [1] = 11, [2] =  0, [3] = 0 }, -- tinker skills
      [4] = { [0] = 12, [1] =  0, [2] =  0, [3] = 0 }, -- druid skills
      [5] = { [0] = 13, [1] =  5, [2] =  0, [3] = 0 }, -- paladin skills
      [6] = { [0] = 14, [1] = 15, [2] =  0, [3] = 0 }, -- ranger skills
      [7] = { [0] = 16, [1] = 17, [2] = 17, [3] = 0 }  -- shepard skills
   },

   stringToSkillMap = -- translates ccvXXX to player_skill_XXX values (defined in uwinterface.lua)
   {
      [ccvAttack]   = player_skill_attack,
      [ccvDefence]  = player_skill_defense,
      [ccvUnarmed]  = player_skill_unarmed,
      [ccvSword]    = player_skill_sword,
      [ccvAxe]      = player_skill_axe,
      [ccvMace]     = player_skill_mace,
      [ccvMissile]  = player_skill_missile,
      [ccvMana]     = player_skill_mana,
      [ccvLore]     = player_skill_lore,
      [ccvCasting]  = player_skill_casting,
      [ccvTraps]    = player_skill_traps,
      [ccvSearch]   = player_skill_search,
      [ccvTrack]    = player_skill_track,
      [ccvSneak]    = player_skill_sneak,
      [ccvRepair]   = player_skill_repair,
      [ccvCharm]    = player_skill_charm,
      [ccvPicklock] = player_skill_picklock,
      [ccvAcrobat]  = player_skill_acrobat,
      [ccvAppraise] = player_skill_appraise,
      [ccvSwimming] = player_skill_swimming
   }
}


-- functions

-- starts character creation
function cchar_start(seed)

   skills = {}
   math.randomseed(seed)

   cchar_do_action(actionSetInitValue,
      charGenData.stringBlock,
      charGenData.buttonGroupXCoord,
      charGenData.textcolorNormal,
      charGenData.textcolorHighlight,
      charGenData.buttonImages)

   finished = 0

   cchar_show_first_button_group();
end


-- show the first button group
function cchar_show_first_button_group()
   currentStep = 0
   playerSex = 0
   playerClass = 0
   playerStrength = 0
   playerDexterity = 0
   playerIntelligence = 0
   playerVitality = 0
   playerImage = 0
   playerName = ""
   currentGroup = 1
   numberOfSkills = 0

   cchar_do_action(actionUIClear)
   cchar_do_action(actionSetUIButtonGroup,
      charGenData.buttonGroups[currentGroup].heading,
      charGenData.buttonGroups[currentGroup].buttonType,
      charGenData.buttonGroups[currentGroup].buttons)
   cchar_do_action(actionUIUpdate)
end


-- cancels character creation, and either restarts the process or ends the
-- screen
function cchar_cancel()

   -- when on the first page, end char creation
   if currentGroup == 1 then
      cchar_do_action(actionEnd)
   else
      cchar_show_first_button_group();
   end
end


-- adds a value to, or changes a value in, the skills table
function cchar_addskill(skill, value)
   -- try to find the skill in the table
   for skillIndex = 1, numberOfSkills do
      -- increase the value if found and return
      if skills[skillIndex].name == skill then
         skills[skillIndex].val = skills[skillIndex].val + value/2
         cchar_do_action(actionSetPlayerSkill,
            charGenData.stringToSkillMap[skill],
            skills[skillIndex].val)
         return
      end
   end

   -- not found? add the skill
   numberOfSkills = numberOfSkills + 1
   skills[numberOfSkills] = { name = skill, val = value };
   cchar_do_action(actionSetPlayerSkill,
      charGenData.stringToSkillMap[skill],
      value)
end


-- processes action on a button click in the current group
function cchar_buttonclick(button, text)

   if finished == 1 and currentGroup < 22
   then
      return
   end

   -- simple case, button groups in sequential order
   if currentGroup <= 2 or currentGroup >= 18 then

      if currentGroup == 1 then
        cchar_do_action(actionSetPlayerAttribute, player_attr_gender, button)
        playerSex = button

      elseif currentGroup == 2 then
        cchar_do_action(actionSetPlayerAttribute, player_attr_handedness, button)

      elseif currentGroup == 18 then
         cchar_do_action(actionSetPlayerAttribute, player_attr_appearance, button)
         playerImage = button
         currentGroup = currentGroup + 1

      elseif currentGroup == 19 then
         cchar_do_action(actionSetPlayerAttribute, player_attr_appearance, button)
         playerImage = button + 5

      elseif currentGroup == 20 then
         cchar_do_action(actionSetPlayerAttribute, player_attr_difficulty, button)

      elseif currentGroup == 21 then
         if string.len(text) < 1 then
            return  -- don't accept an empty name
         else
            playerName = text
            cchar_do_action(actionSetPlayerName, playerName)
         end

      elseif currentGroup == 22 then
         if button == 1 then
            cchar_cancel()
            return
         end

      elseif currentGroup==23 then
         currentGroup = -1
         cchar_do_action(actionEnd, endActionNewGame)
         return
      end
      currentGroup = currentGroup + 1

   -- special case, player skills button groups (depend on player class)
   elseif currentGroup >= 3 and currentGroup <= 17 then

      if currentGroup==3 then   -- player class selection button
         cchar_do_action(actionSetPlayerAttribute, player_attr_profession, button)
         playerClass = button
         currentStep = 0

         playerStrength = math.random(20, 30)
         playerDexterity = math.random(15, 25)
         playerIntelligence = math.random(12, 22)
         playerVitality = math.random(33, 36)

         cchar_do_action(actionSetPlayerAttribute, player_attr_strength, playerStrength)
         cchar_do_action(actionSetPlayerAttribute, player_attr_dexterity, playerDexterity)
         cchar_do_action(actionSetPlayerAttribute, player_attr_intelligence, playerIntelligence)
         cchar_do_action(actionSetPlayerAttribute, player_attr_max_life, playerVitality)
         cchar_do_action(actionSetPlayerAttribute, player_attr_life, playerVitality)

         -- set mana/max_mana attribute (note: this is not the same as mana skill)
         local max_mana = 0
         if button == 0 or button == 3 or button == 6 or button == 7 then   -- fighter/tinker/ranger/shepard
            max_mana = 2
         elseif button == 1 then  -- mage
            max_mana = 35
         elseif button == 2 then  -- bard
            max_mana = 15
         elseif button == 4 then  -- druid
            max_mana = 27
         elseif button == 5 then  -- paladin
            max_mana = 3
         end

         cchar_do_action(actionSetPlayerAttribute, player_attr_max_mana, max_mana)
         cchar_do_action(actionSetPlayerAttribute, player_attr_mana, max_mana)

         -- the attack and defence skill appear for all player classes
         cchar_addskill(ccvAttack, math.random(4, 13))
         cchar_addskill(ccvDefence, math.random(4, 13))

         -- add class specific inital skills
         if button == 1 or button == 4 then   -- mage/druid
            cchar_addskill(ccvMana, math.random(4, 13))
            cchar_addskill(ccvCasting, math.random(4, 13))
         elseif button == 3 then            -- tinker
            cchar_addskill(ccvRepair, math.random(4, 13))
         elseif button == 5 then            -- paladin
            cchar_addskill(ccvCharm, math.random(4, 13))
         elseif button == 6 then            -- ranger
            cchar_addskill(ccvTrack, math.random(4, 13))
         end

      else   -- class specific skill selection button
         cchar_addskill(charGenData.buttonGroups[currentGroup].buttons[button+1], math.random(4, 13))
         currentStep = currentStep + 1
      end
      currentGroup = charGenData.skillOrder[playerClass][currentStep]

      -- determine which set of appearance buttons to show
      if currentGroup == 0 then
         if playerSex == 0 then
            currentGroup = 18
         else
            currentGroup = 19
         end
      end
   end

   -- show the new button group and stats (or end)
   if currentGroup > 0 then
      cchar_do_action(actionUIClear)

      if currentGroup > 1 then
         cchar_do_action(actionSetUIText, playerSex + ccvMale, 18, 21, alignLeft)
      end
      if currentGroup > 3 then
         -- common stats
         cchar_do_action(actionSetUIText, playerClass+ccvFighter, 141, 21, alignRight)
         cchar_do_action(actionSetUIText, ccvStrc, 93, 50, alignLeft)
         cchar_do_action(actionSetUINumber, playerStrength, 139, 50)
         cchar_do_action(actionSetUIText, ccvDexc, 93, 67, alignLeft)
         cchar_do_action(actionSetUINumber, playerDexterity, 139, 67)
         cchar_do_action(actionSetUIText, ccvIntc, 93, 84, alignLeft)
         cchar_do_action(actionSetUINumber, playerIntelligence, 139, 84)
         cchar_do_action(actionSetUIText, ccvVitc, 93, 101, alignLeft)
         cchar_do_action(actionSetUINumber, playerVitality, 139, 101)
      end

      -- class/skill specific stats
      for skillIndex = 1, numberOfSkills do
         cchar_do_action(actionSetUIText, skills[skillIndex].name, 30, 132 + 11 * (skillIndex - 1), alignLeft)
         cchar_do_action(actionSetUINumber, skills[skillIndex].val, 125, 132 + 11 * (skillIndex - 1))
      end

      if currentGroup > 19 then
         cchar_do_action(actionSetUIImage, 17 + playerImage, 44, 81)
      end

      if currentGroup > 21 then
         cchar_do_action(actionSetUICustomText, playerName, 80, 10, alignCenter)
      end

      if currentGroup > 22 then
         -- show "playerName enters the abyss..."
         cchar_do_action(actionSetUICustomText, playerName, 240, 90, alignCenter)
         -- we need one string (#256) from stringblock 1
         cchar_do_action(actionSetUIText, 256, 240, 100, alignCenter, 1)
         -- we're finished (don't allow any more input)
         finished = 1
      end

      cchar_do_action(actionSetUIButtonGroup,
         charGenData.buttonGroups[currentGroup].heading,
         charGenData.buttonGroups[currentGroup].buttonType,
         charGenData.buttonGroups[currentGroup].buttons)
      cchar_do_action(actionUIUpdate)
   else
      cchar_do_action(actionEnd, endActionCancel)
   end
end
