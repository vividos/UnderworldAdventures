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
/*! \file PlayerInfo.cpp

   \brief player info docking window

*/

// includes
#include "stdatl.hpp"
#include "PlayerInfo.hpp"
#include "DebugClient.hpp"

// CPlayerInfoWindow methods

void CPlayerInfoWindow::UpdateData()
{
   m_pDebugClient->Lock(true);

   CDebugClientPlayerInterface playerInfo = m_pDebugClient->GetPlayerInterface();

   // update all position info values
   _TCHAR szBuffer[32];
   double dVal = 0.0;
   for(unsigned int i=0; i<4; i++)
   {
      dVal = playerInfo.GetPosInfo(i);
      _sntprintf(szBuffer, 32, _T("%3.2f"), dVal);
      m_listCtrl.SetItemText(i, 1, szBuffer);
   }

   // update all attributes
   unsigned int max = playerInfo.GetAttrCount();
   unsigned int iVal = 0;
   for(unsigned int n=0; n<max; n++)
   {
      iVal = playerInfo.GetAttribute(n);
      _sntprintf(szBuffer, 32, _T("%u"), iVal);
      m_listCtrl.SetItemText(n+4, 1, szBuffer);
   }

   m_pDebugClient->Lock(false);
}

LRESULT CPlayerInfoWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   // create and set up list control
   CRect rcDef;
   GetClientRect(rcDef);
   m_listCtrl.Create(m_hWnd, rcDef, NULL,
      WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER );

   m_listCtrl.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

   m_listCtrl.InsertColumn(0, (LPCTSTR)_T("Attribute"), LVCFMT_LEFT, 90, -1);
   m_listCtrl.InsertColumn(1, (LPCTSTR)_T("Value"), LVCFMT_LEFT, 90, -1);

   // insert all text descriptions
   CDebugClientPlayerInterface playerInfo = m_pDebugClient->GetPlayerInterface();

   for(unsigned int n=0; n<4; n++)
      m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), playerInfo.GetPosInfoName(n));

   unsigned int max = playerInfo.GetAttrCount();
   for(unsigned int i=0; i<max; i++)
      m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), playerInfo.GetAttrName(i));

   return 0;
}

LRESULT CPlayerInfoWindow::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
   if (wParam != SIZE_MINIMIZED)
   {
      // reposition list control
      RECT rc;
      GetClientRect(&rc);
      m_listCtrl.SetWindowPos(NULL, &rc, SWP_NOZORDER | SWP_NOACTIVATE);
   }
   bHandled = FALSE;
   return 1;
}

LRESULT CPlayerInfoWindow::OnSetFocus(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
   if (m_listCtrl.m_hWnd != NULL && m_listCtrl.IsWindowVisible())
      m_listCtrl.SetFocus();

   bHandled = FALSE;
   return 1;
}

LRESULT CPlayerInfoWindow::OnBeginLabelEdit(WPARAM /*wParam*/, NMHDR* pNMHDR, BOOL& /*bHandled*/)
{
   NMLVDISPINFO* pLvDispInfo = (NMLVDISPINFO*)pNMHDR;

   // disallow editing column 0
   if (pLvDispInfo->item.iSubItem == 0)
      return 1;

   return 0;
}

LRESULT CPlayerInfoWindow::OnEndLabelEdit(WPARAM /*wParam*/, NMHDR* pNMHDR, BOOL& /*bHandled*/)
{
   NMLVDISPINFO* pLvDispInfo = (NMLVDISPINFO*)pNMHDR;
   if (pLvDispInfo->item.iItem == -1 || pLvDispInfo->item.iSubItem != 1)
      return 0;

   // get edited text
   LPCTSTR pszText = pLvDispInfo->item.pszText;
   int item = pLvDispInfo->item.iItem;

   m_pDebugClient->Lock(true);

   CDebugClientPlayerInterface playerInfo = m_pDebugClient->GetPlayerInterface();

   if (item < 4)
   {
      // position info
      double d = _tcstod(pszText, NULL);
      playerInfo.SetPosInfo(item, d);
   }
   else
   {
      // attribute
      unsigned long nVal = _tcstoul(pszText, NULL, 10);
      playerInfo.SetAttribute(item-4, nVal);
   }

   m_pDebugClient->Lock(false);

   return 1;
}
