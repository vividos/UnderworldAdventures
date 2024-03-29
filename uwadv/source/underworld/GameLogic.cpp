//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2006,2019,2021 Underworld Adventures Team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
/// \file GameLogic.cpp
/// \brief game logic
//
#include "pch.hpp"
#include "GameLogic.hpp"
#include "IScripting.hpp"

using Underworld::GameLogic;

GameLogic::GameLogic(IScripting* scripting)
   :m_scripting(scripting),
   m_lastPlayerLevel(std::numeric_limits<size_t>::max())
{
   m_lastEvalTime = -1.0;

   m_isStopped = false;
   m_isAttacking = false;
   m_attackPower = 0;

   m_underworld.GetPlayer().GetInventory().SetObjectProperties(GetObjectProperties());
}

void GameLogic::EvaluateUnderworld(double time)
{
   if (m_isStopped)
      return;

   //double elapsed = m_lastEvalTime > 0 ? time - m_lastEvalTime : 0.1;

   if (m_isAttacking)
   {
      m_attackPower += 5;
      if (m_attackPower > 100)
         m_attackPower = 100;

      if (m_userInterface != NULL)
         m_userInterface->Notify(::notifyUpdatePowergem);
   }

   CheckAutomapTileChanged();
   CheckMoveTrigger();

   m_lastEvalTime = time;
}

static const char* UserActionToDisplayText(UserInterfaceUserAction action)
{
   switch (action)
   {
   case userActionNothing: return "Nothing";
   case userActionLookObject: return "LookObject";
   case userActionLookObjectInventory: return "LookObjectInventory";
   case userActionLookWall: return "LookWall";
   case userActionUseObject: return "UseObject";
   case userActionUseObjectInventory: return "UseObjectInventory";
   case userActionUseWall: return "UseWall";
   case userActionGetObject: return "GetObject";
   case userActionTalkObject: return "TalkObject";
   case userActionClickedActiveSpell: return "ClickedActiveSpell";
   case userActionClickedRuneshelf: return "ClickedRuneshelf";
   case userActionClickedCompass: return "ClickedCompass";
   case userActionClickedVitalityFlash: return "ClickedVitalityFlask";
   case userActionClickedManaFlask: return "ClickedManaFlask";
   case userActionClickedGargoyle: return "ClickedGargoyle";
   case userActionClickedDragons: return "ClickedDragons";
   case userActionTrackCreatures: return "TrackCreatures";
   case userActionSleep: return "Sleep";
   case userActionCombatEnter: return "CombatEnter";
   case userActionCombatLeave: return "CombatLeave";
   case userActionCombatDrawBack: return "CombatDrawBack";
   case userActionCombatRelease: return "CombatRelease";
   case userActionTargetSelected: return "TargetSelected";
   default:
      UaAssert(false);
      return "???";
   }
}

void GameLogic::UserAction(UserInterfaceUserAction action,
   unsigned int param)
{
   if (m_scripting == NULL)
      return;

   UaTrace("user action: action=%s param=%u\n",
      UserActionToDisplayText(action), param);

   switch (action)
   {
   case userActionCombatDrawBack:
      m_isAttacking = true;
      m_attackPower = 0;
      // additionally tell scripting with type of attack
      m_scripting->UserAction(action, param);
      break;

   case userActionCombatRelease:
      // do the actual attack
      m_scripting->UserAction(action, m_attackPower);
      UaTrace("attacking with power=%u\n", m_attackPower);

      // switch off m_isAttacking
      m_isAttacking = false;
      m_attackPower = 0;

      if (m_userInterface != NULL)
         m_userInterface->Notify(::notifyUpdatePowergem);
      break;

   default:
      // pass on to scripting
      m_scripting->UserAction(action, param);
      break;
   }
}

void GameLogic::ChangeLevel(size_t level)
{
   // check if game wants to change to unknown level
   UaAssert(level < m_underworld.GetLevelList().GetNumLevels());

   m_underworld.GetPlayer().SetAttribute(::Underworld::attrMapLevel, static_cast<Uint16>(level));

   // clear activated move triggers
   m_activeTriggers.clear();

   // tell user interface and scripting about change
   if (m_userInterface != NULL)
      m_userInterface->Notify(::notifyLevelChange);

   if (m_scripting != NULL)
      m_scripting->OnChangingLevel();
}

unsigned int GameLogic::GetInventoryWeight() const
{
   const Inventory& inventory = GetUnderworld().GetPlayer().GetInventory();

   return inventory.GetInventoryWeight() / 10;
}

void GameLogic::CheckAutomapTileChanged()
{
   if (GetUnderworld().GetCurrentLevel().GetTilemap().IsAutomapDisabled())
      return;

   const Player& player = GetUnderworld().GetPlayer();
   size_t currentLevel = player.GetAttribute(attrMapLevel);

   unsigned int tileX = (unsigned int)player.GetXPos();
   unsigned int tileY = (unsigned int)player.GetYPos();

   if (m_lastPlayerLevel != currentLevel ||
      m_lastPlayerTileX != tileX ||
      m_lastPlayerTileY != tileY)
   {
      RevealAutomapAtTile(tileX, tileY);
   }

   m_lastPlayerLevel = currentLevel;
   m_lastPlayerTileX = tileX;
   m_lastPlayerTileY = tileY;
}

void GameLogic::RevealAutomapAtTile(unsigned int tileX, unsigned tileY)
{
   Level& level = GetCurrentLevel();

   for (int offsetX = -2; offsetX <= 2; offsetX++)
      for (int offsetY = -2; offsetY <= 2; offsetY++)
      {
         int x = (int)tileX + offsetX;
         int y = (int)tileY + offsetY;
         if (x < 0 || x >= c_underworldTilemapSize ||
            y < 0 || y >= c_underworldTilemapSize)
            continue;

         unsigned int posX = (unsigned int)x;
         unsigned int posY = (unsigned int)y;

         TileInfo& tileInfo = level.GetTilemap().GetTileInfo(posX, posY);

         bool isBorderTile = offsetX == -2 || offsetX == 2 || offsetY == -2 || offsetY == 2;

         // for tiles with offset 2 away, reveal solid tiles
         if ((tileInfo.m_type == tileSolid && isBorderTile) ||
            !isBorderTile)
            tileInfo.m_automapFlag = level.GetAutomapFlagFromTile(posX, posY);
      }
}

void GameLogic::CheckMoveTrigger()
{
   double playerPos[3] =
   {
      m_underworld.GetPlayer().GetXPos(),
      m_underworld.GetPlayer().GetYPos(),
      m_underworld.GetPlayer().GetHeight(),
   };

   unsigned int tilex = static_cast<unsigned int>(playerPos[0]);
   unsigned int tiley = static_cast<unsigned int>(playerPos[1]);

   // check all surrounding tiles
   for (int i = -1; i < 2; i++)
      for (int j = -1; j < 2; j++)
      {
         int x = static_cast<int>(tilex) + i;
         int y = static_cast<int>(tiley) + j;
         if (x < 0 || y < 0 || x >= 64 || y >= 64)
            continue;

         ObjectList& objectList = GetCurrentLevel().GetObjectList();

         // check tile objects for move trigger
         Uint16 pos =
            objectList.GetListStart(
               static_cast<Uint8>(x), static_cast<Uint8>(y));

         while (pos != 0)
         {
            // check if move trigger
            const ObjectPtr& obj = objectList.GetObject(pos);
            if (obj->GetObjectInfo().m_itemID == 0x01a0)
            {
               // found move trigger; check if it's in range
               ObjectPositionInfo& posInfo = obj->GetPosInfo();

               double triggerPos[3] =
               {
                  posInfo.m_tileX + (posInfo.m_xpos + 0.5) / 8.0,
                  posInfo.m_tileY + (posInfo.m_ypos + 0.5) / 8.0,
                  double(posInfo.m_zpos),
               };

               double dx = triggerPos[0] - playerPos[0];
               double dy = triggerPos[1] - playerPos[1];
               double dz = triggerPos[2] - playerPos[2];

               double distance = sqrt(dx * dx + dy * dy + dz * dz);

               if (distance < 0.5)
               {
                  // trigger in range

                  // check if trigger already active
                  if (m_activeTriggers.find(pos) == m_activeTriggers.end())
                  {
                     // not active yet
                     m_activeTriggers.insert(pos);

                     UaTrace("move trigger: activate trigger at %04x\n", pos);

                     if (m_scripting != NULL)
                        m_scripting->TriggerSetOff(pos);
                  }
                  else
                  {
                     // trigger is active; do nothing
                  }
               }
               else
               {
                  // not in range; check if we can deactivate it
                  if (m_activeTriggers.find(pos) != m_activeTriggers.end())
                  {
                     UaTrace("move trigger: deactivate trigger at %04x\n", pos);
                     m_activeTriggers.erase(pos);
                  }
               }
            }

            // next object in chain
            pos = obj->GetObjectInfo().m_link;
         }
      }
}
