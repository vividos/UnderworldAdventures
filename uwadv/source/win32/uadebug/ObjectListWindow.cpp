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
/// \file ObjectListWindow.cpp
/// \brief object list docking window
//
#include "pch.hpp"
#include "ObjectListWindow.hpp"
#include "DebugClient.hpp"
#include "MainFrame.hpp"

void ObjectListInfo::Init(unsigned int columns)
{
   m_columns = columns;
   m_objectList = new unsigned int[columns * 0x400];
   ZeroMemory(&m_objectList[0], columns * 0x400);
}

unsigned int ObjectListInfo::GetItem(unsigned int pos, unsigned int index)
{
   return m_objectList[pos * m_columns + index];
}

void ObjectListInfo::SetItem(unsigned int pos, unsigned int index, unsigned int value)
{
   m_objectList[pos * m_columns + index] = value;
}

ObjectListWindow::ObjectListWindow()
   :baseClass(idObjectListWindow), m_objectListInfoInited(false)
{
}

ObjectListWindow::~ObjectListWindow()
{
}

void ObjectListWindow::ReceiveNotification(DebugWindowNotification& notify)
{
   switch (notify.m_notifyCode)
   {
   case notifyCodeUpdateData:
      UpdateData();
      break;

   case notifyCodeChangedLevel:
      UpdateData();
      break;

   case notifyCodeSelectedObject:
      m_listCtrl.EnsureVisible(notify.m_param1, FALSE);
      m_listCtrl.SelectItem(notify.m_param1);
      break;

   default:
      break;
   }
}

void ObjectListWindow::UpdateData()
{
   m_itemNameList.RemoveAll();
   m_unallocatedItemListIndices.clear();

   DebugClient& debugClient = m_mainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   DebugClientObjectListInterface objectInfo = debugClient.GetObjectInterface();
   unsigned int columns = objectInfo.GetColumnCount();

   if (!m_objectListInfoInited)
   {
      m_objectListInfoInited = true;
      m_objectList.Init(columns);
   }

   for (unsigned int pos = 0; pos < 0x400; pos++)
   {
      if (!objectInfo.IsItemInfoAvail(pos))
      {
         m_itemNameList.Add(_T("unallocated"));
         m_unallocatedItemListIndices.insert(pos);
         continue;
      }

      for (unsigned int i = 0; i < columns; i++)
      {
         if (i == 0 || i == 2) continue; // don't ask for "pos" or "name" field

         unsigned int val = objectInfo.GetItemInfo(pos, i);
         m_objectList.SetItem(pos, i, val);
      }

      // get string for item_id
      unsigned int itemId = m_objectList.GetItem(pos, 1);
      if (itemId != 0xFFFF)
         m_itemNameList.Add(debugClient.GetGameString(4, itemId));
      else
         m_itemNameList.Add(_T(""));
   }

   debugClient.Lock(false);

   m_listCtrl.Update(-1);
}

LRESULT ObjectListWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   CRect rcDef;
   GetClientRect(rcDef);
   m_listCtrl.Create(m_hWnd, rcDef, NULL,
      WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS | LVS_SHOWSELALWAYS | LVS_OWNERDATA | LVS_SINGLESEL | LVS_SHAREIMAGELISTS | LVS_NOSORTHEADER);

   m_listCtrl.SetItemCountEx(0x400, LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);

   m_listCtrl.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

   m_listCtrl.SetImageList(m_mainFrame->GetObjectImageList(), LVSIL_SMALL);


   DebugClient& debugClient = m_mainFrame->GetDebugClientInterface();
   DebugClientObjectListInterface objectInfo = debugClient.GetObjectInterface();
   unsigned int columns = objectInfo.GetColumnCount();

   for (unsigned int n = 0; n < columns; n++)
      m_listCtrl.InsertColumn(n, objectInfo.GetColumnName(n), LVCFMT_LEFT, objectInfo.GetColumnSize(n), -1);

   m_listCtrl.Init(this);

   // allow editing all columns except column 0 and 2
   m_listCtrl.SetColumnEditable(1, true);

   for (unsigned int i = 3; i < columns; i++)
      m_listCtrl.SetColumnEditable(i, true);

   return 0;
}

LRESULT ObjectListWindow::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
   if (wParam != SIZE_MINIMIZED)
   {
      RECT rc;
      GetClientRect(&rc);
      m_listCtrl.SetWindowPos(NULL, &rc, SWP_NOZORDER | SWP_NOACTIVATE);
   }
   bHandled = FALSE;
   return 1;
}

LRESULT ObjectListWindow::OnSetFocus(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
   if (m_listCtrl.m_hWnd != NULL && m_listCtrl.IsWindowVisible())
      m_listCtrl.SetFocus();

   bHandled = FALSE;
   return 1;
}

void ObjectListWindow::OnUpdatedValue(unsigned int item, unsigned int subItem, LPCTSTR text)
{
   ATLASSERT(subItem > 0 && subItem != 2);
   ATLASSERT(text != NULL);

   if (m_unallocatedItemListIndices.find(item) != m_unallocatedItemListIndices.end())
      return;

   DebugClient& debugClient = m_mainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   DebugClientObjectListInterface objectInfo = debugClient.GetObjectInterface();

   int nBase = objectInfo.ViewColumnAsHex(subItem) ? 16 : 10;
   unsigned int value = static_cast<unsigned int>(_tcstoul(text, NULL, nBase));

   objectInfo.SetItemInfo(item, subItem, value);

   debugClient.Lock(false);

   m_listCtrl.Update(item);
}

LRESULT ObjectListWindow::OnGetDispInfo(WPARAM /*wParam*/, NMHDR* pNMHDR, BOOL& /*bHandled*/)
{
   // UpdateData() not called yet?
   if (m_itemNameList.GetSize() == 0)
      return 1;

   NMLVDISPINFO* pDispInfo = (NMLVDISPINFO*)pNMHDR;

   unsigned int item = static_cast<unsigned int>(pDispInfo->item.iItem);
   unsigned int columnIndex = static_cast<unsigned int>(pDispInfo->item.iSubItem);
   unsigned int itemId = m_objectList.GetItem(item, 1);

   if (m_unallocatedItemListIndices.find(pDispInfo->item.iItem) != m_unallocatedItemListIndices.end())
   {
      pDispInfo->item.mask &= ~LVIF_IMAGE;
      pDispInfo->item.iImage = 0;

      if (columnIndex != 0 && columnIndex != 2)
      {
         pDispInfo->item.pszText = NULL;
         return 0;
      }
   }

   DebugClient& debugClient = m_mainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   if (pDispInfo->item.mask & LVIF_TEXT)
   {
      CString text;

      switch (columnIndex)
      {
      case 0: // pos
         text.Format(_T("%04x"), item);
         break;
      case 1: // item_id
         if (itemId != 0xffff)
            text.Format(_T("0x%04x"), itemId);
         else
            text = _T("none");
         break;

      case 2: // name
         if (item < static_cast<unsigned int>(m_itemNameList.GetSize()))
            text = m_itemNameList[item];
         else
            text = _T("???");
         break;

      default:
         if (itemId != 0xffff)
         {
            DebugClientObjectListInterface objectInfo = debugClient.GetObjectInterface();

            unsigned int value = columnIndex == 0 ? item : m_objectList.GetItem(item, columnIndex);
            if (objectInfo.ViewColumnAsHex(columnIndex))
            {
               CString temp;
               temp.Format(_T("%%0%ux"), objectInfo.ColumnHexDigitCount(columnIndex));
               text.Format(temp, value);
            }
            else
               text.Format(_T("%u"), value);
         }
         break;
      }

      _tcsncpy(pDispInfo->item.pszText, (LPCTSTR)text, pDispInfo->item.cchTextMax - 1);
      pDispInfo->item.pszText[pDispInfo->item.cchTextMax - 1] = 0;
   }

   if (pDispInfo->item.mask & LVIF_IMAGE)
   {
      // item_id is image number
      pDispInfo->item.iImage = itemId;
   }

   debugClient.Lock(false);

   return 0;
}

LRESULT ObjectListWindow::OnBeginLabelEdit(WPARAM /*wParam*/, NMHDR* pNMHDR, BOOL& bHandled)
{
   NMLVDISPINFO* pLvDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

   int nItemPos = pLvDispInfo->item.iItem;
   unsigned int itemId = m_objectList.GetItem(nItemPos, 1);

   if (pLvDispInfo->item.iSubItem == 0)
   {
      m_listCtrl.SelectItem(nItemPos);

      DebugWindowNotification notify;
      notify.m_notifyCode = notifyCodeSelectedObject;
      notify.m_param1 = nItemPos;
      m_mainFrame->SendNotification(notify, true, this);
   }

   if (itemId == 0xFFFF)
      return 1;

   bHandled = FALSE;

   return 0;
}
