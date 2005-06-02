/*
   Underworld Adventures Debugger - a debugger tool for Underworld Adventures
   Copyright (c) 2005 Michael Fink

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
/*! \file TileInfo.cpp

   \brief tile info docking window

*/

// includes
#include "stdatl.hpp"
#include "TileInfo.hpp"
#include "DebugClient.hpp"

// CTileInfoForm methods

CTileInfoForm::CTileInfoForm()
:m_bInited(false)
{
   m_nTileX = m_nTileY = unsigned(-1);
}

LRESULT CTileInfoForm::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   m_tileInfoList.SubclassWindow(GetDlgItem(IDC_LIST_TILE_PROPERTIES));
   m_tileInfoList.InsertColumn(0, _T("Property"), LVCFMT_LEFT, 90, -1);
   m_tileInfoList.InsertColumn(1, _T("Value"), LVCFMT_LEFT, 100, -1);
   m_tileInfoList.Init(/*TODO this*/);

   m_objectList.SubclassWindow(GetDlgItem(IDC_LIST_OBJECTS));
   m_objectList.ModifyStyle(0, LVS_SHAREIMAGELISTS);

   m_objectList.InsertColumn(0, _T("Objects"), LVCFMT_LEFT, 190, -1);
   m_objectList.Init();

   m_objectList.SetImageList(m_pMainFrame->GetObjectImageList(), LVSIL_NORMAL);
   m_objectList.SetImageList(m_pMainFrame->GetObjectImageList(), LVSIL_SMALL);

   SetDlgItemText(IDC_EDIT_TILEPOS, _T("Info"));

   return 0;
}

LRESULT CTileInfoForm::OnButtonBeam(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CDebugClientInterface& debugClient = m_pMainFrame->GetDebugClientInterface();
   debugClient.GetPlayerInterface().Teleport(
      debugClient.GetWorkingLevel(), m_nTileX+0.5, m_nTileY+0.5);

   return 0;
}

void CTileInfoForm::InitDebugWindow(IMainFrame* pMainFrame)
{
   // TODO remove
   CDebugWindowBase::InitDebugWindow(pMainFrame);
}

void CTileInfoForm::ReceiveNotification(CDebugWindowNotification& notify)
{
   switch(notify.code)
   {
   case ncUpdateData:
      UpdateTileInfo();
      break;

   case ncSelectedTile:
      m_nTileX = notify.m_nParam1;
      m_nTileY = notify.m_nParam2;
      UpdateTileInfo();
      break;
   }
}

void CTileInfoForm::UpdateTileInfo()
{
   if (m_nTileX == unsigned(-1) || m_nTileY == unsigned(-1))
   {
      m_tileInfoList.DeleteAllItems();
      m_objectList.DeleteAllItems();
      return;
   }

   CString cszText;
   cszText.Format(_T("x: %02x y: %02x"), m_nTileX, m_nTileY);
   SetDlgItemText(IDC_EDIT_TILEPOS, cszText);

   m_tileInfoList.LockWindowUpdate();
   m_tileInfoList.DeleteAllItems();

   CDebugClientInterface& debugClient = m_pMainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

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
      case 0: nValue = debugClient.GetTileInfo(m_nTileX, m_nTileY, tiType); break;
      case 1: nValue = debugClient.GetTileInfo(m_nTileX, m_nTileY, tiFloorHeight); break;
      case 2: nValue = debugClient.GetTileInfo(m_nTileX, m_nTileY, tiCeilingHeight); break;
      case 3: nValue = debugClient.GetTileInfo(m_nTileX, m_nTileY, tiSlope); break;
      case 4: nValue = debugClient.GetTileInfo(m_nTileX, m_nTileY, tiTextureWall); break;
      case 5: nValue = debugClient.GetTileInfo(m_nTileX, m_nTileY, tiTextureFloor); break;
      case 6: nValue = debugClient.GetTileInfo(m_nTileX, m_nTileY, tiTextureCeil); break;
      }

      int nItem = m_tileInfoList.InsertItem(m_tileInfoList.GetItemCount(), apszTileInfoNames[i]);

      cszText.Format(_T("%u (%04x)"), nValue, nValue);

      if (i >= 4)
      {
         cszText += _T(" ");
         if (nValue < 256)
            cszText += debugClient.GetGameString(10, nValue);
         else
            // formula to calculate string for texture above 0xFF; uw1 specific!
            cszText += debugClient.GetGameString(10, 512-(nValue-256));
      }

      m_tileInfoList.SetItemText(nItem, 1, cszText);
   }

   debugClient.Lock(false);

   m_tileInfoList.LockWindowUpdate(FALSE);

   UpdateObjectInfo();
}

void CTileInfoForm::UpdateObjectInfo()
{
   m_objectList.LockWindowUpdate();
   m_objectList.DeleteAllItems();

   CDebugClientInterface& debugClient = m_pMainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   unsigned int nPos = debugClient.GetTileInfo(m_nTileX, m_nTileY, tiObjlistStart);

   CDebugClientObjectInterface objectInfo = debugClient.GetObjectInterface();

   while (nPos != 0)
   {
      unsigned int nItemId = objectInfo.GetItemId(nPos);

      CString cszItemName(debugClient.GetGameString(4, nItemId));

      int nItem = m_objectList.InsertItem(m_objectList.GetItemCount(), cszItemName, nItemId);
      m_objectList.SetItemData(nItem, nItemId);

      nPos = objectInfo.GetItemNext(nPos);
   }

   debugClient.Lock(false);

   m_objectList.LockWindowUpdate(FALSE);
}


// CTileInfoWindow methods

LRESULT CTileInfoWindow::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
   if (wParam != SIZE_MINIMIZED)
   {
      // reposition tree control
      RECT rc;
      GetClientRect(&rc);
      m_form.SetWindowPos(NULL, &rc, SWP_NOZORDER | SWP_NOACTIVATE);
   }
   bHandled = FALSE;
   return 1;
}

LRESULT CTileInfoWindow::OnSetFocus(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
   if (m_form.m_hWnd != NULL && m_form.IsWindowVisible())
      m_form.SetFocus();

   bHandled = FALSE;
   return 1;
}
