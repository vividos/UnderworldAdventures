//
// Underworld Adventures - an Ultima Underworld hacking project
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
/// \file Panel.hpp
/// \brief panel for inventory/stats/rune bag
//
#pragma once

#include "Panel.hpp"
#include "ImageQuad.hpp"
#include "Font.hpp"

/// panel types
enum PanelType
{
   panelInventory = 0, ///< inventory panel
   panelStats,       ///< stats panel
   panelRunebag,     ///< runebag panel
};

/// panel parent interface
class IPanelParent
{
public:
   /// ctor
   IPanelParent() {}
   /// dtor
   virtual ~IPanelParent() {}

   /// sets cursor image
   virtual void SetCursor(int index, bool priority = false) = 0;

   /// returns game interface
   virtual IGame& GetGameInterface() = 0;
};


/// panel
/// \todo when rotating adjust y coords of windows, too, like in the original
class Panel : public ImageQuad
{
public:
   /// ctor
   Panel();

   /// initializes panel
   virtual void Init(IPanelParent* panelParent, unsigned int xpos,
      unsigned int ypos);

   // virtual functions from Window
   virtual void Destroy() override;
   virtual void Draw() override;
   virtual bool ProcessEvent(SDL_Event& event) override;
   virtual void MouseEvent(bool buttonClicked, bool leftButton,
      bool buttonDown, unsigned int mouseX, unsigned int mouseY) override;
   virtual void Tick() override;

protected:
   /// updates panel image
   void UpdatePanel();

   /// update chain images only
   void UpdateChains();

   /// updates inventory panel
   void UpdateInventory();

   /// updates stats panel
   void UpdateStats();

   /// updates runebag panel
   void UpdateRunebag();

   /// clicked somewhere on inventory panel
   void OnInventoryClick(bool buttonDown, bool leftButton,
      unsigned int area);

   /// updates cursor image, e.g. when starting dragging an item
   void UpdateCursorImage();

protected:
   /// interface to panel owner
   IPanelParent* m_panelParent;

   /// panel type
   PanelType m_panelType;

   /// indicates if female armor images are used
   bool m_useFemaleArmor;

   /// tickrate in ticks/s
   double m_tickRate;

   /// start of scrollable stats area
   unsigned int m_startScrollableStatsArea;

   // images / image lists

   /// panel background images
   std::vector<IndexedImage> m_panelBackgroundImages;

   /// chains images
   std::vector<IndexedImage> m_chainImages;

   /// inventory background for bags
   IndexedImage m_inventoryBagBackground;

   /// up/down buttons for inventory
   std::vector<IndexedImage> m_inventoryUpDownImages;

   /// armor pieces for paperdoll
   std::vector<IndexedImage> m_paperdollArmorImages;

   /// paperdoll body images
   std::vector<IndexedImage> m_paperdollBodyImages;

   /// inventory objects
   std::vector<IndexedImage> m_inventoryObjectImages;

   /// chains image on top
   ImageQuad m_chainsTopImage;

   /// chains image on bottom
   ImageQuad m_chainsBottomImage;

   /// font for stats list
   Font m_statsFont;

   /// font for inventory weight
   Font m_inventoryWeightFont;

   /// indicates if panel is currently rotating
   bool m_isPanelRotating;

   /// rotation angle
   double m_panelRotateAngle;

   /// new panel that is shown when rotation completes
   PanelType m_panelTypeAfterRotation;

   /// indicates if old-style "jerky" rotation should be used
   bool m_useOldStyleRotation;

   // inventory / item dragging

   /// start of inventory slots the user sees
   unsigned int m_inventorySlotStart;

   /// indicates if dragging should be checked
   bool m_checkDragging;

   /// item index that is currently dragged
   Uint16 m_draggedItemIndex;

   /// area the item is dragged from
   unsigned int m_draggedItemArea;
};
