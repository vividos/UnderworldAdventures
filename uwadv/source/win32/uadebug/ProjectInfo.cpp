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
#include "ProjectManager.hpp"
#include "Resource.h"
#include "BreakpointList.hpp"

// global variables

LPCTSTR g_pszActive = _T(" (active)");

// CProjectInfoWindow methods

void CProjectInfoWindow::ReceiveNotification(CDebugWindowNotification& notify)
{
   switch (notify.m_enCode)
   {
   case ncUpdateData:
      UpdateData();
      break;

   case ncCodeDebuggerUpdate:
      OnCodeDebuggerUpdate(notify);
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

void CProjectInfoWindow::OnCodeDebuggerUpdate(CDebugWindowNotification& notify)
{
   unsigned int nReason = notify.m_nParam1, nCodeDebugger = notify.m_nParam2;

   SCodeDebuggerInfo codeDebuggerInfo;

   switch (nReason)
   {
   case utAttach:
      {
         m_aCodeDebuggerInfos.SetAt(nCodeDebugger, codeDebuggerInfo);
      }
      break;

   case utDetach:
      {
         codeDebuggerInfo = m_aCodeDebuggerInfos[nCodeDebugger];

         if (codeDebuggerInfo.m_pBreakpointWindow != NULL)
         {
            UndockCloseWindow(*codeDebuggerInfo.m_pBreakpointWindow);
            delete codeDebuggerInfo.m_pBreakpointWindow;
         }

         if (codeDebuggerInfo.m_pWatchesWindow != NULL)
         {
            UndockCloseWindow(*codeDebuggerInfo.m_pWatchesWindow);
            delete codeDebuggerInfo.m_pWatchesWindow;
         }

         if (codeDebuggerInfo.m_pCallstackWindow != NULL)
         {
            UndockCloseWindow(*codeDebuggerInfo.m_pCallstackWindow);
            delete codeDebuggerInfo.m_pCallstackWindow;
         }

         m_aCodeDebuggerInfos.RemoveKey(nCodeDebugger);
      }
      break;

   case utUpdateState:
      {
         codeDebuggerInfo = m_aCodeDebuggerInfos[nCodeDebugger];

         if (codeDebuggerInfo.m_pWatchesWindow != NULL)
            m_pMainFrame->SendNotification(notify, codeDebuggerInfo.m_pWatchesWindow);

         if (codeDebuggerInfo.m_pBreakpointWindow != NULL)
            m_pMainFrame->SendNotification(notify, codeDebuggerInfo.m_pBreakpointWindow);

         if (codeDebuggerInfo.m_pCallstackWindow != NULL)
            m_pMainFrame->SendNotification(notify, codeDebuggerInfo.m_pCallstackWindow);
      }
      break;
   }

   RefreshCodeDebuggerList();
}

void CProjectInfoWindow::CreateActivateWindow(CDockingWindowBase& dockingWindow)
{
   // check if window is already created
   if (dockingWindow.m_hWnd == NULL)
   {
      CRect rect(CPoint(0,0), dockingWindow.GetFloatingSize());
      
      DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
      dockingWindow.Create(m_hWnd, rect, dockingWindow.GetDockWindowCaption(), dwStyle);
   }

   // check if window is already docked
   if (!dockingWindow.IsDocking())
      m_pMainFrame->DockDebugWindow(dockingWindow);

   // also window to readonly / writable
   CDebugWindowNotification notify;
   notify.m_enCode = m_pMainFrame->IsGameStopped() ? ncSetReadonly : ncSetReadWrite;
   m_pMainFrame->SendNotification(notify, &dockingWindow);
}

void CProjectInfoWindow::UndockCloseWindow(CDockingWindowBase& dockingWindow)
{
   // determine if docking window is visible
   bool bVisible = dockingWindow.IsWindow() && dockingWindow.IsWindowVisible() &&
       (dockingWindow.IsDocking() || dockingWindow.IsFloating());

   if (bVisible)
   {
      // when docking, undock window, else hide window
      if (dockingWindow.IsDocking())
         dockingWindow.Undock();
      else
         dockingWindow.Hide();
      SetFocus();

// TODO remove      RemoveDebugWindow(&dockingWindow);
   }

   dockingWindow.DestroyWindow();
}

LRESULT CProjectInfoWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   // create and set up tree control
   CRect rcDef;
   GetClientRect(rcDef);
   m_treeCtrl.Create(m_hWnd, rcDef, NULL,
      WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT );

   // load 24-bit bitmap
   {
      CBitmap bmpImages;
      bmpImages.LoadBitmap(IDB_TREE_PROJECT);

      CSize sizeImage;
      bmpImages.GetSize(sizeImage);

      m_ilIcons.Create(16, sizeImage.cy, ILC_COLOR24, 0, sizeImage.cx/16);
      m_ilIcons.Add(bmpImages, RGB(0,0,0));
   }

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
   STreeItemInfo itemInfoOld = GetTreeItemInfo(hItemOld);

   if (itemInfo.m_enType == tiLuaFilename)
   {
      // Lua source file
      CString cszFilename(itemInfo.m_cszInfo);
      if (!cszFilename.IsEmpty())
         m_pMainFrame->OpenLuaSourceFile(cszFilename);
   }
   else
   if (itemInfo.m_enType == tiConvCodeFilename)
   {
      // conversation code source file
      // TODO open file
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
      if (hItemOld != NULL && itemInfoOld.m_enType == tiLevel)
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
         notify.m_enCode = ncChangedLevel;
         m_pMainFrame->SendNotification(notify, true, this);
      }
   }

   return 0;
}

LRESULT CProjectInfoWindow::OnDblClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
{
   HTREEITEM hItem = m_treeCtrl.GetSelectedItem();
   STreeItemInfo itemInfo = GetTreeItemInfo(hItem);

   unsigned int nCodeDebuggerID = itemInfo.m_nCodeDebuggerID;

   if (itemInfo.m_enType == tiWindow)
   {
      ATLASSERT(NULL != m_aCodeDebuggerInfos.Lookup(nCodeDebuggerID));

      SCodeDebuggerInfo& codeDebuggerInfo = m_aCodeDebuggerInfos[nCodeDebuggerID];

      switch (itemInfo.m_nInfo)
      {
      case wtBreakpoints:
         if (codeDebuggerInfo.m_pBreakpointWindow == NULL)
         {
            codeDebuggerInfo.m_pBreakpointWindow = new CBreakpointListWindow(nCodeDebuggerID);
            codeDebuggerInfo.m_pBreakpointWindow->InitDebugWindow(m_pMainFrame);
         }

         CreateActivateWindow(*codeDebuggerInfo.m_pBreakpointWindow);
         break;

      case wtWatches:
//         CreateActivateWindow(*codeDebuggerInfo.m_pWatchesWindow);
         break;

      case wtCallstack:
//         CreateActivateWindow(*codeDebuggerInfo.m_pCallstackWindow);
         break;

      default:
         ATLASSERT(false);
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

      SetTreeItemInfo(hItem, STreeItemInfo(tiCodeDebugger, 0, nCodeDebuggerID));

      if (cdi.IsSourceAvail() && cdi.GetSourcefileCount() > 0)
      {
         HTREEITEM hSubItem = m_treeCtrl.InsertItem(_T("Sourcecode files"), 0, 0, hItem, NULL);
         SetTreeItemInfo(hSubItem, STreeItemInfo(tiWindow, wtNone, nCodeDebuggerID));

         CString cszGameCfgPath(debugClient.GetGameCfgPath());

         unsigned int nMax = cdi.GetSourcefileCount();
         for (unsigned int n=0; n<nMax; n++)
            InsertSourceFile(hSubItem, enType, cdi.GetSourcefileFilename(n), cszGameCfgPath, nCodeDebuggerID);

         m_treeCtrl.Expand(hSubItem);
      }

      HTREEITEM hSubItem = m_treeCtrl.InsertItem(_T("Breakpoints list"), 3, 3, hItem, NULL);
      SetTreeItemInfo(hSubItem, STreeItemInfo(tiWindow, wtBreakpoints, nCodeDebuggerID));

      hSubItem = m_treeCtrl.InsertItem(_T("Watches"), 3, 3, hItem, NULL);
      SetTreeItemInfo(hSubItem, STreeItemInfo(tiWindow, wtWatches, nCodeDebuggerID));

      hSubItem = m_treeCtrl.InsertItem(_T("Callstack"), 3, 3, hItem, NULL);
      SetTreeItemInfo(hSubItem, STreeItemInfo(tiWindow, wtCallstack, nCodeDebuggerID));

/* memory window not supported yet
      if (enType == cdtUwConv)
      {
         hSubItem = m_treeCtrl.InsertItem(_T("Memory"), 3, 3, hItem, NULL);
         SetTreeItemInfo(hSubItem, STreeItemInfo(tiWindow, wtMemory, nCodeDebuggerID));
      }
*/
      m_treeCtrl.Expand(hItem);
   }

   debugClient.Lock(false);

   m_treeCtrl.Expand(m_hItemCodeDebugger);

   m_bIgnoreSelections = false;

   m_treeCtrl.SetRedraw(TRUE);
}

void CProjectInfoWindow::InsertSourceFile(HTREEITEM hParentItem, T_enCodeDebuggerType enType, LPCTSTR pszFilename, LPCTSTR pszPathRelativeTo, unsigned int nCodeDebuggerID)
{
   CFilename luaFilename(pszFilename);
   CString cszFilePath(pszFilename);

   if (enType == cdtLuaScript)
   {
      // make absolute path
      luaFilename.MakeAbsoluteToCurrentDir();
      ATLASSERT(luaFilename.IsValidObject());

      cszFilePath = luaFilename.Get();

      // calculate relative path
      ATLVERIFY(true == luaFilename.MakeRelativeTo(pszPathRelativeTo));
//      luaFilename.MakeRelativeTo(pszPathRelativeTo);
   }
   else if (enType == cdtUwConv)
   {
      // only show filename part
      luaFilename.Set(luaFilename.GetFilename());
   }
   else
      ATLASSERT(false);

   // insert item
   HTREEITEM hItem = m_treeCtrl.InsertItem(luaFilename.Get(), 1, 1, hParentItem, NULL);
   SetTreeItemInfo(hItem, STreeItemInfo(cszFilePath, 0, enType == cdtLuaScript ? tiLuaFilename : tiConvCodeFilename, nCodeDebuggerID));
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
