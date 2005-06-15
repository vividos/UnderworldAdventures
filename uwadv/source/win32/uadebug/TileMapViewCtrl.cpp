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
/*! \file TileMapViewCtrl.cpp

   \brief tilemap view control

*/

// includes
#include "stdatl.hpp"
#include "TileMapViewCtrl.hpp"
#include "DebugClient.hpp"

// CTileMapViewCtrl methods

CTileMapViewCtrl::CTileMapViewCtrl()
{
   m_aMapInfo.SetCount(64*64);
   m_nTileSizeX = m_nTileSizeY = 8;

   m_nSelectedTileX = m_nSelectedTileY = unsigned(-1);
}

void CTileMapViewCtrl::DoPaint(CDCHandle hDC)
{
   CRect rc;
   rc.SetRectEmpty();

   rc.bottom = rc.top + 64*m_nTileSizeX;
   rc.right = rc.left + 64*m_nTileSizeY;

   {
      CMemDC dc(hDC, &rc);

      dc.FillSolidRect(rc.left, rc.top, rc.right, rc.bottom, RGB(128,128,128));

      for(unsigned int y=0; y<64; y++)
      for(unsigned int x=0; x<64; x++)
      {
         CTileMapInfo& info = GetTileMapInfo(x,63-y);

         COLORREF color = RGB(255,255,255);

         if (info.m_nTexFloor == 272 || info.m_nTexFloor == 273 ||
             info.m_nTexFloor == 288 || info.m_nTexFloor == 289 || info.m_nTexFloor == 290 )
            color = RGB(64,64,255);

         if (info.m_nTexFloor == 280 || info.m_nTexFloor == 281)
            color = RGB(255,64,64);

         if (info.m_nType == 0)
            color = RGB(0,0,0);

         switch(info.m_nType)
         {
         case 0: // solid
         case 1: // open
         case 6: // slope n
         case 7: // slope e
         case 8: // slope s
         case 9: // slope w
            dc.FillSolidRect(rc.left+x*m_nTileSizeX, rc.top+y*m_nTileSizeY, m_nTileSizeX, m_nTileSizeY, color);
            break;

         case 2:
         case 3:
         case 4:
         case 5:
            {
               dc.FillSolidRect(rc.left+x*m_nTileSizeX, rc.top+y*m_nTileSizeY, m_nTileSizeX, m_nTileSizeY, RGB(0,0,0));

               POINT points[3];

               switch(info.m_nType)
               {
               case 2: // diagonal se
                  points[0] = CPoint(rc.left+(x+0)*m_nTileSizeX, rc.top+(y+1)*m_nTileSizeY);
                  points[1] = CPoint(rc.left+(x+1)*m_nTileSizeX, rc.top+(y+1)*m_nTileSizeY);
                  points[2] = CPoint(rc.left+(x+1)*m_nTileSizeX, rc.top+(y+0)*m_nTileSizeY);
                  break;
               case 3: // diagonal sw
                  points[0] = CPoint(rc.left+(x+0)*m_nTileSizeX, rc.top+(y+1)*m_nTileSizeY);
                  points[1] = CPoint(rc.left+(x+1)*m_nTileSizeX, rc.top+(y+1)*m_nTileSizeY);
                  points[2] = CPoint(rc.left+(x+0)*m_nTileSizeX, rc.top+(y+0)*m_nTileSizeY);
                  break;
               case 4: // diagonal nw
                  points[0] = CPoint(rc.left+(x+0)*m_nTileSizeX, rc.top+(y+1)*m_nTileSizeY);
                  points[1] = CPoint(rc.left+(x+1)*m_nTileSizeX, rc.top+(y+0)*m_nTileSizeY);
                  points[2] = CPoint(rc.left+(x+0)*m_nTileSizeX, rc.top+(y+0)*m_nTileSizeY);
                  break;
               case 5: // diagonal ne
                  points[0] = CPoint(rc.left+(x+1)*m_nTileSizeX, rc.top+(y+1)*m_nTileSizeY);
                  points[1] = CPoint(rc.left+(x+1)*m_nTileSizeX, rc.top+(y+0)*m_nTileSizeY);
                  points[2] = CPoint(rc.left+(x+0)*m_nTileSizeX, rc.top+(y+0)*m_nTileSizeY);
                  break;
               }

               dc.SetPolyFillMode(WINDING);

               CPen pen;
               pen.CreatePen(PS_NULL, 0, RGB(0,0,0));

               CBrush brush;
               brush.CreateSolidBrush(color);

               HBRUSH hOldBrush = dc.SelectBrush(brush);
               HPEN hOldPen = dc.SelectPen(pen);

               dc.Polygon(points,3);

               dc.SelectBrush(hOldBrush);
               dc.SelectPen(hOldPen);
            }
            break;

         default:
            break;
         }

         // draw objects dots when tile has object list attached
         if (info.m_nObjlistStart != 0)
         {
            COLORREF color = RGB(255,128,0); // orange
            unsigned int nLeftBase = rc.left + x * m_nTileSizeX;
            unsigned int dx = m_nTileSizeX;
            unsigned int nTopBase = rc.top + y * m_nTileSizeY;
            unsigned int dy = m_nTileSizeY;
            unsigned int nBoxSizeX = unsigned(0.3*dy);
            unsigned int nBoxSizeY = unsigned(0.3*dy);
            if (nBoxSizeX < 2) nBoxSizeX = 2;
            if (nBoxSizeY < 2) nBoxSizeY = 2;

            dc.FillSolidRect(nLeftBase+unsigned(0.2*dx), nTopBase+unsigned(0.2*dy), nBoxSizeX, nBoxSizeY, color);
            dc.FillSolidRect(nLeftBase+unsigned(0.7*dx), nTopBase+unsigned(0.3*dy), nBoxSizeX, nBoxSizeY, color);
            dc.FillSolidRect(nLeftBase+unsigned(0.4*dx), nTopBase+unsigned(0.6*dy), nBoxSizeX, nBoxSizeY, color);
         }
      }
   } // <-- CMemDC dtor
}

void CTileMapViewCtrl::ReceiveNotification(CDebugWindowNotification& notify)
{
   switch(notify.code)
   {
   case ncUpdateData:
      UpdateTileMap();
      RedrawWindow(); // force redraw
      break;

   case ncChangedLevel:
      // invalidate selected tile pos
      m_nSelectedTileX = m_nSelectedTileY = unsigned(-1);
      UpdateTileMap();
      RedrawWindow(); // force redraw
      break;
   }
}

void CTileMapViewCtrl::Init()
{
   SetScrollSize(m_nTileSizeX*64, m_nTileSizeY*64);
   SetScrollPage(m_nTileSizeX*16, m_nTileSizeY*16);
   SetScrollLine(m_nTileSizeX*4, m_nTileSizeY*4);

   // resize parent frame window, too
//   GetParent().MoveWindow(0, 0, m_nTileSizeX*64+10, m_nTileSizeY*64+10);
}

CTileMapInfo& CTileMapViewCtrl::GetTileMapInfo(unsigned int x, unsigned int y)
{
   ATLASSERT(x < 64);
   ATLASSERT(y < 64);

   return m_aMapInfo[(y<<6) + x];
}

void CTileMapViewCtrl::UpdateTileMap()
{
   CDebugClientInterface& debugClient = m_pMainFrame->GetDebugClientInterface();

   debugClient.Lock(true);

   // rebuild tile info map
   for(unsigned int y=0; y<64; y++)
   for(unsigned int x=0; x<64; x++)
   {
      CTileMapInfo& info = GetTileMapInfo(x,y);
      info.m_nType =          debugClient.GetTileInfo(x, y, tiType);
      info.m_nFloorHeight =   debugClient.GetTileInfo(x, y, tiFloorHeight);
      info.m_nCeilingHeight = debugClient.GetTileInfo(x, y, tiCeilingHeight);
      info.m_nSlope =         debugClient.GetTileInfo(x, y, tiSlope);
      info.m_nTexWall =       debugClient.GetTileInfo(x, y, tiTextureWall);
      info.m_nTexFloor =      debugClient.GetTileInfo(x, y, tiTextureFloor);
      info.m_nTexCeil =       debugClient.GetTileInfo(x, y, tiTextureCeil);
      info.m_nObjlistStart =  debugClient.GetTileInfo(x, y, tiObjlistStart);
   }

   debugClient.Lock(false);
}

LRESULT CTileMapViewCtrl::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
   CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

   // add offset from scrolling
   CPoint ptOffset;
   GetScrollOffset(ptOffset);

   pt += ptOffset;

   // convert to tile coordinates
   pt.x /= m_nTileSizeX;
   pt.y /= m_nTileSizeY;

   // check range
   if (pt.x < 0 || pt.x >= 64 || pt.y < 0 || pt.y >= 64)
       return 1;

   m_nSelectedTileX = pt.x;
   m_nSelectedTileY = 63-pt.y;

   // send notification about tile selection
   CDebugWindowNotification notify;
   notify.code = ncSelectedTile;
   notify.m_nParam1 = m_nSelectedTileX;
   notify.m_nParam2 = m_nSelectedTileY;

   m_pMainFrame->SendNotification(notify, true, this);

   return 0;
}

LRESULT CTileMapViewCtrl::OnTilemapZoomIn(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (m_nTileSizeX < 16)
   {
      m_nTileSizeX++;
      m_nTileSizeY++;

      Init();
   }
   return 0;
}

LRESULT CTileMapViewCtrl::OnTilemapZoomOut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (m_nTileSizeX > 2)
   {
      m_nTileSizeX--;
      m_nTileSizeY--;

      Init();
   }
   return 0;
}
