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

// data

// pos and name don't count as columns in CDebugClient
const int g_nColumns = 15-2;

struct ObjectListColumnInfo
{
   LPCTSTR pszColumnName;
   int nColumnSize;
   bool bFormatHex;
   unsigned nFormatHexLength;
} g_aColumnInfo[] =
{
   { _T("pos"),      55,   true,    4 },
   { _T("item_id"),  55,   true,    4 },
   { _T("name"),     150,  false,   0 },
   { _T("link"),     55,   true,    4 },
   { _T("quality"),  50,   true,    3 },
   { _T("owner"),    50,   true,    2 },
   { _T("quantity"), 50,   true,    3 },
   { _T("x"),        35,   false,   1 },
   { _T("y"),        35,   false,   1 },
   { _T("z"),        35,   false,   1 },
   { _T("heading"),  50,   false,   1 },
   { _T("flags"),    40,   true,    1 },
   { _T("ench"),     40,   true,    1 },
   { _T("is_quant"), 55,   false,   1 },
   { _T("hidden"),   55,    false,   1 },
};


// CObjectListWindow methods

CObjectListWindow::CObjectListWindow()
:baseClass(idObjectListWindow)
{
   m_pObjectList = new unsigned int[0x400*g_nColumns];
   ZeroMemory(m_pObjectList, 0x400*g_nColumns);

   m_pItemNameList = new CSimpleArray<CString>;
}

CObjectListWindow::~CObjectListWindow()
{
   delete[] m_pObjectList;
   delete m_pItemNameList;
}

void CObjectListWindow::UpdateData()
{
   ZeroMemory(m_pObjectList, 0x400*g_nColumns);
   m_pItemNameList->RemoveAll();

   m_pDebugClient->Lock(true);

   for(unsigned int pos=0; pos<0x400; pos++)
   {
      for(unsigned int i=0; i<g_nColumns; i++)
      {
         unsigned int val = m_pDebugClient->GetObjectListInfo(pos,i);
         m_pObjectList[pos*g_nColumns+i] = val;
      }

      // get string for item_id
      unsigned int item_id = m_pObjectList[pos*g_nColumns];
      m_pItemNameList->Add(m_pDebugClient->GetGameString(4, item_id));
   }

   m_pDebugClient->Lock(false);

   m_listCtrl.Update(-1);
}

LRESULT CObjectListWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   CRect rcDef;
   GetClientRect(rcDef);
   m_listCtrl.Create(m_hWnd, rcDef, NULL,
      WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS | LVS_SHOWSELALWAYS | LVS_OWNERDATA | LVS_SINGLESEL);

   m_listCtrl.SetItemCountEx(0x400, LVSICF_NOINVALIDATEALL|LVSICF_NOSCROLL);

   m_listCtrl.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

   for(unsigned int n=0; n<sizeof(g_aColumnInfo)/sizeof(g_aColumnInfo[0]); n++)
      m_listCtrl.InsertColumn(n, g_aColumnInfo[n].pszColumnName, LVCFMT_LEFT, g_aColumnInfo[n].nColumnSize, -1);

   return 0;
}

LRESULT CObjectListWindow::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
   if(wParam != SIZE_MINIMIZED )
   {
      RECT rc;
      GetClientRect(&rc);
      m_listCtrl.SetWindowPos(NULL, &rc ,SWP_NOZORDER | SWP_NOACTIVATE );
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

LRESULT CObjectListWindow::OnBeginLabelEdit(WPARAM /*wParam*/, NMHDR* pNMHDR, BOOL& /*bHandled*/)
{
   NMLVDISPINFO* pLvDispInfo = (NMLVDISPINFO*)pNMHDR;

   // cannot edit pos and name
   if (pLvDispInfo->item.iSubItem == 0)
   {
      m_listCtrl.SelectItem(pLvDispInfo->item.iItem);
      return 1;
   }
   if (pLvDispInfo->item.iSubItem == 2)
   {
      return 1;
   }

   return 0;
}

LRESULT CObjectListWindow::OnEndLabelEdit(WPARAM /*wParam*/, NMHDR* pNMHDR, BOOL& /*bHandled*/)
{
   NMLVDISPINFO* pLvDispInfo = (NMLVDISPINFO*)pNMHDR;
   if (pLvDispInfo->item.iItem == -1 || pLvDispInfo->item.iSubItem != 1)
      return 0;
/*
   LPCTSTR pszText = pLvDispInfo->item.pszText;
   int item = pLvDispInfo->item.iItem;

   m_pDebugClient->Lock(true);

   if (item<4)
   {
      double d = _tcstod(pszText, NULL);
      m_pDebugClient->SetPlayerPosInfo(item, d);
   }
   else
   {
      unsigned long nVal = _tcstoul(pszText, NULL, 10);
      m_pDebugClient->SetPlayerAttribute(item-4, nVal);
   }

   m_pDebugClient->Lock(false);
*/
   return 1;
}

LRESULT CObjectListWindow::OnGetDispInfo(WPARAM /*wParam*/, NMHDR* pNMHDR, BOOL& /*bHandled*/)
{
   // UpdateData() not called yet?
   if (m_pItemNameList->GetSize() < 0x400)
      return 1;

   NMLVDISPINFO* pDispInfo = (NMLVDISPINFO*)pNMHDR;

   if (pDispInfo->item.mask & LVIF_TEXT)
   {
      CString cszText;
      unsigned int pos = pDispInfo->item.iItem;
      unsigned int item_id = m_pObjectList[pos*g_nColumns+0];

      switch(pDispInfo->item.iSubItem)
      {
      case 0: // pos
         cszText.Format(_T("0x%04x"), pos);
         break;

      case 1: // item_id
         if (item_id != 0xffff)
            cszText.Format(_T("0x%04x"), item_id);
         else
            cszText = _T("none");
         break;

      case 2: // name
         if (m_pItemNameList != NULL && m_pItemNameList->GetSize() >= 0x400)
            cszText = (*m_pItemNameList)[pos];
         else
            cszText = _T("???");
         break;

      default:
         if (item_id != 0xffff)
         {
            unsigned int nIndex = pDispInfo->item.iSubItem-2;
            unsigned int nValue = m_pObjectList[pos*g_nColumns+nIndex];
            if (g_aColumnInfo[pDispInfo->item.iSubItem].bFormatHex)
            {
               CString cszTemp;
               cszTemp.Format(_T("%%0%ux"), g_aColumnInfo[pDispInfo->item.iSubItem].nFormatHexLength);
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
   return 0;
}
