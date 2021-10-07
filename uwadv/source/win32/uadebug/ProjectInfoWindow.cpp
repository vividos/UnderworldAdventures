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
/// \file ProjectInfoWindow.cpp
/// \brief project info docking window
//
#include "pch.hpp"
#include "ProjectInfoWindow.hpp"
#include "ProjectManager.hpp"
#include "Resource.h"
#include "BreakpointListWindow.hpp"

LPCTSTR g_pszActive = _T(" (active)");

void ProjectInfoWindow::ReceiveNotification(DebugWindowNotification& notify)
{
   switch (notify.m_notifyCode)
   {
   case notifyCodeUpdateData:
      UpdateData();
      break;

   case notifyCodeCodeDebuggerUpdate:
      OnCodeDebuggerUpdate(notify);
      break;
   }
}

void ProjectInfoWindow::UpdateData()
{
   // set project name text at root item
   ProjectManager& projectManager = m_mainFrame->GetProjectManager();

   CString projectName{ _T("No project open.") };
   if (projectManager.IsOpen())
      projectName.Format(_T("Project: %s"), projectManager.GetProjectName().GetString());

   m_treeCtrl.SetItemText(m_treeCtrl.GetRootItem(), projectName);

   RefreshCodeDebuggerList();
   RefreshLevelList();
}

void ProjectInfoWindow::OnCodeDebuggerUpdate(DebugWindowNotification& notify)
{
   unsigned int nReason = notify.m_param1, nCodeDebugger = notify.m_param2;

   CodeDebuggerInfo codeDebuggerInfo;

   switch (nReason)
   {
   case codeDebuggerAttach:
   {
      m_aCodeDebuggerInfos.SetAt(nCodeDebugger, codeDebuggerInfo);
   }
   break;

   case codeDebuggerDetach:
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

   case codeDebuggerUpdateState:
   {
      codeDebuggerInfo = m_aCodeDebuggerInfos[nCodeDebugger];

      if (codeDebuggerInfo.m_pWatchesWindow != NULL)
         m_mainFrame->SendNotification(notify, codeDebuggerInfo.m_pWatchesWindow);

      if (codeDebuggerInfo.m_pBreakpointWindow != NULL)
         m_mainFrame->SendNotification(notify, codeDebuggerInfo.m_pBreakpointWindow);

      if (codeDebuggerInfo.m_pCallstackWindow != NULL)
         m_mainFrame->SendNotification(notify, codeDebuggerInfo.m_pCallstackWindow);
   }
   break;
   }

   RefreshCodeDebuggerList();
}

void ProjectInfoWindow::CreateActivateWindow(DockingWindowBase& dockingWindow)
{
   // check if window is already created
   if (dockingWindow.m_hWnd == NULL)
   {
      CRect rect(CPoint(0, 0), dockingWindow.GetFloatingSize());

      DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
      dockingWindow.Create(m_hWnd, rect, dockingWindow.GetDockWindowCaption(), dwStyle);
   }

   // check if window is already docked
   if (!dockingWindow.IsDocking())
      m_mainFrame->DockDebugWindow(dockingWindow);

   // also window to readonly / writable
   DebugWindowNotification notify;
   notify.m_notifyCode = m_mainFrame->IsGameStopped() ? notifyCodeSetReadonly : notifyCodeSetReadWrite;
   m_mainFrame->SendNotification(notify, &dockingWindow);
}

void ProjectInfoWindow::UndockCloseWindow(DockingWindowBase& dockingWindow)
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

LRESULT ProjectInfoWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   // create and set up tree control
   CRect rcDef;
   GetClientRect(rcDef);
   m_treeCtrl.Create(m_hWnd, rcDef, NULL,
      WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT);

   // load 24-bit bitmap
   {
      CBitmap bmpImages;
      bmpImages.LoadBitmap(IDB_TREE_PROJECT);

      CSize sizeImage;
      bmpImages.GetSize(sizeImage);

      m_ilIcons.Create(16, sizeImage.cy, ILC_COLOR24, 0, sizeImage.cx / 16);
      m_ilIcons.Add(bmpImages, RGB(0, 0, 0));
   }

   m_treeCtrl.SetImageList(m_ilIcons, TVSIL_NORMAL);

   // set up fixed tree items
   HTREEITEM hItemProject = m_treeCtrl.InsertItem(_T(""), 0, 0, TVI_ROOT, NULL);
   m_hItemLevels = m_treeCtrl.InsertItem(_T("Level list"), 0, 0, hItemProject, NULL);
   m_hItemCodeDebugger = m_treeCtrl.InsertItem(_T("Sourcecode debugger list"), 0, 0, hItemProject, NULL);

   m_treeCtrl.Expand(hItemProject);

   return 0;
}

LRESULT ProjectInfoWindow::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
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

LRESULT ProjectInfoWindow::OnSetFocus(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
   if (m_treeCtrl.m_hWnd != NULL && m_treeCtrl.IsWindowVisible())
      m_treeCtrl.SetFocus();

   bHandled = FALSE;
   return 1;
}

LRESULT ProjectInfoWindow::OnSelChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
   if (m_bIgnoreSelections)
      return 0;

   NMTREEVIEW* pNMTreeView = reinterpret_cast<NMTREEVIEW*>(pnmh);

   ATLASSERT((pNMTreeView->itemNew.mask & (TVIF_HANDLE | TVIF_STATE)) == (TVIF_HANDLE | TVIF_STATE));

   HTREEITEM hItem = pNMTreeView->itemNew.hItem;
   if (hItem == NULL || (pNMTreeView->itemNew.state & TVIS_SELECTED) == 0)
      return 0;

   HTREEITEM hItemOld = pNMTreeView->itemOld.hItem;

   ProjectTreeItemInfo itemInfo = GetTreeItemInfo(hItem);
   ProjectTreeItemInfo itemInfoOld = GetTreeItemInfo(hItemOld);

   if (itemInfo.m_enType == tiLuaFilename)
   {
      // Lua source file
      CString filename{ itemInfo.m_cszInfo };
      if (!filename.IsEmpty())
         m_mainFrame->OpenLuaSourceFile(filename);
   }
   else if (itemInfo.m_enType == tiConvCodeFilename)
   {
      // conversation code source file
      CString filename{ itemInfo.m_cszInfo };
      if (!filename.IsEmpty())
         m_mainFrame->OpenConvCodeSourceFile(filename);
   }
   else if (itemInfo.m_enType == tiLevel)
   {
      // changed active Level
      unsigned int nLevel = itemInfo.m_nInfo;

      // set new level
      DebugClient& debugClient = m_mainFrame->GetDebugClientInterface();

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
         int pos = cszText.Find(g_pszActive);
         if (pos != 0)
            m_treeCtrl.SetItemText(hItemOld, cszText.Left(pos));

         m_treeCtrl.GetItemText(hItem, cszText);
         cszText += g_pszActive;
         m_treeCtrl.SetItemText(hItem, cszText);
      }
      else
         RefreshLevelList();

      // send notification when level changed
      if (nLevel != nOldLevel)
      {
         DebugWindowNotification notify;
         notify.m_notifyCode = notifyCodeChangedLevel;
         m_mainFrame->SendNotification(notify, true, this);
      }
   }

   return 0;
}

LRESULT ProjectInfoWindow::OnDblClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
{
   HTREEITEM hItem = m_treeCtrl.GetSelectedItem();
   ProjectTreeItemInfo itemInfo = GetTreeItemInfo(hItem);

   unsigned int codeDebuggerId = itemInfo.m_codeDebuggerId;

   if (itemInfo.m_enType == tiWindow)
   {
      ATLASSERT(NULL != m_aCodeDebuggerInfos.Lookup(codeDebuggerId));

      CodeDebuggerInfo& codeDebuggerInfo = m_aCodeDebuggerInfos[codeDebuggerId];

      switch (itemInfo.m_nInfo)
      {
      case wtNone:
         break;

      case wtBreakpoints:
         if (codeDebuggerInfo.m_pBreakpointWindow == NULL)
         {
            codeDebuggerInfo.m_pBreakpointWindow = new BreakpointListWindow(codeDebuggerId);
            codeDebuggerInfo.m_pBreakpointWindow->InitDebugWindow(m_mainFrame);
         }

         CreateActivateWindow(*codeDebuggerInfo.m_pBreakpointWindow);
         break;

      case wtWatches:
         //CreateActivateWindow(*codeDebuggerInfo.m_pWatchesWindow);
         break;

      case wtCallstack:
         //CreateActivateWindow(*codeDebuggerInfo.m_pCallstackWindow);
         break;

      default:
         ATLASSERT(false);
      }
   }

   return 0;
}

LRESULT ProjectInfoWindow::OnDeleteItem(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
   NMTREEVIEW* pNMTreeView = reinterpret_cast<NMTREEVIEW*>(pnmh);

   // delete tree info struct if set
   ProjectTreeItemInfo* pInfo = reinterpret_cast<ProjectTreeItemInfo*>(pNMTreeView->itemOld.lParam);
   if (pInfo != NULL)
      delete pInfo;

   return 0;
}

void ProjectInfoWindow::RefreshLevelList()
{
   m_treeCtrl.SetRedraw(FALSE);

   m_bIgnoreSelections = true;

   // remove all subitems
   RemoveSubitems(m_hItemLevels);

   DebugClient& debugClient = m_mainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   unsigned int nLevel = debugClient.GetWorkingLevel();

   HTREEITEM hSelItem = NULL;

   unsigned int nMax = debugClient.GetNumLevels();
   for (unsigned int n = 0; n < nMax; n++)
   {
      CString cszLevelName;
      cszLevelName.Format(_T("Level %u: %s"), n + 1, debugClient.GetLevelName(n).GetString());

      if (nLevel == n)
         cszLevelName += g_pszActive;

      HTREEITEM hItem = m_treeCtrl.InsertItem(cszLevelName, 2, 2, m_hItemLevels, NULL);
      SetTreeItemInfo(hItem, ProjectTreeItemInfo(tiLevel, n));

      if (nLevel == n)
         hSelItem = hItem;
   }

   debugClient.Lock(false);

   m_treeCtrl.Expand(m_hItemLevels);

   m_treeCtrl.SelectItem(hSelItem);
   m_bIgnoreSelections = false;

   m_treeCtrl.SetRedraw(TRUE);
}

void ProjectInfoWindow::RefreshCodeDebuggerList()
{
   m_treeCtrl.SetRedraw(FALSE);

   m_bIgnoreSelections = true;

   RemoveSubitems(m_hItemCodeDebugger);

   DebugClient& debugClient = m_mainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   unsigned int maxIndex = debugClient.GetCodeDebuggerCount();
   for (size_t index = 0; index < maxIndex; index++)
   {
      unsigned int codeDebuggerId = debugClient.GetCodeDebuggerByIndex(index);
      DebugClientCodeDebugger cdi = debugClient.GetCodeDebuggerInterface(codeDebuggerId);
      CodeDebuggerType enType = cdi.GetDebuggerType();

      HTREEITEM hItem = m_treeCtrl.InsertItem(
         enType == cdtUwConv ? _T("Conversation code") : _T("Lua script code"),
         0, 0, m_hItemCodeDebugger, NULL);

      SetTreeItemInfo(hItem, ProjectTreeItemInfo(tiCodeDebugger, 0, codeDebuggerId));

      if (cdi.IsSourceAvail() && cdi.GetSourceFileCount() > 0)
      {
         HTREEITEM hSubItem = m_treeCtrl.InsertItem(_T("Sourcecode files"), 0, 0, hItem, NULL);
         SetTreeItemInfo(hSubItem, ProjectTreeItemInfo(tiWindow, wtNone, codeDebuggerId));

         CString cszGameCfgPath(debugClient.GetGameConfigPath());

         size_t maxSourceFiles = cdi.GetSourceFileCount();
         for (size_t sourceFileIndex = 0; sourceFileIndex < maxSourceFiles; sourceFileIndex++)
            InsertSourceFile(hSubItem, enType, cdi.GetSourceFileName(sourceFileIndex), cszGameCfgPath, codeDebuggerId);

         m_treeCtrl.Expand(hSubItem);
      }

      HTREEITEM hSubItem = m_treeCtrl.InsertItem(_T("Breakpoints list"), 3, 3, hItem, NULL);
      SetTreeItemInfo(hSubItem, ProjectTreeItemInfo(tiWindow, wtBreakpoints, codeDebuggerId));

      hSubItem = m_treeCtrl.InsertItem(_T("Watches"), 3, 3, hItem, NULL);
      SetTreeItemInfo(hSubItem, ProjectTreeItemInfo(tiWindow, wtWatches, codeDebuggerId));

      hSubItem = m_treeCtrl.InsertItem(_T("Callstack"), 3, 3, hItem, NULL);
      SetTreeItemInfo(hSubItem, ProjectTreeItemInfo(tiWindow, wtCallstack, codeDebuggerId));

      /* memory window not supported yet
            if (enType == cdtUwConv)
            {
               hSubItem = m_treeCtrl.InsertItem(_T("Memory"), 3, 3, hItem, NULL);
               SetTreeItemInfo(hSubItem, ProjectTreeItemInfo(tiWindow, wtMemory, codeDebuggerId));
            }
      */
      m_treeCtrl.Expand(hItem);
   }

   debugClient.Lock(false);

   m_treeCtrl.Expand(m_hItemCodeDebugger);

   m_bIgnoreSelections = false;

   m_treeCtrl.SetRedraw(TRUE);
}

void ProjectInfoWindow::InsertSourceFile(
   HTREEITEM hParentItem,
   CodeDebuggerType enType,
   LPCTSTR filename,
   LPCTSTR pszPathRelativeTo,
   unsigned int codeDebuggerId)
{
   CFilename sourceFilename{ filename };
   CString filePath{ filename };

   if (enType == cdtLuaScript)
   {
      // make absolute path
      sourceFilename.MakeAbsoluteToCurrentDir();
      ATLASSERT(sourceFilename.IsValidObject());

      filePath = sourceFilename.Get();

      // calculate relative path
      ATLVERIFY(true == sourceFilename.MakeRelativeTo(pszPathRelativeTo));
   }
   else if (enType == cdtUwConv)
   {
      // only show filename part
      sourceFilename.Set(sourceFilename.GetFilename());
   }
   else
      ATLASSERT(false);

   // insert item
   HTREEITEM hItem = m_treeCtrl.InsertItem(sourceFilename.Get(), 1, 1, hParentItem, NULL);
   SetTreeItemInfo(hItem,
      ProjectTreeItemInfo(filePath, 0, enType == cdtLuaScript ? tiLuaFilename : tiConvCodeFilename, codeDebuggerId));
}

ProjectInfoWindow::ProjectTreeItemInfo ProjectInfoWindow::GetTreeItemInfo(HTREEITEM hItem)
{
   ProjectTreeItemInfo* pInfo = reinterpret_cast<ProjectTreeItemInfo*>(m_treeCtrl.GetItemData(hItem));
   if (pInfo != NULL)
      return *pInfo;

   ProjectTreeItemInfo itemInfo;
   return itemInfo;
}

/// sets item info for a tree item
void ProjectInfoWindow::SetTreeItemInfo(HTREEITEM hItem, const ProjectTreeItemInfo& itemInfo)
{
   ProjectTreeItemInfo* pInfo = reinterpret_cast<ProjectTreeItemInfo*>(m_treeCtrl.GetItemData(hItem));
   if (pInfo != NULL)
   {
      *pInfo = itemInfo;
   }
   else
   {
      pInfo = new ProjectTreeItemInfo;
      *pInfo = itemInfo;
      m_treeCtrl.SetItemData(hItem, reinterpret_cast<DWORD_PTR>(pInfo));
   }
}

void ProjectInfoWindow::RemoveSubitems(HTREEITEM hParentItem)
{
   HTREEITEM hItem = m_treeCtrl.GetNextItem(hParentItem, TVGN_CHILD);
   while (hItem != NULL)
   {
      m_treeCtrl.DeleteItem(hItem);
      hItem = m_treeCtrl.GetNextItem(hParentItem, TVGN_CHILD);
   }
}
