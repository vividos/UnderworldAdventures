//
// Underworld Adventures Debugger - a debugger tool for Underworld Adventures
// Copyright (c) 2004,2005,2019 Underworld Adventures Team
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
/// \file TileMapViewCtrl.hpp
/// \brief tilemap view control
//
#pragma once

#include "resource.h"
#include <atlscrl.h>

class DebugClient;

/// info about a tile on the tilemap
struct TileMapInfo
{
public:
   /// ctor
   TileMapInfo() :m_tileType(0), m_floorHeight(0), m_ceilingHeight(0), m_slope(0),
      m_textureWall(0), m_textureFloor(0), m_textureCeiling(0) {}

   unsigned int m_tileType;
   unsigned int m_floorHeight;
   unsigned int m_ceilingHeight;
   unsigned int m_slope;
   unsigned int m_textureWall;
   unsigned int m_textureFloor;
   unsigned int m_textureCeiling;
   unsigned int m_nObjectListStart;
};

/// tilemap view control; draws tilemap
class TileMapViewCtrl :
   public CScrollWindowImpl<TileMapViewCtrl, CWindow>,
   public DebugWindowBase
{
   typedef CScrollWindowImpl<TileMapViewCtrl, CWindow> baseClass;
public:
   DECLARE_WND_CLASS_EX(_T("TileMapViewCtrl"), CS_DBLCLKS, COLOR_WINDOW);

   /// ctor
   TileMapViewCtrl();
   /// dtor
   virtual ~TileMapViewCtrl() {}

   /// returns tile x size
   unsigned int GetTileSizeX() const { return m_tileSizeX; }
   /// returns tile y size
   unsigned int GetTileSizeY() const { return m_tileSizeY; }

   /// returns currently selected tile X and Y coordinates, or false when none is selected
   bool GetSelectedTileXY(unsigned int& selectedTileX, unsigned int& selectedTileY) const;

   /// handles scrolled painting; called by CScrollWindowImpl
   void DoPaint(CDCHandle dc);

private:
   // virtual methods from DebugWindowBase
   virtual void ReceiveNotification(DebugWindowNotification& notify);

   /// initializes scrolling
   void Init();

   /// returns tile map info for a given tile
   TileMapInfo& GetTileMapInfo(unsigned int x, unsigned int y);

   /// updates tile map values
   void UpdateTileMap();

private:
   BEGIN_MSG_MAP(TileMapViewCtrl)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
      COMMAND_ID_HANDLER(ID_TILEMAP_ZOOMIN, OnTilemapZoomIn)
      COMMAND_ID_HANDLER(ID_TILEMAP_ZOOMOUT, OnTilemapZoomOut)
      CHAIN_MSG_MAP(baseClass)
   END_MSG_MAP()

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      Init();
      bHandled = FALSE;
      return 1;
   }

   LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnTilemapZoomIn(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnTilemapZoomOut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
   /// array with all tile map infos
   CAtlArray<TileMapInfo> m_tileMapInfos;

   /// current tile x and y size
   unsigned int m_tileSizeX, m_tileSizeY;

   /// selected tile x and y coordinates;
   unsigned int m_selectedTileX, m_selectedTileY;
};
