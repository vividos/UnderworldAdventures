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
/*! \file TileMapView.cpp

   \brief tilemap view class

*/

// includes
#include "stdatl.hpp"
#include "TileMapView.hpp"
#include "DebugClient.hpp"
#include "MainFrame.hpp"

// methods

CTileMapViewWindow::CTileMapViewWindow()
{
   m_nTileX = m_nTileY = unsigned(-1);
}

CTileMapViewWindow::~CTileMapViewWindow()
{
}

void CTileMapViewWindow::UpdateData()
{
   m_tileMapView.UpdateTileMap(m_pDebugClient);
}

void CTileMapViewWindow::SelectTile(unsigned int x, unsigned y, bool select)
{
   if (select)
   {
      m_nTileX = x;
      m_nTileY = y;
   }
   else
      m_nTileX = m_nTileY = unsigned(-1);
   UpdateTileInfo();
}

void CTileMapViewWindow::UpdateTileInfo()
{
   CString cszText;
   cszText.Format(_T("x: %02x y: %02x"), m_nTileX, m_nTileY);
   SetDlgItemText(IDC_EDIT_TILEPOS, cszText);

   CTileMapInfo& info = m_tileMapView.GetTileMapInfo(m_nTileX, m_nTileY);
   
   m_tileInfoList.LockWindowUpdate();
   m_tileInfoList.DeleteAllItems();

   for(unsigned int i=0; i<7; i++)
   {
      static LPCTSTR apszTileInfoNames[] =
      {
         _T("Type"),
         _T("Floor"),
         _T("Ceiling"),
         _T("Slope"),
         _T("Wall Tex"),
         _T("Floor Tex"),
         _T("Ceil Tex")
      };

      unsigned int nValue = unsigned(-1);
      switch(i)
      {
      case 0: nValue = info.m_nType; break;
      case 1: nValue = info.m_nFloorHeight; break;
      case 2: nValue = info.m_nCeilingHeight; break;
      case 3: nValue = info.m_nSlope; break;
      case 4: nValue = info.m_nTexWall; break;
      case 5: nValue = info.m_nTexFloor; break;
      case 6: nValue = info.m_nTexCeil; break;
      }

      int nItem = m_tileInfoList.InsertItem(m_tileInfoList.GetItemCount(), apszTileInfoNames[i]);

      cszText.Format(_T("%u (%04x)"), nValue, nValue);
/*
      if (i >= 4)
      {
         cszText += _T(" ");
         cszText += m_pDebugClient->GetGameString(10, nValue);
      }
*/
      m_tileInfoList.SetItemText(nItem, 1, cszText);
   }

   m_tileInfoList.LockWindowUpdate(FALSE);

   UpdateObjectInfo();
}

void CTileMapViewWindow::UpdateObjectInfo()
{
   CTileMapInfo& info = m_tileMapView.GetTileMapInfo(m_nTileX, m_nTileY);

   unsigned int nPos = info.m_nObjlistStart;

   m_objectList.LockWindowUpdate();
   m_objectList.DeleteAllItems();

   m_pDebugClient->Lock(true);

   CDebugClientObjectInterface objectInfo = m_pDebugClient->GetObjectInterface();

   while(nPos != 0)
   {
      unsigned int nItemId = objectInfo.GetItemId(nPos);

      CString cszItemName(m_pDebugClient->GetGameString(4, nItemId));

      int nItem = m_objectList.InsertItem(m_objectList.GetItemCount(), cszItemName, nItemId);
      m_objectList.SetItemData(nItem, nItemId);

      nPos = objectInfo.GetItemNext(nPos);
   }

   m_pDebugClient->Lock(false);

   m_objectList.LockWindowUpdate(FALSE);
}

LRESULT CTileMapViewWindow::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   m_tileMapView.SubclassWindow(GetDlgItem(IDC_TILEMAP));
   m_tileMapView.SendMessage(WM_SIZE);

   m_tileInfoList.SubclassWindow(GetDlgItem(IDC_LIST_TILE_PROPERTIES));
   m_tileInfoList.InsertColumn(0, _T("Property"), LVCFMT_LEFT, 90, -1);
   m_tileInfoList.InsertColumn(1, _T("Value"), LVCFMT_LEFT, 100, -1);

   m_objectList.SubclassWindow(GetDlgItem(IDC_LIST_OBJECTS));
   m_objectList.ModifyStyle(0, LVS_SHAREIMAGELISTS);

   m_objectList.InsertColumn(0, _T("Objects"), LVCFMT_LEFT, 190, -1);

   m_objectList.SetImageList(m_pMainFrame->m_ilObjects, LVSIL_NORMAL);
   m_objectList.SetImageList(m_pMainFrame->m_ilObjects, LVSIL_SMALL);

   SetDlgItemText(IDC_EDIT_TILEPOS, _T("info"));

   return 0;
}

LRESULT CTileMapViewWindow::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   POINT pt;
   pt.x = GET_X_LPARAM(lParam);
   pt.y = GET_Y_LPARAM(lParam);

   ClientToScreen(&pt);
   m_tileMapView.ScreenToClient(&pt);

   pt.x /= m_tileMapView.GetTileSizeX();
   pt.y /= m_tileMapView.GetTileSizeY();

   if (pt.x < 0 || pt.x >= 64 || pt.y < 0 || pt.y >= 64)
       return 1;

   SelectTile(pt.x, 63-pt.y);

   return 0;
}

LRESULT CTileMapViewWindow::OnButtonBeam(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
   m_pDebugClient->GetPlayerInterface().Teleport(m_pDebugClient->GetWorkingLevel(),
      m_nTileX+0.5, m_nTileY+0.5);
   return 0;
}
