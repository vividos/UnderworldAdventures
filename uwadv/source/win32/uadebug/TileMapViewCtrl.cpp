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

// methods

CTileMapViewCtrl::CTileMapViewCtrl()
{
   m_pMapInfo = new CTileMapInfo[64*64];
   m_nTileSizeX = m_nTileSizeY = 8;
}

CTileMapViewCtrl::~CTileMapViewCtrl()
{
   delete m_pMapInfo;
   m_pMapInfo = NULL;
}

void CTileMapViewCtrl::UpdateTileMap(CDebugClientInterface* pDebugClient)
{
   pDebugClient->Lock(true);

   for(unsigned int y=0; y<64; y++)
   for(unsigned int x=0; x<64; x++)
   {
      CTileMapInfo& info = GetTileMapInfo(x,y);
      info.m_nType =          pDebugClient->GetTileInfo(x,y,0);
      info.m_nFloorHeight =   pDebugClient->GetTileInfo(x,y,1);
      info.m_nCeilingHeight = pDebugClient->GetTileInfo(x,y,2);
      info.m_nSlope =         pDebugClient->GetTileInfo(x,y,3);
      info.m_nTexWall =       pDebugClient->GetTileInfo(x,y,4);
      info.m_nTexFloor =      pDebugClient->GetTileInfo(x,y,5);
      info.m_nTexCeil =       pDebugClient->GetTileInfo(x,y,6);
      info.m_nObjlistStart =  pDebugClient->GetTileInfo(x,y,7);
   }

   pDebugClient->Lock(false);
}

CTileMapInfo& CTileMapViewCtrl::GetTileMapInfo(unsigned int x, unsigned int y)
{
   ATLASSERT(x < 64);
   ATLASSERT(y < 64);

   return m_pMapInfo[(y<<6) + x];
}

LRESULT CTileMapViewCtrl::OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
   PAINTSTRUCT paintStruct;
   HDC hDC = BeginPaint(&paintStruct);

   {
      CRect rc = paintStruct.rcPaint;
      CMemDC dc(hDC, &rc);

      rc.bottom = rc.top + 64*m_nTileSizeX;
      rc.right = rc.left + 64*m_nTileSizeY;

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
      }
   } // <-- CMemDC dtor

   EndPaint(&paintStruct);

   return 0;
}

LRESULT CTileMapViewCtrl::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   return 0;
}

LRESULT CTileMapViewCtrl::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   MINMAXINFO* pMinMaxInfo = (MINMAXINFO*)lParam;

   pMinMaxInfo->ptMaxSize.x = 64*m_nTileSizeX;
   pMinMaxInfo->ptMaxSize.y = 64*m_nTileSizeY;

   return 0;
}

LRESULT CTileMapViewCtrl::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   int nBorderSize = 2;
   SetWindowPos(NULL, 0,0,64*m_nTileSizeX+nBorderSize,64*m_nTileSizeY+nBorderSize,
      SWP_NOMOVE|SWP_NOZORDER|SWP_SHOWWINDOW);

   return 0;
}
