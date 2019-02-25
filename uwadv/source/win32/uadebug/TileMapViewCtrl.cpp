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
/// \file TileMapViewCtrl.cpp
/// \brief tilemap view control
//
#include "stdatl.hpp"
#include "TileMapViewCtrl.hpp"
#include "DebugClient.hpp"

TileMapViewCtrl::TileMapViewCtrl()
{
   m_tileMapInfos.SetCount(64 * 64);
   m_tileSizeX = m_tileSizeY = 8;

   m_selectedTileX = m_selectedTileY = unsigned(-1);
}

void TileMapViewCtrl::DoPaint(CDCHandle hDC)
{
   CRect rc;
   rc.SetRectEmpty();

   rc.bottom = rc.top + 64 * m_tileSizeX;
   rc.right = rc.left + 64 * m_tileSizeY;

   {
      CMemDC dc(hDC, &rc);

      dc.FillSolidRect(rc.left, rc.top, rc.right, rc.bottom, RGB(128, 128, 128));

      for (unsigned int y = 0; y < 64; y++)
         for (unsigned int x = 0; x < 64; x++)
         {
            TileMapInfo& info = GetTileMapInfo(x, 63 - y);

            COLORREF color = RGB(255, 255, 255);

            if (info.m_textureFloor == 272 || info.m_textureFloor == 273 ||
               info.m_textureFloor == 288 || info.m_textureFloor == 289 || info.m_textureFloor == 290)
               color = RGB(64, 64, 255);

            if (info.m_textureFloor == 280 || info.m_textureFloor == 281)
               color = RGB(255, 64, 64);

            if (info.m_tileType == 0)
               color = RGB(0, 0, 0);

            switch (info.m_tileType)
            {
            case 0: // solid
            case 1: // open
            case 6: // slope n
            case 7: // slope e
            case 8: // slope s
            case 9: // slope w
               dc.FillSolidRect(rc.left + x * m_tileSizeX, rc.top + y * m_tileSizeY, m_tileSizeX, m_tileSizeY, color);
               break;

            case 2:
            case 3:
            case 4:
            case 5:
            {
               dc.FillSolidRect(rc.left + x * m_tileSizeX, rc.top + y * m_tileSizeY, m_tileSizeX, m_tileSizeY, RGB(0, 0, 0));

               POINT points[3];

               switch (info.m_tileType)
               {
               case 2: // diagonal se
                  points[0] = CPoint(rc.left + (x + 0)*m_tileSizeX, rc.top + (y + 1)*m_tileSizeY);
                  points[1] = CPoint(rc.left + (x + 1)*m_tileSizeX, rc.top + (y + 1)*m_tileSizeY);
                  points[2] = CPoint(rc.left + (x + 1)*m_tileSizeX, rc.top + (y + 0)*m_tileSizeY);
                  break;
               case 3: // diagonal sw
                  points[0] = CPoint(rc.left + (x + 0)*m_tileSizeX, rc.top + (y + 1)*m_tileSizeY);
                  points[1] = CPoint(rc.left + (x + 1)*m_tileSizeX, rc.top + (y + 1)*m_tileSizeY);
                  points[2] = CPoint(rc.left + (x + 0)*m_tileSizeX, rc.top + (y + 0)*m_tileSizeY);
                  break;
               case 4: // diagonal nw
                  points[0] = CPoint(rc.left + (x + 0)*m_tileSizeX, rc.top + (y + 1)*m_tileSizeY);
                  points[1] = CPoint(rc.left + (x + 1)*m_tileSizeX, rc.top + (y + 0)*m_tileSizeY);
                  points[2] = CPoint(rc.left + (x + 0)*m_tileSizeX, rc.top + (y + 0)*m_tileSizeY);
                  break;
               case 5: // diagonal ne
                  points[0] = CPoint(rc.left + (x + 1)*m_tileSizeX, rc.top + (y + 1)*m_tileSizeY);
                  points[1] = CPoint(rc.left + (x + 1)*m_tileSizeX, rc.top + (y + 0)*m_tileSizeY);
                  points[2] = CPoint(rc.left + (x + 0)*m_tileSizeX, rc.top + (y + 0)*m_tileSizeY);
                  break;
               }

               dc.SetPolyFillMode(WINDING);

               CPen pen;
               pen.CreatePen(PS_NULL, 0, RGB(0, 0, 0));

               CBrush brush;
               brush.CreateSolidBrush(color);

               HBRUSH hOldBrush = dc.SelectBrush(brush);
               HPEN hOldPen = dc.SelectPen(pen);

               dc.Polygon(points, 3);

               dc.SelectBrush(hOldBrush);
               dc.SelectPen(hOldPen);
            }
            break;

            default:
               break;
            }

            // draw objects dots when tile has object list attached
            if (info.m_nObjectListStart != 0)
            {
               COLORREF colorObject = RGB(255, 128, 0); // orange
               unsigned int nLeftBase = rc.left + x * m_tileSizeX;
               unsigned int dx = m_tileSizeX;
               unsigned int nTopBase = rc.top + y * m_tileSizeY;
               unsigned int dy = m_tileSizeY;
               unsigned int nBoxSizeX = unsigned(0.3*dy);
               unsigned int nBoxSizeY = unsigned(0.3*dy);
               if (nBoxSizeX < 2) nBoxSizeX = 2;
               if (nBoxSizeY < 2) nBoxSizeY = 2;

               dc.FillSolidRect(nLeftBase + unsigned(0.2*dx), nTopBase + unsigned(0.2*dy), nBoxSizeX, nBoxSizeY, colorObject);
               dc.FillSolidRect(nLeftBase + unsigned(0.7*dx), nTopBase + unsigned(0.3*dy), nBoxSizeX, nBoxSizeY, colorObject);
               dc.FillSolidRect(nLeftBase + unsigned(0.4*dx), nTopBase + unsigned(0.6*dy), nBoxSizeX, nBoxSizeY, colorObject);
            }
         }

      // draw border around selected tile
      if (m_selectedTileX != unsigned(-1) && m_selectedTileY != unsigned(-1))
      {
         unsigned int nLineSizeX = unsigned(0.15*m_tileSizeX);
         unsigned int nLineSizeY = unsigned(0.15*m_tileSizeY);
         if (nLineSizeX == 0) nLineSizeX = 0;
         if (nLineSizeY == 0) nLineSizeY = 0;
         COLORREF crBorder = RGB(255, 0, 0);
         unsigned int posX = rc.left + m_selectedTileX * m_tileSizeX;
         unsigned int posY = rc.top + (63 - m_selectedTileY)*m_tileSizeY;

         // top, bottom, left, right
         dc.FillSolidRect(posX, posY, m_tileSizeX, nLineSizeY, crBorder);
         dc.FillSolidRect(posX, posY + m_tileSizeY - nLineSizeY, m_tileSizeX, nLineSizeY, crBorder);
         dc.FillSolidRect(posX, posY, nLineSizeX, m_tileSizeY, crBorder);
         dc.FillSolidRect(posX + m_tileSizeX - nLineSizeX, posY, nLineSizeX, m_tileSizeY, crBorder);
      }

   } // <-- CMemDC dtor
}

void TileMapViewCtrl::ReceiveNotification(DebugWindowNotification& notify)
{
   switch (notify.m_notifyCode)
   {
   case notifyCodeUpdateData:
      UpdateTileMap();
      RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW); // force redraw
      break;

   case notifyCodeChangedLevel:
      // invalidate selected tile pos
      m_selectedTileX = m_selectedTileY = unsigned(-1);
      UpdateTileMap();
      RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW); // force redraw
      break;
   }
}

void TileMapViewCtrl::Init()
{
   SetScrollSize(m_tileSizeX * 64, m_tileSizeY * 64);
   SetScrollPage(m_tileSizeX * 16, m_tileSizeY * 16);
   SetScrollLine(m_tileSizeX * 4, m_tileSizeY * 4);

   // resize parent frame window, too
   //GetParent().MoveWindow(0, 0, m_tileSizeX*64+10, m_tileSizeY*64+10);
}

TileMapInfo& TileMapViewCtrl::GetTileMapInfo(unsigned int x, unsigned int y)
{
   ATLASSERT(x < 64);
   ATLASSERT(y < 64);

   return m_tileMapInfos[(y << 6) + x];
}

void TileMapViewCtrl::UpdateTileMap()
{
   DebugClient& debugClient = m_mainFrame->GetDebugClientInterface();

   debugClient.Lock(true);

   // rebuild tile info map
   for (unsigned int y = 0; y < 64; y++)
      for (unsigned int x = 0; x < 64; x++)
      {
         TileMapInfo& info = GetTileMapInfo(x, y);
         info.m_tileType = debugClient.GetTileInfo(x, y, tiType);
         info.m_floorHeight = debugClient.GetTileInfo(x, y, tiFloorHeight);
         info.m_ceilingHeight = debugClient.GetTileInfo(x, y, tiCeilingHeight);
         info.m_slope = debugClient.GetTileInfo(x, y, tiSlope);
         info.m_textureWall = debugClient.GetTileInfo(x, y, tiTextureWall);
         info.m_textureFloor = debugClient.GetTileInfo(x, y, tiTextureFloor);
         info.m_textureCeiling = debugClient.GetTileInfo(x, y, tiTextureCeil);
         info.m_nObjectListStart = debugClient.GetTileInfo(x, y, tiObjlistStart);
      }

   debugClient.Lock(false);
}

LRESULT TileMapViewCtrl::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
   CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

   // add offset from scrolling
   CPoint offset;
   GetScrollOffset(offset);

   pt += offset;

   // convert to tile coordinates
   pt.x /= m_tileSizeX;
   pt.y /= m_tileSizeY;

   // check range
   if (pt.x < 0 || pt.x >= 64 || pt.y < 0 || pt.y >= 64)
      return 1;

   m_selectedTileX = pt.x;
   m_selectedTileY = 63 - pt.y;

   // send notification about tile selection
   DebugWindowNotification notify;
   notify.m_notifyCode = notifyCodeSelectedTile;
   notify.m_param1 = m_selectedTileX;
   notify.m_param2 = m_selectedTileY;

   m_mainFrame->SendNotification(notify, true, this);

   RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

   return 0;
}

LRESULT TileMapViewCtrl::OnTilemapZoomIn(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (m_tileSizeX < 16)
   {
      m_tileSizeX++;
      m_tileSizeY++;

      Init();
   }
   return 0;
}

LRESULT TileMapViewCtrl::OnTilemapZoomOut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (m_tileSizeX > 2)
   {
      m_tileSizeX--;
      m_tileSizeY--;

      Init();
   }
   return 0;
}
