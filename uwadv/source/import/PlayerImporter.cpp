//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Underworld Adventures Team
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
/// \file PlayerImporter.cpp
/// \brief player importing
//
#include "pch.hpp"
#include "PlayerImporter.hpp"
#include "Player.hpp"
#include "ResourceManager.hpp"
#include "File.hpp"

using Import::PlayerImporter;

/// \todo import more player settings, including conv globals and inventory
/// \todo xpos and ypos only store tile position; how is intra-tile coordiate be calculated?
void PlayerImporter::LoadPlayer(Underworld::Player& player, const std::string& folder, bool initialPlayer)
{
   Base::File file = m_resourceManager.GetUnderworldFile(Base::resourceGameUw, folder + "/player.dat");

   Uint8 xorBase = !initialPlayer ? file.Read8() : 0;

   std::vector<Uint8> playerInfos(0x00dd);
   file.ReadBuffer(playerInfos.data(), playerInfos.size());

   if (!initialPlayer)
   {
      Uint8 currentNumber = 3;
      for (unsigned int i = 0; i < 220; i++)
      {
         if (i == 80 || i == 160)
            currentNumber = 3;

         playerInfos[i] ^= (xorBase + currentNumber);
         currentNumber += 3;
      }
   }

   char buffer[15] = {0};
   memcpy(buffer, &playerInfos[0], 14);
   buffer[14] = 0;

   player.SetName(std::string(buffer));
   /*
      attrGender=0,     ///< 0 means male
      attrHandedness=1, ///< 0 means left-handedness
      attrAppearance=2, ///< values from 0..4
      attrProfession=3, ///< values from 0..7, fighter, mage, bard, ...

      attrWeariness=12,
      attrPoisoned=14,    ///< 1 when poisoned
      attrMentalState=15, ///< drunk, tripping, etc. 0 means normal
      attrNightVision=16,

      attrTalks=17,       ///< number of conversations
      attrKills=18,       ///< number of successful kills
      attrExperienceLevel=19,   ///< experience level
      attrExperiencePoints=20,  ///< number of experience points

      attrDifficulty=21,  ///< 0=easy, 1=normal
   */
   player.SetAttribute(Underworld::attrStrength, playerInfos[0x001E]);
   player.SetAttribute(Underworld::attrDexterity, playerInfos[0x001F]);
   player.SetAttribute(Underworld::attrIntelligence, playerInfos[0x0020]);
   player.SetAttribute(Underworld::attrVitality, playerInfos[0x00DC]);
   player.SetAttribute(Underworld::attrMaxVitality, playerInfos[0x0036]);
   player.SetAttribute(Underworld::attrMana, playerInfos[0x0037]);
   player.SetAttribute(Underworld::attrMaxMana, playerInfos[0x0038]);

   player.SetAttribute(Underworld::attrHungriness, playerInfos[0x0039]);

   player.SetSkill(Underworld::skillAttack, playerInfos[0x0021]);
   player.SetSkill(Underworld::skillDefense, playerInfos[0x0022]);
   player.SetSkill(Underworld::skillUnarmed, playerInfos[0x0023]);
   player.SetSkill(Underworld::skillSword, playerInfos[0x0024]);
   player.SetSkill(Underworld::skillAxe, playerInfos[0x0025]);
   player.SetSkill(Underworld::skillMace, playerInfos[0x0026]);
   player.SetSkill(Underworld::skillMissile, playerInfos[0x0027]);
   player.SetSkill(Underworld::skillMana, playerInfos[0x0028]);
   player.SetSkill(Underworld::skillLore, playerInfos[0x0029]);
   player.SetSkill(Underworld::skillCasting, playerInfos[0x002A]);
   player.SetSkill(Underworld::skillTraps, playerInfos[0x002B]);
   player.SetSkill(Underworld::skillSearch, playerInfos[0x002C]);
   player.SetSkill(Underworld::skillTrack, playerInfos[0x002D]);
   player.SetSkill(Underworld::skillSneak, playerInfos[0x002E]);
   player.SetSkill(Underworld::skillRepair, playerInfos[0x002F]);
   player.SetSkill(Underworld::skillCharm, playerInfos[0x0030]);
   player.SetSkill(Underworld::skillPicklock, playerInfos[0x0031]);
   player.SetSkill(Underworld::skillAcrobat, playerInfos[0x0032]);
   player.SetSkill(Underworld::skillAppraise, playerInfos[0x0033]);
   player.SetSkill(Underworld::skillSwimming, playerInfos[0x0034]);

   player.SetAttribute(Underworld::attrTalks, 0);
   player.SetAttribute(Underworld::attrKills, 0);
   //   player.SetAttribute(Underworld::attrExperienceLevel, playerInfos[0x003D]); // really?

   Uint16 expPoints = Uint16(playerInfos[0x004E]) | (Uint16(playerInfos[0x004F]) << 8);
   player.SetAttribute(Underworld::attrExperiencePoints, expPoints);

   // only use high byte for positions
   double xpos = playerInfos[0x0056], ypos = playerInfos[0x0058], zpos = playerInfos[0x005A];
   player.SetPos(xpos, ypos);
   player.SetHeight(zpos);

   Uint8 mapLevel = playerInfos[0x005C];
   player.SetAttribute(Underworld::attrMapLevel, mapLevel);

   double rangle = playerInfos[0x005B] * (360.0 / 8.0);
   player.SetRotateAngle(rangle);

   /*
      003D   Int8    character level (play_level)
      ...
      0044   3*8bits rune flags (*)
      ...
      004C   Int16   weight in 0.1 stones
      ...
      005C   Int16   dungeon level
      ...
      005F   Int8    bits 2..5: play_poison
      ...
      00CE   Int32   game time
      ...
   */

   // runes
   /// \todo wrong; rune A is stored in bit 7 of first word, not in bit 0
   Uint32 uiRunes = Uint32(playerInfos[0x0044]) ||
      (Uint32(playerInfos[0x0045]) << 8) ||
      (Uint32(playerInfos[0x0046]) << 16);

   Underworld::Runebag& runebag = player.GetRunebag();
   runebag.GetBitsetRunebag().reset();

   for (unsigned int t = Underworld::runeAn; t < Underworld::runeLast; t++)
      runebag.SetRune(static_cast<Underworld::RuneType>(t),
      (Import::GetBits(uiRunes, t, 1) != 0));

   player.GetQuestFlags().Resize(35);
}
