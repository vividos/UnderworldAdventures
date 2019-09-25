//
// Underworld Adventures Debugger - a debugger tool for Underworld Adventures
// Copyright (c) 2005,2019 Underworld Adventures Team
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
/// \file TileInfoWindow.cpp
/// \brief tile info docking window
//
#include "stdatl.hpp"
#include "TileInfoWindow.hpp"
#include "DebugClient.hpp"

TileInfoForm::TileInfoForm()
{
   m_tileX = m_tileY = unsigned(-1);
}

LRESULT TileInfoForm::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);
   DlgResize_Init(false, false);

   m_tileInfoList.InsertColumn(0, _T("Property"), LVCFMT_LEFT, 90, -1);
   m_tileInfoList.InsertColumn(1, _T("Value"), LVCFMT_LEFT, 100, -1);
   m_tileInfoList.Init(/*TODO this*/);

   m_objectList.ModifyStyle(0, LVS_SHAREIMAGELISTS);

   m_objectList.InsertColumn(0, _T("Objects"), LVCFMT_LEFT, 190, -1);
   m_objectList.Init();

   m_objectList.SetImageList(m_mainFrame->GetObjectImageList(), LVSIL_NORMAL);
   m_objectList.SetImageList(m_mainFrame->GetObjectImageList(), LVSIL_SMALL);

   m_tilePos.SetWindowText(_T("Info"));

   return 0;
}

LRESULT TileInfoForm::OnButtonBeam(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   DebugClient& debugClient = m_mainFrame->GetDebugClientInterface();
   debugClient.GetPlayerInterface().Teleport(
      debugClient.GetWorkingLevel(), m_tileX + 0.5, m_tileY + 0.5);

   return 0;
}

LRESULT TileInfoForm::OnListItemChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
   NMLISTVIEW* pNMListView = reinterpret_cast<NMLISTVIEW*>(pnmh);

   // check if item changed to state "focused"
   if (pNMListView->iItem >= 0 &&
      (pNMListView->uOldState & LVIS_FOCUSED) == 0 &&
      (pNMListView->uNewState & LVIS_FOCUSED) != 0)
   {
      unsigned int itemPos = m_objectList.GetItemData(pNMListView->iItem);

      // send notification that an object was clicked
      DebugWindowNotification notify;
      notify.m_notifyCode = notifyCodeSelectedObject;
      notify.m_param1 = itemPos;

      m_mainFrame->SendNotification(notify, true, this);
   }
   return 0;
}

void TileInfoForm::ReceiveNotification(DebugWindowNotification& notify)
{
   switch (notify.m_notifyCode)
   {
   case notifyCodeUpdateData:
      UpdateTileInfo();
      break;

   case notifyCodeSelectedTile:
      m_tileX = notify.m_param1;
      m_tileY = notify.m_param2;
      UpdateTileInfo();
      break;
   }
}

void TileInfoForm::UpdateTileInfo()
{
   if (m_tileX == unsigned(-1) || m_tileY == unsigned(-1))
   {
      m_tileInfoList.DeleteAllItems();
      m_objectList.DeleteAllItems();
      return;
   }

   CString text;
   text.Format(_T("x: %02x y: %02x"), m_tileX, m_tileY);
   m_tilePos.SetWindowText(text);

   m_tileInfoList.SetRedraw(FALSE);
   m_tileInfoList.DeleteAllItems();

   DebugClient& debugClient = m_mainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   for (unsigned int i = 0; i < 7; i++)
   {
      static LPCTSTR tileInfoNames[] =
      {
         _T("Type"),
         _T("Floor"),
         _T("Ceiling"),
         _T("Slope"),
         _T("Wall Tex"),
         _T("Floor Tex"),
         _T("Ceil Tex")
      };

      unsigned int value = unsigned(-1);
      switch (i)
      {
      case 0: value = debugClient.GetTileInfo(m_tileX, m_tileY, tiType); break;
      case 1: value = debugClient.GetTileInfo(m_tileX, m_tileY, tiFloorHeight); break;
      case 2: value = debugClient.GetTileInfo(m_tileX, m_tileY, tiCeilingHeight); break;
      case 3: value = debugClient.GetTileInfo(m_tileX, m_tileY, tiSlope); break;
      case 4: value = debugClient.GetTileInfo(m_tileX, m_tileY, tiTextureWall); break;
      case 5: value = debugClient.GetTileInfo(m_tileX, m_tileY, tiTextureFloor); break;
      case 6: value = debugClient.GetTileInfo(m_tileX, m_tileY, tiTextureCeil); break;
      }

      int itemIndex = m_tileInfoList.InsertItem(m_tileInfoList.GetItemCount(), tileInfoNames[i]);

      text.Format(_T("%u (%04x)"), value, value);

      if (i >= 4)
      {
         text += _T(" ");
         if (value < 256)
            text += debugClient.GetGameString(10, value);
         else
            // formula to calculate string for texture above 0xFF; uw1 specific!
            text += debugClient.GetGameString(10, 512 - (value - 256));
      }

      m_tileInfoList.SetItemText(itemIndex, 1, text);
   }

   debugClient.Lock(false);

   m_tileInfoList.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

   m_tileInfoList.SetRedraw(TRUE);

   UpdateObjectInfo();
}

void TileInfoForm::UpdateObjectInfo()
{
   m_objectList.SetRedraw(FALSE);
   m_objectList.DeleteAllItems();

   DebugClient& debugClient = m_mainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   unsigned int pos = debugClient.GetTileInfo(m_tileX, m_tileY, tiObjlistStart);

   DebugClientObjectListInterface objectInfo = debugClient.GetObjectInterface();

   while (pos != 0)
   {
      unsigned int itemId = objectInfo.GetItemId(pos);

      CString itemName(debugClient.GetGameString(4, itemId));

      int itemIndex = m_objectList.InsertItem(m_objectList.GetItemCount(), itemName, itemId);
      m_objectList.SetItemData(itemIndex, pos);

      pos = objectInfo.GetItemNext(pos);
   }

   m_objectList.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);

   debugClient.Lock(false);

   m_objectList.SetRedraw(TRUE);
}

LRESULT TileInfoWindow::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
   if (wParam != SIZE_MINIMIZED)
   {
      // reposition form control
      RECT rc;
      GetClientRect(&rc);
      m_form.SetWindowPos(NULL, &rc, SWP_NOZORDER | SWP_NOACTIVATE);
   }

   bHandled = FALSE;
   return 1;
}

LRESULT TileInfoWindow::OnSetFocus(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
   if (m_form.m_hWnd != NULL && m_form.IsWindowVisible())
      m_form.SetFocus();

   bHandled = FALSE;
   return 1;
}
