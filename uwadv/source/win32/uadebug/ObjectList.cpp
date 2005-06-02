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
/*! \file ObjectList.cpp

   \brief object list docking window

*/

// includes
#include "stdatl.hpp"
#include "ObjectList.hpp"
#include "DebugClient.hpp"
#include "MainFrame.hpp"

// CObjectListInfo methods

void CObjectListInfo::Init(unsigned int nColumns)
{
   m_nColumns = nColumns;
   m_pObjectList = new unsigned int[nColumns*0x400];
   ZeroMemory(&m_pObjectList[0], nColumns*0x400);
}

unsigned int CObjectListInfo::GetItem(unsigned int nPos, unsigned int nIndex)
{
   return m_pObjectList[nPos*m_nColumns+nIndex];
}

void CObjectListInfo::SetItem(unsigned int nPos, unsigned int nIndex, unsigned int nValue)
{
   m_pObjectList[nPos*m_nColumns+nIndex] = nValue;
}


// CObjectListWindow methods

CObjectListWindow::CObjectListWindow()
:baseClass(idObjectListWindow), m_bObjlistInfoInited(false)
{
   m_pItemNameList = new CSimpleArray<CString>;
}

CObjectListWindow::~CObjectListWindow()
{
   delete m_pItemNameList;
}

void CObjectListWindow::ReceiveNotification(CDebugWindowNotification& notify)
{
   switch(notify.code)
   {
   case ncUpdateData:
      UpdateData();
      break;

   case ncSelectedObject:
      m_listCtrl.SelectItem(notify.m_nParam1);
      break;

   default:
      break;
   }
}

void CObjectListWindow::UpdateData()
{
   m_pItemNameList->RemoveAll();

   CDebugClientInterface& debugClient = m_pMainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   CDebugClientObjectInterface objectInfo = debugClient.GetObjectInterface();
   unsigned int nColumns = objectInfo.GetColumnCount();

   if (!m_bObjlistInfoInited)
   {
      m_bObjlistInfoInited = true;
      m_objectList.Init(nColumns);
   }

   for(unsigned int pos=0; pos<0x400; pos++)
   {
      for(unsigned int i=0; i<nColumns; i++)
      {
         if (i==0 || i==2) continue; // don't ask for "pos" or "name" field

         unsigned int val = objectInfo.GetItemInfo(pos,i);
         m_objectList.SetItem(pos, i, val);
      }

      // get string for item_id
      unsigned int nItemId = m_objectList.GetItem(pos, 1);
      if (nItemId != 0xFFFF)
         m_pItemNameList->Add(debugClient.GetGameString(4, nItemId));
      else
         m_pItemNameList->Add(_T(""));
   }

   debugClient.Lock(false);

   m_listCtrl.Update(-1);
}

LRESULT CObjectListWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   CRect rcDef;
   GetClientRect(rcDef);
   m_listCtrl.Create(m_hWnd, rcDef, NULL,
      WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS | LVS_SHOWSELALWAYS | LVS_OWNERDATA | LVS_SINGLESEL | LVS_SHAREIMAGELISTS | LVS_NOSORTHEADER);

   m_listCtrl.SetItemCountEx(0x400, LVSICF_NOINVALIDATEALL|LVSICF_NOSCROLL);

   m_listCtrl.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

   m_listCtrl.SetImageList(m_pMainFrame->GetObjectImageList(), LVSIL_SMALL);


   CDebugClientInterface& debugClient = m_pMainFrame->GetDebugClientInterface();
   CDebugClientObjectInterface objectInfo = debugClient.GetObjectInterface();
   unsigned int nColumns = objectInfo.GetColumnCount();

   for(unsigned int n=0; n<nColumns; n++)
      m_listCtrl.InsertColumn(n, objectInfo.GetColumnName(n), LVCFMT_LEFT, objectInfo.GetColumnSize(n), -1);

   m_listCtrl.Init(this);

   // allow editing all columns except column 0 and 2
   m_listCtrl.SetColumnEditable(1, true);

   for(unsigned int i=3; i<nColumns; i++)
      m_listCtrl.SetColumnEditable(i, true);

   return 0;
}

LRESULT CObjectListWindow::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
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

LRESULT CObjectListWindow::OnSetFocus(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
   if (m_listCtrl.m_hWnd != NULL && m_listCtrl.IsWindowVisible())
      m_listCtrl.SetFocus();

   bHandled = FALSE;
   return 1;
}

void CObjectListWindow::OnUpdatedValue(unsigned int nItem, unsigned int nSubItem, LPCTSTR pszText)
{
   ATLASSERT(nSubItem > 0 && nSubItem != 2);
   ATLASSERT(pszText != NULL);

   CDebugClientInterface& debugClient = m_pMainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   CDebugClientObjectInterface objectInfo = debugClient.GetObjectInterface();

   int nBase = objectInfo.ViewColumnAsHex(nSubItem) ? 16 : 10;
   unsigned int nValue = static_cast<unsigned int>(_tcstoul(pszText, NULL, nBase));

   objectInfo.SetItemInfo(nItem, nSubItem, nValue);

   debugClient.Lock(false);

   m_listCtrl.Update(nItem);
}

LRESULT CObjectListWindow::OnGetDispInfo(WPARAM /*wParam*/, NMHDR* pNMHDR, BOOL& /*bHandled*/)
{
   // UpdateData() not called yet?
   if (m_pItemNameList->GetSize() < 0x400)
      return 1;

   NMLVDISPINFO* pDispInfo = (NMLVDISPINFO*)pNMHDR;

   CDebugClientInterface& debugClient = m_pMainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   unsigned int nItem = static_cast<unsigned int>(pDispInfo->item.iItem);
   unsigned int nColumn = static_cast<unsigned int>(pDispInfo->item.iSubItem);
   unsigned int nItemId = m_objectList.GetItem(nItem, 1);

   if (pDispInfo->item.mask & LVIF_TEXT)
   {
      CString cszText;

      switch(nColumn)
      {
      case 0: // pos
         cszText.Format(_T("%04x"), nItem);
         break;
      case 1: // item_id
         if (nItemId != 0xffff)
            cszText.Format(_T("0x%04x"), nItemId);
         else
            cszText = _T("none");
         break;

      case 2: // name
         if (m_pItemNameList != NULL && m_pItemNameList->GetSize() >= 0x400)
            cszText = (*m_pItemNameList)[nItem];
         else
            cszText = _T("???");
         break;

      default:
         if (nItemId != 0xffff)
         {
            CDebugClientObjectInterface objectInfo = debugClient.GetObjectInterface();

            unsigned int nValue = nColumn == 0 ? nItem : m_objectList.GetItem(nItem, nColumn);
            if (objectInfo.ViewColumnAsHex(nColumn))
            {
               CString cszTemp;
               cszTemp.Format(_T("%%0%ux"), objectInfo.ColumnHexDigitCount(nColumn));
               cszText.Format(cszTemp, nValue);
            }
            else
               cszText.Format(_T("%u"), nValue);
         }
         break;
      }

      _tcsncpy(pDispInfo->item.pszText, (LPCTSTR)cszText, pDispInfo->item.cchTextMax-1);
      pDispInfo->item.pszText[pDispInfo->item.cchTextMax-1] = 0;
   }

   if (pDispInfo->item.mask & LVIF_IMAGE)
   {
      // item_id is image number
      pDispInfo->item.iImage = nItemId;
   }

   debugClient.Lock(false);

   return 0;
}

LRESULT CObjectListWindow::OnBeginLabelEdit(WPARAM /*wParam*/, NMHDR* pNMHDR, BOOL& bHandled)
{
   NMLVDISPINFO* pLvDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

   int nItem = pLvDispInfo->item.iItem;
   unsigned int nItemId = m_objectList.GetItem(nItem, 1);

   if (pLvDispInfo->item.iSubItem == 0)
   {
      m_listCtrl.SelectItem(nItem);

      CDebugWindowNotification notify;
      notify.code = ncSelectedObject;
      notify.m_nParam1 = nItem;
      m_pMainFrame->SendNotification(notify, true, this);
   }

   if (nItemId == 0xFFFF)
      return 1;

   bHandled = FALSE;

   return 0;
}
