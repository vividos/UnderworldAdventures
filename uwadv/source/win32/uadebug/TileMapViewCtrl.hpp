/*
   Underworld Adventures Debugger - a debugger tool for Underworld Adventures
   Copyright (c) 2004,2005 Michael Fink

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file TileMapViewCtrl.hpp

   \brief tilemap view control

*/
//! \ingroup uadebug

//@{

// include guard
#pragma once

// includes

// forward references
class CDebugClientInterface;

// classes

struct CTileMapInfo
{
public:
   CTileMapInfo():m_nType(0), m_nFloorHeight(0), m_nCeilingHeight(0), m_nSlope(0),
      m_nTexWall(0), m_nTexFloor(0), m_nTexCeil(0){}

   unsigned int m_nType;
   unsigned int m_nFloorHeight;
   unsigned int m_nCeilingHeight;
   unsigned int m_nSlope;
   unsigned int m_nTexWall;
   unsigned int m_nTexFloor;
   unsigned int m_nTexCeil;
   unsigned int m_nObjlistStart;
};

class CTileMapViewCtrl: public CWindowImpl<CTileMapViewCtrl, CWindow>
{
public:
   CTileMapViewCtrl();
   virtual ~CTileMapViewCtrl();

   unsigned int GetTileSizeX(){ return m_nTileSizeX; }
   unsigned int GetTileSizeY(){ return m_nTileSizeY; }

   void UpdateTileMap(CDebugClientInterface* pDebugClient);

   CTileMapInfo& GetTileMapInfo(unsigned int x, unsigned int y);

protected:
   BEGIN_MSG_MAP(CTileMapViewCtrl)
      MESSAGE_HANDLER(WM_PAINT, OnPaint)
      MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
      MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
   END_MSG_MAP()

protected:
   LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
   LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
   CTileMapInfo* m_pMapInfo;

   unsigned int m_nTileSizeX, m_nTileSizeY;
};

//@}
