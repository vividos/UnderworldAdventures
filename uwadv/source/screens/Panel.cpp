//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
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
/// \file Panel.cpp
/// \brief inventory/stats/rune bag panel
//
#include "pch.hpp"
#include "Panel.hpp"
#include "Underworld.hpp"
#include "IScripting.hpp"
#include "GameStrings.hpp"
#include "ImageManager.hpp"
#include "Math.hpp"
#include <sstream>

/// time to rotate panel
const double c_panelRotateTime = 2.5;

/// inventory area types
enum PanelInventoryAreaId
{
   areaInventorySlot0 = 0,
   areaInventorySlot1,
   areaInventorySlot2,
   areaInventorySlot3,
   areaInventorySlot4,
   areaInventorySlot5,
   areaInventorySlot6,
   areaInventorySlot7,

   areaInventoryContainer,
   areaInventoryScroll_up,
   areaInventoryScroll_down,

   areaEquipmentLeftHand,
   areaEquipmentLeftShoulder,
   areaEquipmentLeftRing,

   areaEquipmentRightHand,
   areaEquipmentRightShoulder,
   areaEquipmentRightRing,

   areaPaperdollHead,
   areaPaperdollChest,
   areaPaperdollHand,
   areaPaperdollLegs,
   areaPaperdollFeet,

   areaNone
};

/// inventory area table
struct {
   PanelInventoryAreaId areaId;
   unsigned int xmin, xmax, ymin, ymax;
} g_panelInventoryAreas[] =
{
   { areaInventorySlot0,  5, 24, 76,  93 },
   { areaInventorySlot1, 25, 43, 76,  93 },
   { areaInventorySlot2, 44, 62, 76,  93 },
   { areaInventorySlot3, 63, 80, 76,  93 },
   { areaInventorySlot4,  5, 24, 94, 110 },
   { areaInventorySlot5, 25, 43, 94, 110 },
   { areaInventorySlot6, 44, 62, 94, 110 },
   { areaInventorySlot7, 63, 80, 94, 110 },

   { areaInventoryContainer,    5, 24, 60, 75 },
   { areaInventoryScroll_up,   61, 70, 64, 75 },
   { areaInventoryScroll_down, 71, 80, 64, 75 },

   { areaEquipmentLeftHand,      7, 24, 29, 45 },
   { areaEquipmentLeftShoulder, 10, 27,  7, 23 },
   { areaEquipmentLeftRing,     24, 31, 50, 57 },

   { areaEquipmentRightHand,     61, 77, 29, 45 },
   { areaEquipmentRightShoulder, 59, 75,  7, 23 },
   { areaEquipmentRightRing,     55, 62, 50, 57 },

   { areaPaperdollHead,  36, 49,  5, 22 },
   { areaPaperdollChest, 34, 51, 23, 44 },
   { areaPaperdollHand,  27, 33, 38, 49 },
   { areaPaperdollHand,  52, 58, 38, 49 }, // 2nd hand area
   { areaPaperdollLegs,  34, 51, 45, 60 },
   { areaPaperdollFeet,  32, 54, 61, 75 },
};

Panel::Panel()
{
}

void Panel::Init(IPanelParent* panelParent, unsigned int xpos,
   unsigned int ypos)
{
   m_panelParent = panelParent;
   IGame& game = m_panelParent->GetGameInterface();
   IBasicGame& gameInstance = m_panelParent->GetGameInterface();

   m_panelType = panelInventory;
   m_useFemaleArmor = gameInstance.GetUnderworld().GetPlayer().GetAttribute(Underworld::attrGender) != 0;
   m_tickRate = game.GetTickRate();
   m_startScrollableStatsArea = 0;
   m_isPanelRotating = false;
   m_panelRotateAngle = 0.0;
   m_useOldStyleRotation = !gameInstance.GetSettings().GetBool(Base::settingUwadvFeatures);

   m_inventorySlotStart = 0;
   m_checkDragging = false;

   // load image lists
   ImageManager& imageManager = gameInstance.GetImageManager();

   imageManager.LoadList(m_panelBackgroundImages, "panels", 0, 3);
   imageManager.LoadList(m_chainImages, "chains");
   imageManager.Load(m_inventoryBagBackground, "inv", 6);
   imageManager.LoadList(m_inventoryUpDownImages, "buttons", 27, 29);

   imageManager.LoadList(m_paperdollArmorImages, m_useFemaleArmor ? "armor_f" : "armor_m");

   imageManager.LoadList(m_paperdollBodyImages, "bodies");
   imageManager.LoadList(m_inventoryObjectImages, "objects");

   // load fonts
   m_statsFont.Load(gameInstance.GetResourceManager(), fontItalic);
   m_inventoryWeightFont.Load(gameInstance.GetResourceManager(), fontNormal);

   // create image
   GetImage().Create(85, 116);
   GetImage().SetPalette(imageManager.GetPalette(0));

   ImageQuad::Init(game, xpos, ypos);

   {
      m_chainsTopImage.Init(gameInstance, xpos + 37, ypos - 4);
      m_chainsTopImage.GetImage().Create(10, 4);
      m_chainsTopImage.GetImage().SetPalette(gameInstance.GetImageManager().GetPalette(0));

      m_chainsBottomImage.Init(gameInstance, xpos + 37, ypos + 114);
      m_chainsBottomImage.GetImage().Create(10, 18);
      m_chainsBottomImage.GetImage().SetPalette(gameInstance.GetImageManager().GetPalette(0));

      UpdateChains();
   }

   UpdatePanel();
}

void Panel::Destroy()
{
   ImageQuad::Destroy();
}

void Panel::Draw()
{
   if (m_isPanelRotating)
   {
      // save position
      unsigned int save_xpos = m_windowXPos;
      unsigned int save_width = m_windowWidth;

      // calculate new position
      if (m_useOldStyleRotation)
      {
         // old-style: calculate angles in steps of 22.5
         double angle = (unsigned(m_panelRotateAngle / 180.0 * 8.0) / 8.0 * 180.0);

         // ensure at least a small panel
         if (angle > 89.0 && angle < 91.0) angle = 87.0;

         m_windowWidth = unsigned(save_width * fabs(cos(Deg2rad(angle))));
      }
      else
      {
         // new-style: smooth
         m_windowWidth = unsigned(save_width*fabs(cos(Deg2rad(m_panelRotateAngle))));
      }

      m_windowXPos = save_xpos + save_width / 2 - m_windowWidth / 2;

      ImageQuad::Draw();

      m_windowXPos = save_xpos;
      m_windowWidth = save_width;
   }
   else
      ImageQuad::Draw();

   // also draw chains
   m_chainsTopImage.Draw();
   m_chainsBottomImage.Draw();
}

/// \todo when pressing bottom chain, check if user has an item on cursor
/// and prevent switching then.
bool Panel::ProcessEvent(SDL_Event& event)
{
   // don't process events when in rotate mode
   if (m_isPanelRotating)
      return ImageQuad::ProcessEvent(event);

   // check if bottom chain was pressed
   if (event.type == SDL_MOUSEBUTTONUP)
   {
      unsigned int xpos = 0, ypos = 0;
      CalcMousePosition(event, xpos, ypos);

      // in window?
      if (m_chainsBottomImage.IsInWindow(xpos, ypos))
      {
         m_isPanelRotating = true;
         m_panelRotateAngle = 0.0;

         // change to stats when in inventory, and to inventory when not
         if (m_panelType != panelInventory)
            m_panelTypeAfterRotation = panelInventory;
         else
            m_panelTypeAfterRotation = panelStats;
      }
   }

   return ImageQuad::ProcessEvent(event);
}

bool Panel::MouseEvent(bool buttonClicked, bool leftButton,
   bool buttonDown, unsigned int mouseX, unsigned int mouseY)
{
   if (m_panelType == panelInventory)
   {
      PanelInventoryAreaId area = areaNone;

      mouseX -= m_windowXPos; mouseY -= m_windowYPos;

      // determine area
      for (unsigned int i = 0; i < SDL_TABLESIZE(g_panelInventoryAreas); i++)
      {
         // check ranges
         if (mouseX >= g_panelInventoryAreas[i].xmin &&
            mouseX <= g_panelInventoryAreas[i].xmax &&
            mouseY >= g_panelInventoryAreas[i].ymin &&
            mouseY <= g_panelInventoryAreas[i].ymax)
         {
            area = g_panelInventoryAreas[i].areaId;
            break;
         }
      }

      if (buttonClicked)
         OnInventoryClick(buttonDown, leftButton, area);
      else
      {
         // check item dragging
         if (m_checkDragging && m_draggedItemArea != static_cast<unsigned int>(area))
         {
            Underworld::Inventory& inventory = m_panelParent->GetGameInterface().
               GetUnderworld().GetPlayer().GetInventory();

            if (inventory.GetFloatingObjectPos() == Underworld::c_inventorySlotNoItem)
            {
               // user dragged item out of area
               m_checkDragging = false;
               inventory.FloatObject(m_draggedItemIndex);

               UpdateCursorImage();
               UpdatePanel();
            }
         }
      }
      /*
      #ifdef HAVE_DEBUG
            if (area != areaNone)
            {
               UpdateInventory();
               GetImage().IndexedImage::FillRect(
                  g_panelInventoryAreas[i].xmin,
                  g_panelInventoryAreas[i].ymin,
                  g_panelInventoryAreas[i].xmax-g_panelInventoryAreas[i].xmin,
                  g_panelInventoryAreas[i].ymax-g_panelInventoryAreas[i].ymin, 13);
               update();
            }
      #endif
      */
   }

   if (m_panelType == panelStats && buttonClicked && !buttonDown)
   {
      mouseX -= m_windowXPos; mouseY -= m_windowYPos;
      if (mouseX >= 29 && mouseX <= 56 && mouseY >= 106 && mouseY <= 114)
      {
         bool leftbutton = mouseX < 43;
         if (leftbutton)
         {
            if (m_startScrollableStatsArea > 0)
               m_startScrollableStatsArea--;
         }
         else
         {
            if (m_startScrollableStatsArea < 14)
               m_startScrollableStatsArea++;
         }
         UpdatePanel();
      }
   }

   if (m_panelType == panelRunebag && buttonClicked && !buttonDown)
   {
      mouseX -= m_windowXPos; mouseY -= m_windowYPos;
      if (mouseX >= 8 && mouseX < 80 && mouseY >= 7 && mouseY < 96)
      {
         // calculate which rune we hit
         mouseX -= 8;
         mouseY -= 7;
         mouseX /= 18;
         mouseY /= 15;

         UaTrace("runebag: mouse hit rune %u\n", mouseX + mouseY * 4);
      }
      else
      {
         if (mouseX >= 21 && mouseX < 68 && mouseY >= 96 && mouseY < 111)
         {
            UaTrace("runebag: mouse hit \"reset shelf\" area\n");
         }
         else
            UaTrace("runebag: mouse hit outside\n");
      }
   }

   return true;
}

void Panel::Tick()
{
   if (m_isPanelRotating)
   {
      double oldAngle = m_panelRotateAngle;
      m_panelRotateAngle += 180.0 / (c_panelRotateTime * m_tickRate);

      // check if we have to update the chains
      if (unsigned(oldAngle / 180.0*8.0) != unsigned(m_panelRotateAngle / 180.0*8.0))
         UpdateChains();

      // do we have to change panel image?
      if (m_panelRotateAngle >= 90.0)
      {
         m_panelType = m_panelTypeAfterRotation;
         UpdatePanel();
      }

      // check for rotation end
      if (m_panelRotateAngle >= 180.0)
      {
         m_isPanelRotating = false;
         UpdateChains();
      }
   }
}

void Panel::UpdatePanel()
{
   // inventory panel?
   if (m_panelType == panelInventory)
      UpdateInventory();
   else
      // stats panel?
      if (m_panelType == panelStats)
         UpdateStats();
      else
         // runebag panel?
         if (m_panelType == panelRunebag)
            UpdateRunebag();

   Update();
}

void Panel::UpdateChains()
{
   const Base::Settings& settings = m_panelParent->GetGameInterface().GetSettings();
   bool isUw2 = settings.GetGameType() == Base::gameUw2;

   unsigned int maxChainImage = isUw2 ? 4 : 8;

   unsigned int chainIndex = m_isPanelRotating ?
      unsigned(m_panelRotateAngle / 180.0 * maxChainImage) % maxChainImage : 0;

   // set chains images
   if (!isUw2)
   {
      m_chainsTopImage.GetImage().PasteImage(m_chainImages[chainIndex + 8], 0, 0);
      m_chainsBottomImage.GetImage().PasteImage(m_chainImages[chainIndex], 0, 0);

      m_chainsTopImage.Update();
      m_chainsBottomImage.Update();
   }
   else
   {
      m_chainsBottomImage.GetImage().PasteImage(m_chainImages[chainIndex], 0, 0);
      m_chainsBottomImage.Update();
   }
}

void Panel::UpdateInventory()
{
   IndexedImage& img = GetImage();

   Underworld::Inventory& inventory = m_panelParent->GetGameInterface().
      GetUnderworld().GetPlayer().GetInventory();
   Underworld::Player& player = m_panelParent->GetGameInterface().
      GetUnderworld().GetPlayer();

   // background image
   img.PasteImage(m_panelBackgroundImages[0], 1, 1);

   bool female = player.GetAttribute(Underworld::attrGender) != 0;

   // player appearance
   {
      unsigned int bodyimg = player.GetAttribute(Underworld::attrAppearance) % 5 +
         (female ? 5 : 0);

      img.PasteImage(m_paperdollBodyImages[bodyimg], 25, 5);
   }

   // check if paperdoll images have to be reloaded (e.g. when changed in
   // debugger)
   if (female != m_useFemaleArmor)
   {
      m_useFemaleArmor = female;

      m_paperdollArmorImages.clear();
      m_panelParent->GetGameInterface().GetImageManager().
         LoadList(m_paperdollArmorImages, m_useFemaleArmor ? "armor_f" : "armor_m");
   }

   // start index; 0 is used in the main inventory
   unsigned int start = 0;

   if (inventory.GetContainerPos() != Underworld::c_inventorySlotNoItem)
   {
      Uint16 cont_id = inventory.GetObjectInfo(inventory.GetContainerPos()).m_itemID;
      if (cont_id != Underworld::c_itemIDNone)
      {
         // inside a container

         // draw alternative inventory panel
         img.PasteImage(m_inventoryBagBackground, 1, 74);

         // draw up/down arrows
         if (m_inventorySlotStart > 0)
            img.PasteImage(m_inventoryUpDownImages[0], 61, 64, true); // up

         if (inventory.GetNumSlots() - m_inventorySlotStart >= 8)
            img.PasteImage(m_inventoryUpDownImages[1], 71, 64, true); // down

         // draw container we're in
         img.PasteImage(m_inventoryObjectImages[cont_id], 6, 58, true);

         // begin at current slot start
         start = m_inventorySlotStart;
      }
   }

   // paste inventory slots
   for (unsigned int i = 0; i < 8; i++)
   {
      if (start + i < inventory.GetNumSlots())
      {
         Uint16 item_pos = inventory.GetSlotListPos(start + i);
         Uint16 itemId = inventory.GetObjectInfo(item_pos).m_itemID;

         // draw item
         if (itemId != 0xffff)
         {
            unsigned int
               destx = 7 + (i & 3) * 19,
               desty = 76 + (i >> 2) * 18;

            img.PasteImage(m_inventoryObjectImages[itemId], destx, desty, true);
         }
      }
   }

   // do paperdoll items
   for (unsigned int j = Underworld::slotLeftHand; j < Underworld::slotMax; j++)
   {
      // paperdoll image coordinates
      static unsigned int slot_coords[] =
      {
           5,27,  59,27, // hand
           8, 5,  57, 5, // shoulder
          18,43,  48,43, // finger
          32,19,         // legs
          26,15,         // chest
          25,35,         // hands
          30,59,         // feet
          31, 3,         // head
      };

      unsigned int destx, desty;
      destx = slot_coords[(j - Underworld::slotLeftHand) * 2] + 1;
      desty = slot_coords[(j - Underworld::slotLeftHand) * 2 + 1] + 1;

      // paste item
      Uint16 itemId = inventory.GetObjectInfo(j).m_itemID;
      if (itemId != 0xffff)
      {
         if (j < Underworld::slotPaperdollStart)
         {
            // normal object
            img.PasteImage(m_inventoryObjectImages[itemId], destx, desty, true);
         }
         else
         {
            // paperdoll image
            unsigned int quality = 3; // can be between 0..3
            Uint16 armorimg = itemId < 0x002f ?
               (itemId - 0x0020) + 15 * quality : (itemId - 0x002f + 60);

            img.PasteImage(m_paperdollArmorImages[armorimg], destx, desty, true);
         }
      }
   }

   // inventory weight
   {
      unsigned int strength = m_panelParent->GetGameInterface().GetUnderworld().GetPlayer().GetAttribute(Underworld::attrStrength);
      unsigned int inventoryWeight = m_panelParent->GetGameInterface().GetGameLogic().GetInventoryWeight();
      unsigned int leftWeight = strength * 2 - inventoryWeight;

      std::ostringstream buffer;
      buffer << leftWeight;

      IndexedImage img_weight;
      m_inventoryWeightFont.CreateString(img_weight, buffer.str(), 224);
      img.PasteImage(img_weight, 70 - img_weight.GetXRes() / 2, 53, true);
   }
}

void Panel::UpdateStats()
{
   IndexedImage& img = GetImage();

   Underworld::Player& player = m_panelParent->GetGameInterface().
      GetUnderworld().GetPlayer();

   GameStrings& gameStrings = m_panelParent->GetGameInterface().
      GetGameStrings();

   img.PasteImage(m_panelBackgroundImages[2], 1, 1);

   IndexedImage tempImage;
   unsigned int xpos;
   std::string text;

   // player name
   text = player.GetName();
   xpos = (83 - m_statsFont.CalcLength(text)) / 2;
   m_statsFont.CreateString(tempImage, player.GetName(), 1);
   img.PasteRect(tempImage, 0, 0, tempImage.GetXRes(), tempImage.GetYRes(), xpos, 7, true);

   // player profession
   unsigned int prof = player.GetAttribute(Underworld::attrProfession) & 7;
   text = gameStrings.GetString(2, prof + 23);
   Base::String::Uppercase(text);

   m_statsFont.CreateString(tempImage, text, 1);
   img.PasteRect(tempImage, 0, 0, tempImage.GetXRes(), tempImage.GetYRes(), 7, 15, true);

   // player main stats
   for (unsigned int i = 0; i < 7; i++)
   {
      std::ostringstream buffer;
      switch (i)
      {
      case 0: buffer << player.GetAttribute(Underworld::attrExperienceLevel) << "TH"; break;
      case 1: buffer << player.GetAttribute(Underworld::attrStrength); break;
      case 2: buffer << player.GetAttribute(Underworld::attrDexterity); break;
      case 3: buffer << player.GetAttribute(Underworld::attrIntelligence); break;
      case 4: buffer << player.GetAttribute(Underworld::attrVitality) << "/" <<
         player.GetAttribute(Underworld::attrMaxVitality); break;
      case 5: buffer << player.GetAttribute(Underworld::attrMana) << "/" <<
         player.GetAttribute(Underworld::attrMaxMana); break;

      case 6: buffer << player.GetAttribute(Underworld::attrExperiencePoints); break;
      }

      xpos = 77 - m_statsFont.CalcLength(buffer.str());

      m_statsFont.CreateString(tempImage, buffer.str(), 1);
      img.PasteRect(tempImage, 0, 0, tempImage.GetXRes(), tempImage.GetYRes(), xpos, i * 7 + 15, true);

      buffer.str().erase();
   }

   // player skills
   for (unsigned int n = 0; n < 6; n++)
   {
      std::ostringstream buffer;

      // skill name
      text = gameStrings.GetString(2, n + m_startScrollableStatsArea + 31);
      Base::String::Uppercase(text);

      m_statsFont.CreateString(tempImage, text, 104);
      img.PasteRect(tempImage, 0, 0, tempImage.GetXRes(), tempImage.GetYRes(), 7, n * 7 + 65, true);

      // skill value
      unsigned int skill = player.GetSkill(
         static_cast<Underworld::PlayerSkill>(Underworld::skillAttack + n + m_startScrollableStatsArea));

      buffer << skill;

      xpos = 77 - m_statsFont.CalcLength(buffer.str());

      m_statsFont.CreateString(tempImage, buffer.str(), 104);
      img.PasteRect(tempImage, 0, 0, tempImage.GetXRes(), tempImage.GetYRes(), xpos, n * 7 + 65, true);
   }
}

void Panel::UpdateRunebag()
{
   IndexedImage& img = GetImage();
   img.PasteImage(m_panelBackgroundImages[1], 1, 1);

   Underworld::Runebag& runebag = m_panelParent->GetGameInterface().
      GetUnderworld().GetPlayer().GetRunebag();

   for (unsigned int i = 0; i < 24; i++)
      if (runebag.IsInBag(static_cast<Underworld::RuneType>(i)))
         img.PasteRect(m_inventoryObjectImages[0x00e8 + i], 0, 0, 14, 14,
         (i & 3) * 18 + 9, (i >> 2) * 15 + 6, true);
}

void Panel::OnInventoryClick(bool buttonDown, bool leftButton,
   unsigned int the_area)
{
   PanelInventoryAreaId area =
      static_cast<PanelInventoryAreaId>(the_area);

   Underworld::Inventory& inventory = m_panelParent->GetGameInterface().
      GetUnderworld().GetPlayer().GetInventory();

   // check scroll up/down buttons
   if (!buttonDown && area == areaInventoryScroll_up && m_inventorySlotStart > 0)
   {
      m_inventorySlotStart -= m_inventorySlotStart > 4 ? 4 : m_inventorySlotStart;
      UpdatePanel();
      Update();
      return;
   }

   if (!buttonDown && area == areaInventoryScroll_down &&
      inventory.GetNumSlots() - m_inventorySlotStart >= 8)
   {
      m_inventorySlotStart += 4;
      UpdatePanel();
      Update();
      return;
   }

   // find out itemlist pos
   Uint16 item_pos = Underworld::c_inventorySlotNoItem;
   {
      switch (area)
      {
      case areaInventorySlot0: case areaInventorySlot1:
      case areaInventorySlot2: case areaInventorySlot3:
      case areaInventorySlot4: case areaInventorySlot5:
      case areaInventorySlot6: case areaInventorySlot7:
      {
         size_t containerIndex = m_inventorySlotStart + unsigned(area - areaInventorySlot0);
         if (containerIndex < inventory.GetNumSlots())
            item_pos = inventory.GetSlotListPos(containerIndex);
         else
            item_pos = ::Underworld::c_inventorySlotNoItem; // dropping after end of slot list
         break;
      }

      case areaEquipmentLeftHand: item_pos = Underworld::slotLeftHand; break;
      case areaEquipmentLeftShoulder: item_pos = Underworld::slotLeftShoulder; break;
      case areaEquipmentLeftRing: item_pos = Underworld::slotLeftFinger; break;

      case areaEquipmentRightHand: item_pos = Underworld::slotRightHand; break;
      case areaEquipmentRightShoulder: item_pos = Underworld::slotRightShoulder; break;
      case areaEquipmentRightRing: item_pos = Underworld::slotRightFinger; break;

      case areaPaperdollHead: item_pos = Underworld::slotPaperdollHead; break;
      case areaPaperdollChest: item_pos = Underworld::slotPaperdollChest; break;
      case areaPaperdollHand: item_pos = Underworld::slotPaperdollHands; break;
      case areaPaperdollLegs: item_pos = Underworld::slotPaperdollLegs; break;
      case areaPaperdollFeet: item_pos = Underworld::slotPaperdollFeet; break;

      case areaInventoryContainer: break;
      default: area = areaNone; break;
      }
   }

   if (area == areaNone)
      return; // no pos that is interesting for us

   // left/right button pressed
   if (buttonDown && !m_checkDragging && area != areaInventoryContainer)
   {
      // start checking for dragging items
      m_checkDragging = true;
      m_draggedItemIndex = item_pos;
      m_draggedItemArea = area;
      return;
   }

   if (buttonDown)
      return; // no more checks for pressed mouse button

   // left/right button release

   // stop dragging
   m_checkDragging = false;

   // check if we have a floating item
   if (inventory.GetFloatingObjectPos() != Underworld::c_inventorySlotNoItem)
   {
      // yes, floating

      // check for container icon "slot"
      if (area == areaInventoryContainer)
      {
         if (inventory.GetContainerPos() != Underworld::c_inventorySlotNoItem)
         {
            // put item into parent's container, if possible
            inventory.DropFloatingObject(inventory.GetParentContainerPos());
         }
      }
      else
      {
         // normal inventory slot

         // just drop the item on that position
         // an item on that slot gets the next floating one
         // items dropped onto a container are put into that container
         // items that are combineable will be combined
         // items that don't fit into a paperdoll slot will be rejected
         inventory.DropFloatingObject(inventory.GetContainerPos(), item_pos);
      }

      UpdateCursorImage();
      UpdatePanel();
      return;
   }

   // no floating object

   // click on container icon
   if (area == areaInventoryContainer)
   {
      // close container when not topmost
      if (inventory.GetContainerPos() != Underworld::c_inventorySlotNoItem)
         inventory.CloseContainer();

      UpdatePanel();
      return;
   }

   // check if container
   if (item_pos != Underworld::c_inventorySlotNoItem &&
      inventory.IsContainer(inventory.GetObjectInfo(item_pos).m_itemID))
   {
      // open container
      inventory.OpenContainer(item_pos);
      //isCursorObject = false;
      m_inventorySlotStart = 0;

      UpdatePanel();
      return;
   }

   IScripting& scripting = m_panelParent->GetGameInterface().GetScripting();

   // perform left/right click action
   if (leftButton)
   {
      // trigger "use" action
      if (item_pos != Underworld::c_itemIDNone)
         scripting.UserAction(userActionUseObjectInventory, item_pos);
   }
   else
   {
      // trigger "look" action
      if (item_pos != Underworld::c_itemIDNone)
         scripting.UserAction(userActionLookObjectInventory, item_pos);
   }
}

void Panel::UpdateCursorImage()
{
   Underworld::Inventory& inventory = m_panelParent->GetGameInterface().
      GetUnderworld().GetPlayer().GetInventory();

   Uint16 cursorObject = 0;
   bool isCursorObject = false;

   // check if we still have a floating object
   if (inventory.GetFloatingObjectPos() != Underworld::c_inventorySlotNoItem)
   {
      // still floating? then set new cursor object
      cursorObject = inventory.GetObjectInfo(inventory.GetFloatingObjectPos()).m_itemID;
      isCursorObject = cursorObject != Underworld::c_inventorySlotNoItem;
   }

   if (isCursorObject)
      m_panelParent->SetCursor(cursorObject + 0x0100, true);
   else
   {
      m_panelParent->SetCursor(-1, true);
      m_panelParent->SetCursor(0, false);
   }
}
