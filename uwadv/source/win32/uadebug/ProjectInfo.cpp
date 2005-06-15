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
#include "ProjectManager.hpp"
#include "Resource.h"

// global variables

LPCTSTR g_pszActive = _T(" (active)");

// CProjectInfoWindow methods

void CProjectInfoWindow::ReceiveNotification(CDebugWindowNotification& notify)
{
   switch(notify.code)
   {
   case ncUpdateData:
      UpdateData();
      break;

   case ncCodeDebuggerUpdate:
      RefreshCodeDebuggerList();
      break;
   }
}

void CProjectInfoWindow::UpdateData()
{
   // set project name text at root item
   CProjectManager& projectManager = m_pMainFrame->GetProjectManager();

   CString cszProjectName(_T("No project open."));
   if (projectManager.IsOpen())
      cszProjectName.Format(_T("Project: %s"), projectManager.GetProjectName());

   m_treeCtrl.SetItemText(m_treeCtrl.GetRootItem(), cszProjectName);

   RefreshCodeDebuggerList();
   RefreshLevelList();
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

   // set up fixed tree items
   HTREEITEM hItemProject = m_treeCtrl.InsertItem(_T(""), 0, 0, TVI_ROOT, NULL);
   m_hItemLevels = m_treeCtrl.InsertItem(_T("Level list"), 0, 0, hItemProject, NULL);
   m_hItemCodeDebugger = m_treeCtrl.InsertItem(_T("Sourcecode debugger list"), 0, 0, hItemProject, NULL);

   m_treeCtrl.Expand(hItemProject);

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

LRESULT CProjectInfoWindow::OnSelChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
   if (m_bIgnoreSelections)
      return 0;

   NMTREEVIEW* pNMTreeView = reinterpret_cast<NMTREEVIEW*>(pnmh);

   ATLASSERT((pNMTreeView->itemNew.mask & (TVIF_HANDLE | TVIF_STATE)) == (TVIF_HANDLE | TVIF_STATE));

   HTREEITEM hItem = pNMTreeView->itemNew.hItem;
   if (hItem == NULL || (pNMTreeView->itemNew.state & TVIS_SELECTED) == 0)
      return 0;

   HTREEITEM hItemOld = pNMTreeView->itemOld.hItem;

   STreeItemInfo itemInfo = GetTreeItemInfo(hItem);

   if (itemInfo.m_enType == tiLuaFilename)
   {
      // Lua source file
      CString cszFilename(itemInfo.m_cszInfo);
      if (!cszFilename.IsEmpty())
         m_pMainFrame->OpenLuaSourceFile(cszFilename);
   }
   else
   if (itemInfo.m_enType == tiLevel)
   {
      // changed active Level
      unsigned int nLevel = itemInfo.m_nInfo;

      // set new level
      CDebugClientInterface& debugClient = m_pMainFrame->GetDebugClientInterface();

      debugClient.Lock(true);

      unsigned int nOldLevel = debugClient.GetWorkingLevel();
      debugClient.SetWorkingLevel(nLevel);

      debugClient.Lock(false);

      // refresh list
      if (hItemOld != NULL)
      {
         CString cszText;

         // remove "active" keyword
         m_treeCtrl.GetItemText(hItemOld, cszText);
         int nPos = cszText.Find(g_pszActive);
         if (nPos != 0)
            m_treeCtrl.SetItemText(hItemOld, cszText.Left(nPos));

         m_treeCtrl.GetItemText(hItem, cszText);
         cszText += g_pszActive;
         m_treeCtrl.SetItemText(hItem, cszText);
      }
      else
         RefreshLevelList();

      // send notification when level changed
      if (nLevel != nOldLevel)
      {
         CDebugWindowNotification notify;
         notify.code = ncChangedLevel;
         m_pMainFrame->SendNotification(notify, true, this);
      }
   }

   return 0;
}

LRESULT CProjectInfoWindow::OnDeleteItem(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
   NMTREEVIEW* pNMTreeView = reinterpret_cast<NMTREEVIEW*>(pnmh);

   // delete tree info struct if set
   STreeItemInfo* pInfo = reinterpret_cast<STreeItemInfo*>(pNMTreeView->itemOld.lParam);
   if (pInfo != NULL)
      delete pInfo;

   return 0;
}

void CProjectInfoWindow::RefreshLevelList()
{
   m_treeCtrl.SetRedraw(FALSE);

   m_bIgnoreSelections = true;

   // remove all subitems
   RemoveSubitems(m_hItemLevels);

   CDebugClientInterface& debugClient = m_pMainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   unsigned int nLevel = debugClient.GetWorkingLevel();

   HTREEITEM hSelItem = NULL;

   unsigned int nMax = debugClient.GetNumLevels();
   for (unsigned int n=0; n<nMax; n++)
   {
      CString cszLevelName;
      cszLevelName.Format(_T("Level %u: %s"), n+1, debugClient.GetLevelName(n));

      if (nLevel == n)
         cszLevelName += g_pszActive;

      HTREEITEM hItem = m_treeCtrl.InsertItem(cszLevelName, 2, 2, m_hItemLevels, NULL);
      SetTreeItemInfo(hItem, STreeItemInfo(tiLevel, n));

      if (nLevel == n)
         hSelItem = hItem;
   }

   debugClient.Lock(false);

   m_treeCtrl.Expand(m_hItemLevels);

   m_treeCtrl.SelectItem(hSelItem);
   m_bIgnoreSelections = false;

   m_treeCtrl.SetRedraw(TRUE);
}

void CProjectInfoWindow::RefreshCodeDebuggerList()
{
   m_treeCtrl.SetRedraw(FALSE);

   m_bIgnoreSelections = true;

   RemoveSubitems(m_hItemCodeDebugger);

   CDebugClientInterface& debugClient = m_pMainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   unsigned int nMax = debugClient.GetCodeDebuggerCount();
   for (unsigned int n=0; n<nMax; n++)
   {
      unsigned int nCodeDebuggerID = debugClient.GetCodeDebuggerByIndex(n);
      CDebugClientCodeDebuggerInterface cdi = debugClient.GetCodeDebuggerInterface(nCodeDebuggerID);
      T_enCodeDebuggerType enType = cdi.GetDebuggerType();

      HTREEITEM hItem = m_treeCtrl.InsertItem(
         enType == cdtUwConv ? _T("Conversation code") : _T("Lua script code"),
         0, 0, m_hItemCodeDebugger, NULL);

      SetTreeItemInfo(hItem, STreeItemInfo(tiCodeDebugger, nCodeDebuggerID));

      if (cdi.IsSourceAvail() && cdi.GetSourcecodeCount() > 0)
      {
         HTREEITEM hSubItem = m_treeCtrl.InsertItem(_T("Sourcecode files"), 0, 0, hItem, NULL);
         SetTreeItemInfo(hSubItem, STreeItemInfo(tiWindowType, 0));

         CString cszGameCfgPath(debugClient.GetGameCfgPath());

         unsigned int nMax = cdi.GetSourcecodeCount();
         for (unsigned int n=0; n<nMax; n++)
            InsertLuaSourceFile(hSubItem, cdi.GetSourcecodeFilename(n), cszGameCfgPath);

         m_treeCtrl.Expand(hSubItem);
      }

      HTREEITEM hSubItem = m_treeCtrl.InsertItem(_T("Breakpoints list"), 0, 0, hItem, NULL);
      SetTreeItemInfo(hSubItem, STreeItemInfo(tiWindowType, 1));

      hSubItem = m_treeCtrl.InsertItem(_T("Watches"), 0, 0, hItem, NULL);
      SetTreeItemInfo(hSubItem, STreeItemInfo(tiWindowType, 2));

      if (enType == cdtUwConv)
      {
         hSubItem = m_treeCtrl.InsertItem(_T("Memory"), 0, 0, hItem, NULL);
         SetTreeItemInfo(hSubItem, STreeItemInfo(tiWindowType, 3));
      }

      m_treeCtrl.Expand(hItem);
   }

   debugClient.Lock(false);

   m_treeCtrl.Expand(m_hItemCodeDebugger);

   m_bIgnoreSelections = false;

   m_treeCtrl.SetRedraw(TRUE);
}

void CProjectInfoWindow::InsertLuaSourceFile(HTREEITEM hParentItem, LPCTSTR pszFilename, LPCTSTR pszPathRelativeTo)
{
   CFilename luaFilename(pszFilename);

   // make absolute path
   luaFilename.MakeAbsoluteToCurrentDir();
   ATLASSERT(luaFilename.IsValidObject());

   CString cszFilePath(luaFilename.Get());

   // calculate relative path
   ATLVERIFY(true == luaFilename.MakeRelativeTo(pszPathRelativeTo));

   // insert item
   HTREEITEM hItem = m_treeCtrl.InsertItem(luaFilename.Get(), 1, 1, hParentItem, NULL);
   SetTreeItemInfo(hItem, STreeItemInfo(cszFilePath, tiLuaFilename));
}

CProjectInfoWindow::STreeItemInfo CProjectInfoWindow::GetTreeItemInfo(HTREEITEM hItem)
{
   STreeItemInfo* pInfo = reinterpret_cast<STreeItemInfo*>(m_treeCtrl.GetItemData(hItem));
   if (pInfo != NULL)
      return *pInfo;

   STreeItemInfo itemInfo;
   return itemInfo;
}

//! sets item info for a tree item
void CProjectInfoWindow::SetTreeItemInfo(HTREEITEM hItem, const STreeItemInfo& itemInfo)
{
   STreeItemInfo* pInfo = reinterpret_cast<STreeItemInfo*>(m_treeCtrl.GetItemData(hItem));
   if (pInfo != NULL)
   {
      *pInfo = itemInfo;
   }
   else
   {
      pInfo = new STreeItemInfo;
      *pInfo = itemInfo;
      m_treeCtrl.SetItemData(hItem, reinterpret_cast<DWORD_PTR>(pInfo));
   }
}

void CProjectInfoWindow::RemoveSubitems(HTREEITEM hParentItem)
{
   HTREEITEM hItem = m_treeCtrl.GetNextItem(hParentItem, TVGN_CHILD);
   while (hItem != NULL)
   {
      m_treeCtrl.DeleteItem(hItem);
      hItem = m_treeCtrl.GetNextItem(hParentItem, TVGN_CHILD);
   }
}
