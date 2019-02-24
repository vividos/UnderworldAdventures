//
// Underworld Adventures Debugger - a debugger tool for Underworld Adventures
// Copyright (c) 2004,2005,2019 Michael Fink
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

#include "Resource.h"
#include <atlscrl.h>

class CDebugClientInterface;

/// info about a tile on the tilemap
struct CTileMapInfo
{
public:
   /// ctor
   CTileMapInfo() :m_nType(0), m_nFloorHeight(0), m_nCeilingHeight(0), m_nSlope(0),
      m_nTexWall(0), m_nTexFloor(0), m_nTexCeil(0) {}

   unsigned int m_nType;
   unsigned int m_nFloorHeight;
   unsigned int m_nCeilingHeight;
   unsigned int m_nSlope;
   unsigned int m_nTexWall;
   unsigned int m_nTexFloor;
   unsigned int m_nTexCeil;
   unsigned int m_nObjlistStart;
};

/// tilemap view control; draws tilemap
class CTileMapViewCtrl :
   public CScrollWindowImpl<CTileMapViewCtrl, CWindow>,
   public CDebugWindowBase
{
   typedef CScrollWindowImpl<CTileMapViewCtrl, CWindow> baseClass;
public:
   DECLARE_WND_CLASS_EX(_T("TileMapViewCtrl"), CS_DBLCLKS, COLOR_WINDOW);

   /// ctor
   CTileMapViewCtrl();
   /// dtor
   virtual ~CTileMapViewCtrl() {}

   /// returns tile x size
   unsigned int GetTileSizeX() const { return m_nTileSizeX; }
   /// returns tile y size
   unsigned int GetTileSizeY() const { return m_nTileSizeY; }

   /// handles scrolled painting; called by CScrollWindowImpl
   void DoPaint(CDCHandle dc);

protected:
   // virtual methods from CDebugWindowBase
   virtual void ReceiveNotification(CDebugWindowNotification& notify);

   /// initializes scrolling
   void Init();

   /// returns tile map info for a given tile
   CTileMapInfo& GetTileMapInfo(unsigned int x, unsigned int y);

   /// updates tile map values
   void UpdateTileMap();

protected:
   BEGIN_MSG_MAP(CTileMapViewCtrl)
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

protected:
   /// array with all tile map infos
   CAtlArray<CTileMapInfo> m_aMapInfo;

   /// current tile x and y size
   unsigned int m_nTileSizeX, m_nTileSizeY;

   /// selected tile x and y coordinates;
   unsigned int m_nSelectedTileX, m_nSelectedTileY;
};
