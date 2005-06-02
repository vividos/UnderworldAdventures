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
#include "ProjectInfo.hpp"
#include "DebugClient.hpp"
#include "Resource.h"

// CProjectInfoWindow methods

void CProjectInfoWindow::ReceiveNotification(CDebugWindowNotification& notify)
{
   if (notify.code == ncUpdateData)
      UpdateData();
}

void CProjectInfoWindow::UpdateData()
{
   CDebugClientInterface& debugClient = m_pMainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   m_treeCtrl.DeleteAllItems();

   HTREEITEM hLuaTree = m_treeCtrl.InsertItem(_T("Lua source files"), 0, 0, TVI_ROOT, NULL);
   m_treeCtrl.InsertItem(_T("triggers.lua"), 1, 1, hLuaTree, NULL);

   m_treeCtrl.Expand(hLuaTree);

   debugClient.Lock(false);
}

LRESULT CProjectInfoWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   // create and set up tree control
   CRect rcDef;
   GetClientRect(rcDef);
   m_treeCtrl.Create(m_hWnd, rcDef, NULL,
      WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT );

   m_ilIcons.Create(IDB_TREE_PROJECT, 16, 0, RGB(255,255,255));

   m_treeCtrl.SetImageList(m_ilIcons, TVSIL_NORMAL);

   return 0;
}

LRESULT CProjectInfoWindow::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
   if (wParam != SIZE_MINIMIZED)
   {
      // reposition tree control
      RECT rc;
      GetClientRect(&rc);
      m_treeCtrl.SetWindowPos(NULL, &rc, SWP_NOZORDER | SWP_NOACTIVATE);
   }
   bHandled = FALSE;
   return 1;
}

LRESULT CProjectInfoWindow::OnSetFocus(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
   if (m_treeCtrl.m_hWnd != NULL && m_treeCtrl.IsWindowVisible())
      m_treeCtrl.SetFocus();

   bHandled = FALSE;
   return 1;
}
