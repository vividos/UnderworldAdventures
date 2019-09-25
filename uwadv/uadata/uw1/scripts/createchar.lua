--
-- Underworld Adventures - an Ultima Underworld remake project
-- Copyright (c) 2002,2003 Underworld Adventures Team
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

-- global actions (incoming param values for Lua function cchar_global)
gactInit = 0    -- initialize
gactDeinit = 1  -- deinitialize

-- actions (outgoing param values for C function cchar_do_action)
actEnd = 0             -- ends the character creation screen (no params)
actSetInitVal = 1      -- sets init values (param1=stringblock, param2=buttongroup x-coord, param3=normal text color, param4=highlighted text color, param5=table with button image indexes)
actSetUIBtnGroup = 2   -- sets the specified button group (param1=heading, param2=buttontype, param3=buttontable)
actSetUIText = 3       -- sets the specified text at the specified location (param1=stringno, param2=x-coord, param3=y-coord, param4=alignment, [param5=stringblock])
actSetUICustText = 4   -- sets the specified custom text at the specified location (param1=text, param2=x-coord, param3=y-coord, param4=alignment)
actSetUINumber = 5     -- sets the specifed number at the specified location (param1=number, param2=right x-coord, param3=y-coord)
actSetUIImg = 6        -- sets the specified image at the specified location (param1=imagenumber, param2=center x-coord, param3=center y-coord)
actUIClear = 7         -- clears all screen elements (not the background)
actUIUpdate = 8        -- updates the screen after a SetUIxxx/UIClear action (no params)
actSetPlayerName = 9   -- does what the name suggests (param1=name)
actSetPlayerAttr = 10  -- does what the name suggests (param1=attribute, param2=value)
actSetPlayerSkill = 11 -- does what the name suggests (param1=skill, param2=value)

-- labels/button values, these must match entries in string table
-- starting at block cchar_strblock.
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
buttonText = 0       -- standard button with text from string table, btns contains stringtable index
buttonImage = 1      -- square button with image, btns contains index of image
buttonInput = 2      -- input area, btns contains the stringtable index of the label of the input area
btTimer = 3      -- countdown timer, heading contains time in msec before virtual button 0 is pressed, values in btns are ignored

-- text alignment types
alLeft = 0
alCenter = 1
alRight = 2

-- end action types
eaCancel = 0    -- end screen and go back to start menu
eaNewGame = 1   -- end screen and go the in-game view with a new game


-- tables

-- button groups

ccharui = {
   strblock = 2,                    -- string block for labels/buttons
   btngxcoord = 240,                -- x-coord for center of buttongroup
   textcolor_normal = 73,           -- palette index of normal text (labels and normal buttons)
   textcolor_highlight = 68,        -- palette index of highlighted button text
   btnimages = { 0, 2, 3, 5, 6 },   -- image indexes of buttons (textn,texth,imgn,imgh,inp)

   btngroups = -- table with all buttons groups (a.k.a. "screens")
   {
     -- general buttons
      [1] = { heading = ccvSex,  btntype = buttonText,  btns = { ccvMale, ccvFemale } },
      [2] = { heading = ccvHandedness, btntype = buttonText, btns = { ccvLeft, ccvRight } },
      [3] = { heading = ccvClass, btntype = buttonText, btns = { ccvFighter, ccvMage, ccvBard, ccvTinker, ccvDruid, ccvPaladin, ccvRanger, ccvShepard } },

     -- fighter skills
      [4] = { heading = ccvSkill, btntype = btnText, btns = { ccvAttack, ccvDefence } },
      [5] = { heading = ccvSkill, btntype = btnText, btns = { ccvUnarmed, ccvSword, ccvAxe, ccvMace, ccvMissile } },
      [6] = { heading = ccvSkill, btntype = btnText, btns = { ccvSwimming, ccvTraps, ccvSearch, ccvCharm, ccvAcrobat, ccvAppraise } },

     -- mage Skills
      [7] = { heading = ccvSkill, btntype = btnText, btns = { ccvMana, ccvCasting } },

     -- bard skills
      [8] = { heading = ccvSkill, btntype = btnText, btns = { ccvLore, ccvCharm } },
      [9] = { heading = ccvSkill, btntype = btnText, btns = { ccvAppraise, ccvAcrobat, ccvSneak, ccvPicklock, ccvSearch, ccvSwimming } },
      [10]= { heading = ccvSkill, btntype = btnText, btns = { ccvMana, ccvCasting, ccvSword, ccvAxe, ccvMace, ccvUnarmed,  ccvMissile } },

     -- tinker skills
      -- first tinker skills page same as fighter skills page 2 (index 5)
      [11]= { heading = ccvSkill, btntype = btnText, btns = { ccvPicklock, ccvTraps, ccvSearch, ccvAppraise, ccvRepair } },

     -- druid skills
      [12]= { heading = ccvSkill, btntype = btnText, btns = { ccvTrack, ccvLore, ccvSearch } },

     -- paladin skills
      [13]= { heading = ccvSkill, btntype = btnText, btns = { ccvAppraise, ccvCharm, ccvAcrobat, ccvRepair } },
      -- second paladin skill page same as fighter skills page 2 (index 5)

     -- ranger skills
      [14]= { heading = ccvSkill, btntype = btnText, btns = { ccvTraps, ccvAcrobat, ccvSneak, ccvSearch, ccvSwimming, ccvRepair } },
      [15]= { heading = ccvSkill, btntype = btnText, btns = { ccvUnarmed, ccvSword, ccvAxe, ccvMace, ccvMissile, ccvAttack, ccvDefence, ccvTrack } },

     -- shepard skills  (note: the original game had the last 2 pages combined on one page with 10 buttons)
      [16]= { heading = ccvSkill, btntype = btnText, btns = { ccvUnarmed, ccvSword, ccvAxe, ccvMace, ccvMissile, ccvDefence } },
      [17]= { heading = ccvSkill, btntype = btnText, btns = { ccvTraps, ccvSneak, ccvAppraise, ccvTrack, ccvLore, ccvSearch, ccvAcrobat, ccvSwimming, ccvCasting, ccvMana } },

     -- appearance
      [18]= { heading = ccvNone, btntype = buttonImage, btns = {  7,  8,  9, 10, 11 } },
      [19]= { heading = ccvNone, btntype = buttonImage, btns = { 12, 13, 14, 15, 16 } },

     -- general buttons
      [20]= { heading = ccvDifficulty, btntype = buttonText, btns = { ccvStandard, ccvEasy } },
      [21]= { heading = ccvNone, btntype = buttonInput, btns = { ccvName }},
      [22]= { heading = ccvKeep, btntype = buttonText, btns = { ccvYes, ccvNo } },

      [23]= { heading = 1000, btntype = btTimer, btns = { } }
   },

   skillorder = -- table with the skill button group order for every player class
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

   skilltrans = -- translates ccvXXX to player_skill_XXX values (defined in uwinterface.lua)
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

-- initializes/deinitializes create character screen
function cchar_global(globalaction, seed)
   if globalaction==gactInit then
      skills = {}
      randomseed(seed)
      cchar_do_action(actSetInitVal, ccharui.strblock, ccharui.btngxcoord, ccharui.textcolor_normal, ccharui.textcolor_highlight, ccharui.btnimages)
      finished = 0
   end

   -- end it all when we're at the first page and a deinit was received
   if globalaction==gactDeinit and curgroup==1 then
      cchar_do_action(actEnd)
   else
      -- show the first button group
      curstep = 0
      psex = 0
      pclass = 0
      pstr = 0
      pdex = 0
      pint = 0
      pvit = 0
      img = 0
      pname = ""
      curgroup = 1
      numberofskills = 0
      cchar_do_action(actUIClear)
      cchar_do_action(actSetUIBtnGroup, ccharui.btngroups[curgroup].heading, ccharui.btngroups[curgroup].btntype, ccharui.btngroups[curgroup].btns)
      cchar_do_action(actUIUpdate)
   end
end


-- adds a value to, or changes a value in, the skills table
function cchar_addskill(skill, value)
   -- try to find the skill in the table
   for csi = 1, numberofskills do
      -- increase the value if found and return
      if skills[csi].name==skill then
         skills[csi].val = skills[csi].val + value/2
         cchar_do_action(actSetPlayerSkill, ccharui.skilltrans[skill], skills[csi].val)
         return
      end
   end
   -- not found? add the skill
   numberofskills = numberofskills + 1
   skills[numberofskills] = { name = skill, val = value };
   cchar_do_action(actSetPlayerSkill, ccharui.skilltrans[skill], value)
end


-- processes action on a button click in the current group
function cchar_buttonclick(button, text)

   if finished == 1 and curgroup < 22
   then
      return
   end

   -- simple case, button groups in sequential order
   if curgroup<=2 or curgroup>=18 then

      if curgroup==1 then
        cchar_do_action(actSetPlayerAttr, player_attr_gender, button)
        psex = button

      elseif curgroup==2 then
        cchar_do_action(actSetPlayerAttr, player_attr_handedness, button)

      elseif curgroup==18 then
         cchar_do_action(actSetPlayerAttr, player_attr_appearance, button)
         pimg = button
         curgroup = curgroup + 1

      elseif curgroup==19 then
         cchar_do_action(actSetPlayerAttr, player_attr_appearance, button)
         pimg = button + 5

      elseif curgroup==20 then
         cchar_do_action(actSetPlayerAttr, player_attr_difficulty, button)

      elseif curgroup==21 then
         if strlen(text)<1 then
            return  -- don't accept an empty name
         else
            pname = text
            cchar_do_action(actSetPlayerName, pname)
         end

      elseif curgroup==22 then
         if button==1 then
            cchar_global(gactDeinit)
            return
         end

      elseif curgroup==23 then
         curgroup = -1
         cchar_do_action(actEnd, eaNewGame)
         return
      end
      curgroup = curgroup + 1

   -- special case, player skills button groups (depend on player class)
   elseif curgroup>=3 and curgroup<=17 then

      if curgroup==3 then   -- player class selection button
         cchar_do_action(actSetPlayerAttr, player_attr_profession, button)
         pclass = button
         curstep = 0

         pstr = random(20,30)
         pdex = random(15,25)
         pint = random(12,22)
         pvit = random(33,36)
         cchar_do_action(actSetPlayerAttr, player_attr_strength, pstr)
         cchar_do_action(actSetPlayerAttr, player_attr_dexterity, pdex)
         cchar_do_action(actSetPlayerAttr, player_attr_intelligence, pint)
         cchar_do_action(actSetPlayerAttr, player_attr_max_life, pvit)
         cchar_do_action(actSetPlayerAttr, player_attr_life, pvit)

         -- set mana/max_mana attribute (note: this is not the same as mana skill)
         local max_mana = 0
         if button==0 or button==3 or button==6 or button==7 then   -- fighter/tinker/ranger/shepard
            max_mana = 2
         elseif button==1 then  -- mage
            max_mana = 35
         elseif button==2 then  -- bard
            max_mana = 15
         elseif button==4 then  -- druid
            max_mana = 27
         elseif button==5 then  -- paladin
            max_mana = 3
         end
         cchar_do_action(actSetPlayerAttr, player_attr_max_mana, max_mana)
         cchar_do_action(actSetPlayerAttr, player_attr_mana, max_mana)


         -- the attack and defence skill appear for all player classes
         cchar_addskill(ccvAttack, random(4,13))
         cchar_addskill(ccvDefence, random(4,13))

         -- add class specific inital skills
         if button==1 or button==4 then   -- mage/druid
            cchar_addskill(ccvMana, random(4,13))
            cchar_addskill(ccvCasting, random(4,13))
         elseif button==3 then            -- tinker
            cchar_addskill(ccvRepair, random(4,13))
         elseif button==5 then            -- paladin
            cchar_addskill(ccvCharm, random(4,13))
         elseif button==6 then            -- ranger
            cchar_addskill(ccvTrack, random(4,13))
         end

      else   -- class specific skill selection button
         cchar_addskill(ccharui.btngroups[curgroup].btns[button+1], random(4,13))
         curstep = curstep + 1
      end
      curgroup = ccharui.skillorder[pclass][curstep]

      -- determine which set of appearance buttons to show
      if curgroup==0 then
         if psex==0 then
            curgroup = 18
         else
            curgroup = 19
         end
      end
   end

   -- show the new button group and stats (or end)
   if curgroup>0 then
      cchar_do_action(actUIClear)

      if curgroup>1 then
         cchar_do_action(actSetUIText, psex+ccvMale, 18, 21, alLeft)
      end
      if curgroup>3 then
         -- common stats
         cchar_do_action(actSetUIText, pclass+ccvFighter, 141, 21, alRight)
         cchar_do_action(actSetUIText, ccvStrc, 93, 50, alLeft)
         cchar_do_action(actSetUINumber, pstr, 139, 50)
         cchar_do_action(actSetUIText, ccvDexc, 93, 67, alLeft)
         cchar_do_action(actSetUINumber, pdex, 139, 67)
         cchar_do_action(actSetUIText, ccvIntc, 93, 84, alLeft)
         cchar_do_action(actSetUINumber, pint, 139, 84)
         cchar_do_action(actSetUIText, ccvVitc, 93, 101, alLeft)
         cchar_do_action(actSetUINumber, pvit, 139, 101)
      end

      -- class/skill specific stats
      for csi = 1, numberofskills do
         cchar_do_action(actSetUIText, skills[csi].name, 30, 132+11*(csi-1), alLeft)
         cchar_do_action(actSetUINumber, skills[csi].val, 125, 132+11*(csi-1))
      end

      if curgroup>19 then
         cchar_do_action(actSetUIImg, 17+pimg, 44, 81)
      end

      if curgroup>21 then
         cchar_do_action(actSetUICustText, pname, 80, 10, alCenter)
      end

      if curgroup>22 then
         -- show "playname enters the abyss..."
         cchar_do_action(actSetUICustText, pname, 240, 90, alCenter)
         -- we need one string (#256) from stringblock 1
         cchar_do_action(actSetUIText, 256, 240, 100, alCenter, 1)
         -- we're finished (don't allow any more input)
         finished = 1
      end

      cchar_do_action(actSetUIBtnGroup, ccharui.btngroups[curgroup].heading, ccharui.btngroups[curgroup].btntype, ccharui.btngroups[curgroup].btns)
      cchar_do_action(actUIUpdate)
   else
      cchar_do_action(actEnd, eaCancel)
   end
end
