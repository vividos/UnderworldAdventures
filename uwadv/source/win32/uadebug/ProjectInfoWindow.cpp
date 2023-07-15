//
// Underworld Adventures Debugger - a debugger tool for Underworld Adventures
// Copyright (c) 2004,2005,2019,2023 Underworld Adventures Team
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
#include "resource.h"
#include "BreakpointListWindow.hpp"

LPCTSTR c_activePrefixText = _T(" (active)");

void ProjectInfoWindow::InitTree()
{
   ProjectManager& projectManager = m_mainFrame->GetProjectManager();

   HTREEITEM treeItemProject = m_treeCtrl.InsertItem(_T(""), 0, 0, TVI_ROOT, nullptr);

   CString projectName{ _T("No project open.") };
   if (projectManager.IsOpen())
      projectName.Format(_T("Project: %s"), projectManager.GetProjectName().GetString());

   m_treeCtrl.SetItemText(m_treeCtrl.GetRootItem(), projectName);

   if (projectManager.IsOpen())
   {
      // set up fixed tree items
      m_treeItemLevels = m_treeCtrl.InsertItem(
         _T("Level list"),
         0,
         0,
         treeItemProject, nullptr);

      m_treeItemCodeDebugger = m_treeCtrl.InsertItem(
         _T("Sourcecode debugger list"),
         0,
         0,
         treeItemProject,
         nullptr);

      m_treeCtrl.Expand(treeItemProject);
   }
}

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
   bool hasRootTreeNode = m_treeCtrl.GetRootItem() != nullptr;

   ProjectManager& projectManager = m_mainFrame->GetProjectManager();
   bool wasProjectOpen = m_treeItemLevels != nullptr;

   if (!hasRootTreeNode ||
      wasProjectOpen != projectManager.IsOpen())
      InitTree();

   if (projectManager.IsOpen())
   {
      RefreshCodeDebuggerList();
      RefreshLevelList();
   }
}

void ProjectInfoWindow::OnCodeDebuggerUpdate(DebugWindowNotification& notify)
{
   unsigned int codeDebuggerEvent = notify.m_param1, codeDebuggerId = notify.m_param2;

   CodeDebuggerInfo codeDebuggerInfo;

   switch (codeDebuggerEvent)
   {
   case codeDebuggerAttach:
   {
      m_codeDebuggerInfoMap.SetAt(codeDebuggerId, codeDebuggerInfo);
   }
   break;

   case codeDebuggerDetach:
   {
      codeDebuggerInfo = m_codeDebuggerInfoMap[codeDebuggerId];

      if (codeDebuggerInfo.m_breakpointWindow != nullptr)
      {
         UndockCloseWindow(*codeDebuggerInfo.m_breakpointWindow);
         delete codeDebuggerInfo.m_breakpointWindow;
      }

      if (codeDebuggerInfo.m_watchesWindow != nullptr)
      {
         UndockCloseWindow(*codeDebuggerInfo.m_watchesWindow);
         delete codeDebuggerInfo.m_watchesWindow;
      }

      if (codeDebuggerInfo.m_callStackWindow != nullptr)
      {
         UndockCloseWindow(*codeDebuggerInfo.m_callStackWindow);
         delete codeDebuggerInfo.m_callStackWindow;
      }

      m_codeDebuggerInfoMap.RemoveKey(codeDebuggerId);
   }
   break;

   case codeDebuggerUpdateState:
   {
      codeDebuggerInfo = m_codeDebuggerInfoMap[codeDebuggerId];

      if (codeDebuggerInfo.m_watchesWindow != nullptr)
         m_mainFrame->SendNotification(notify, codeDebuggerInfo.m_watchesWindow);

      if (codeDebuggerInfo.m_breakpointWindow != nullptr)
         m_mainFrame->SendNotification(notify, codeDebuggerInfo.m_breakpointWindow);

      if (codeDebuggerInfo.m_callStackWindow != nullptr)
         m_mainFrame->SendNotification(notify, codeDebuggerInfo.m_callStackWindow);
   }
   break;
   }

   RefreshCodeDebuggerList();
}

void ProjectInfoWindow::CreateActivateWindow(DockingWindowBase& dockingWindow)
{
   // check if window is already created
   if (dockingWindow.m_hWnd == nullptr)
   {
      CRect rect{ CPoint(0, 0), dockingWindow.GetFloatingSize() };

      DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
      dockingWindow.Create(m_hWnd, rect, dockingWindow.GetDockWindowCaption(), dwStyle);
   }

   // check if window is already docked
   if (!dockingWindow.IsDocking())
      m_mainFrame->DockDebugWindow(dockingWindow);

   // also set window to readonly / writable
   DebugWindowNotification notify;
   notify.m_notifyCode = m_mainFrame->IsGameStopped()
      ? notifyCodeSetReadonly
      : notifyCodeSetReadWrite;

   m_mainFrame->SendNotification(notify, &dockingWindow);
}

void ProjectInfoWindow::UndockCloseWindow(DockingWindowBase& dockingWindow)
{
   // determine if docking window is visible
   bool isVisible = dockingWindow.IsWindow() &&
      dockingWindow.IsWindowVisible() &&
      (dockingWindow.IsDocking() || dockingWindow.IsFloating());

   if (isVisible)
   {
      // when docking, undock window, else hide window
      if (dockingWindow.IsDocking())
         dockingWindow.Undock();
      else
         dockingWindow.Hide();
      SetFocus();
   }

   dockingWindow.DestroyWindow();
}

LRESULT ProjectInfoWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   // create and set up tree control
   CRect defaultRect;
   GetClientRect(defaultRect);
   m_treeCtrl.Create(m_hWnd, defaultRect, nullptr,
      WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT);

   // load 24-bit bitmap
   {
      CBitmap images;
      images.LoadBitmap(IDB_TREE_PROJECT);

      CSize sizeImage;
      images.GetSize(sizeImage);

      m_treeIconList.Create(16, sizeImage.cy, ILC_COLOR24, 0, sizeImage.cx / 16);
      m_treeIconList.Add(images, RGB(0, 0, 0));
   }

   m_treeCtrl.SetImageList(m_treeIconList, TVSIL_NORMAL);

   return 0;
}

LRESULT ProjectInfoWindow::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
   if (wParam != SIZE_MINIMIZED)
   {
      // reposition tree control
      RECT rect;
      GetClientRect(&rect);
      m_treeCtrl.SetWindowPos(nullptr, &rect, SWP_NOZORDER | SWP_NOACTIVATE);
   }

   bHandled = FALSE;
   return 1;
}

LRESULT ProjectInfoWindow::OnSetFocus(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
   if (m_treeCtrl.m_hWnd != nullptr
      && m_treeCtrl.IsWindowVisible())
      m_treeCtrl.SetFocus();

   bHandled = FALSE;
   return 1;
}

LRESULT ProjectInfoWindow::OnSelChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
   if (m_ignoreSelections)
      return 0;

   NMTREEVIEW& nmTreeView = *reinterpret_cast<NMTREEVIEW*>(pnmh);

   ATLASSERT((nmTreeView.itemNew.mask & (TVIF_HANDLE | TVIF_STATE)) == (TVIF_HANDLE | TVIF_STATE));

   HTREEITEM treeItem = nmTreeView.itemNew.hItem;
   if (treeItem == nullptr || (nmTreeView.itemNew.state & TVIS_SELECTED) == 0)
      return 0;

   HTREEITEM oldTreeItem = nmTreeView.itemOld.hItem;

   ProjectTreeItemInfo itemInfo = GetTreeItemInfo(treeItem);
   ProjectTreeItemInfo itemInfoOld = GetTreeItemInfo(oldTreeItem);

   if (itemInfo.m_treeItemType == treeItemLuaFile)
   {
      // Lua source file
      CString filename{ itemInfo.m_infoText };
      if (!filename.IsEmpty())
         m_mainFrame->OpenLuaSourceFile(filename);
   }
   else if (itemInfo.m_treeItemType == treeItemConvCodeFile)
   {
      // conversation code source file
      CString filename{ itemInfo.m_infoText };
      if (!filename.IsEmpty())
         m_mainFrame->OpenConvCodeSourceFile(filename);
   }
   else if (itemInfo.m_treeItemType == treeItemLevel)
   {
      // changed active Level
      unsigned int level = itemInfo.m_infoNumber;

      // set new level
      DebugClient& debugClient = m_mainFrame->GetDebugClientInterface();

      debugClient.Lock(true);

      unsigned int oldLevel = debugClient.GetWorkingLevel();
      debugClient.SetWorkingLevel(level);

      debugClient.Lock(false);

      // refresh list
      if (oldTreeItem != nullptr &&
         itemInfoOld.m_treeItemType == treeItemLevel)
      {
         CString text;

         // remove "active" keyword
         m_treeCtrl.GetItemText(oldTreeItem, text);
         int pos = text.Find(c_activePrefixText);
         if (pos != 0)
            m_treeCtrl.SetItemText(oldTreeItem, text.Left(pos));

         m_treeCtrl.GetItemText(treeItem, text);
         text += c_activePrefixText;
         m_treeCtrl.SetItemText(treeItem, text);
      }
      else
         RefreshLevelList();

      // send notification when level changed
      if (level != oldLevel)
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
   HTREEITEM treeItem = m_treeCtrl.GetSelectedItem();
   ProjectTreeItemInfo itemInfo = GetTreeItemInfo(treeItem);

   unsigned int codeDebuggerId = itemInfo.m_codeDebuggerId;

   if (itemInfo.m_treeItemType == treeItemWindow)
   {
      ATLASSERT(nullptr != m_codeDebuggerInfoMap.Lookup(codeDebuggerId));

      CodeDebuggerInfo& codeDebuggerInfo = m_codeDebuggerInfoMap[codeDebuggerId];

      switch (itemInfo.m_infoNumber)
      {
      case wtNone:
         break;

      case wtBreakpoints:
         if (codeDebuggerInfo.m_breakpointWindow == nullptr)
         {
            codeDebuggerInfo.m_breakpointWindow = new BreakpointListWindow(codeDebuggerId);
            codeDebuggerInfo.m_breakpointWindow->InitDebugWindow(m_mainFrame);
         }

         CreateActivateWindow(*codeDebuggerInfo.m_breakpointWindow);
         break;

      case wtWatches:
         //CreateActivateWindow(*codeDebuggerInfo.m_watchesWindow);
         break;

      case wtCallstack:
         //CreateActivateWindow(*codeDebuggerInfo.m_callStackWindow);
         break;

      default:
         ATLASSERT(false);
      }
   }

   return 0;
}

LRESULT ProjectInfoWindow::OnDeleteItem(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
   NMTREEVIEW& nmTreeView = *reinterpret_cast<NMTREEVIEW*>(pnmh);

   // delete tree info struct if set
   ProjectTreeItemInfo* treeItemInfo = reinterpret_cast<ProjectTreeItemInfo*>(nmTreeView.itemOld.lParam);
   if (treeItemInfo != nullptr)
      delete treeItemInfo;

   return 0;
}

void ProjectInfoWindow::RefreshLevelList()
{
   m_treeCtrl.SetRedraw(FALSE);

   m_ignoreSelections = true;

   // remove all subitems
   RemoveSubitems(m_treeItemLevels);

   DebugClient& debugClient = m_mainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   unsigned int workingLevel = debugClient.GetWorkingLevel();

   HTREEITEM selectedTreeItem = nullptr;

   size_t maxLevel = debugClient.GetNumLevels();
   for (unsigned int levelIndex = 0; levelIndex < maxLevel; levelIndex++)
   {
      CString levelName;
      levelName.Format(_T("Level %u: %s"),
         levelIndex + 1,
         debugClient.GetLevelName(levelIndex).GetString());

      if (workingLevel == levelIndex)
         levelName += c_activePrefixText;

      HTREEITEM treeItem = m_treeCtrl.InsertItem(levelName, 2, 2, m_treeItemLevels, nullptr);
      SetTreeItemInfo(treeItem, ProjectTreeItemInfo(treeItemLevel, levelIndex));

      if (workingLevel == levelIndex)
         selectedTreeItem = treeItem;
   }

   debugClient.Lock(false);

   m_treeCtrl.Expand(m_treeItemLevels);

   m_treeCtrl.SelectItem(selectedTreeItem);
   m_ignoreSelections = false;

   m_treeCtrl.SetRedraw(TRUE);
}

void ProjectInfoWindow::RefreshCodeDebuggerList()
{
   if (m_treeItemCodeDebugger == nullptr)
      return; // no project open

   m_treeCtrl.SetRedraw(FALSE);

   m_ignoreSelections = true;

   RemoveSubitems(m_treeItemCodeDebugger);

   DebugClient& debugClient = m_mainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   unsigned int maxIndex = debugClient.GetCodeDebuggerCount();
   for (size_t index = 0; index < maxIndex; index++)
   {
      unsigned int codeDebuggerId = debugClient.GetCodeDebuggerByIndex(index);
      DebugClientCodeDebugger cdi = debugClient.GetCodeDebuggerInterface(codeDebuggerId);
      CodeDebuggerType codeDebuggerType = cdi.GetDebuggerType();

      HTREEITEM treeItem = m_treeCtrl.InsertItem(
         codeDebuggerType == cdtUwConv ? _T("Conversation code") : _T("Lua script code"),
         0, 0, m_treeItemCodeDebugger, nullptr);

      SetTreeItemInfo(treeItem, ProjectTreeItemInfo(treeItemCodeDebugger, 0, codeDebuggerId));

      if (cdi.IsSourceAvail() && cdi.GetSourceFileCount() > 0)
      {
         HTREEITEM subItem = m_treeCtrl.InsertItem(_T("Sourcecode files"), 0, 0, treeItem, nullptr);
         SetTreeItemInfo(subItem, ProjectTreeItemInfo(treeItemWindow, wtNone, codeDebuggerId));

         CString gameCfgPath = debugClient.GetGameConfigPath();

         size_t maxSourceFiles = cdi.GetSourceFileCount();
         for (size_t sourceFileIndex = 0; sourceFileIndex < maxSourceFiles; sourceFileIndex++)
            InsertSourceFile(subItem, codeDebuggerType, cdi.GetSourceFileName(sourceFileIndex), gameCfgPath, codeDebuggerId);

         m_treeCtrl.Expand(subItem);
      }

      HTREEITEM subItem = m_treeCtrl.InsertItem(_T("Breakpoints list"), 3, 3, treeItem, nullptr);
      SetTreeItemInfo(subItem, ProjectTreeItemInfo(treeItemWindow, wtBreakpoints, codeDebuggerId));

      subItem = m_treeCtrl.InsertItem(_T("Watches"), 3, 3, treeItem, nullptr);
      SetTreeItemInfo(subItem, ProjectTreeItemInfo(treeItemWindow, wtWatches, codeDebuggerId));

      subItem = m_treeCtrl.InsertItem(_T("Callstack"), 3, 3, treeItem, nullptr);
      SetTreeItemInfo(subItem, ProjectTreeItemInfo(treeItemWindow, wtCallstack, codeDebuggerId));

      /* memory window not supported yet
            if (codeDebuggerType == cdtUwConv)
            {
               subItem = m_treeCtrl.InsertItem(_T("Memory"), 3, 3, treeItem, nullptr);
               SetTreeItemInfo(subItem, ProjectTreeItemInfo(treeItemWindow, wtMemory, codeDebuggerId));
            }
      */
      m_treeCtrl.Expand(treeItem);
   }

   debugClient.Lock(false);

   m_treeCtrl.Expand(m_treeItemCodeDebugger);

   m_ignoreSelections = false;

   m_treeCtrl.SetRedraw(TRUE);
}

void ProjectInfoWindow::InsertSourceFile(
   HTREEITEM parentTreeItem,
   CodeDebuggerType codeDebuggerType,
   LPCTSTR filename,
   LPCTSTR pathRelativeTo,
   unsigned int codeDebuggerId)
{
   CFilename sourceFilename{ filename };
   CString filePath{ filename };

   if (codeDebuggerType == cdtLuaScript)
   {
      // make absolute path
      sourceFilename.MakeAbsoluteToCurrentDir();
      ATLASSERT(sourceFilename.IsValidObject());

      filePath = sourceFilename.Get();

      // calculate relative path
      ATLVERIFY(true == sourceFilename.MakeRelativeTo(pathRelativeTo));
   }
   else if (codeDebuggerType == cdtUwConv)
   {
      // only show filename part
      sourceFilename.Set(sourceFilename.GetFilename());
   }
   else
      ATLASSERT(false);

   // insert item
   HTREEITEM treeItem = m_treeCtrl.InsertItem(sourceFilename.Get(), 1, 1, parentTreeItem, nullptr);
   SetTreeItemInfo(treeItem,
      ProjectTreeItemInfo(filePath, 0,
         codeDebuggerType == cdtLuaScript ? treeItemLuaFile : treeItemConvCodeFile,
         codeDebuggerId));
}

ProjectInfoWindow::ProjectTreeItemInfo ProjectInfoWindow::GetTreeItemInfo(HTREEITEM treeItem)
{
   ProjectTreeItemInfo* info = reinterpret_cast<ProjectTreeItemInfo*>(m_treeCtrl.GetItemData(treeItem));
   if (info != nullptr)
      return *info;

   ProjectTreeItemInfo itemInfo;
   return itemInfo;
}

/// sets item info for a tree item
void ProjectInfoWindow::SetTreeItemInfo(HTREEITEM treeItem, const ProjectTreeItemInfo& itemInfo)
{
   ProjectTreeItemInfo* info = reinterpret_cast<ProjectTreeItemInfo*>(m_treeCtrl.GetItemData(treeItem));
   if (info != nullptr)
   {
      *info = itemInfo;
   }
   else
   {
      info = new ProjectTreeItemInfo;
      *info = itemInfo;
      m_treeCtrl.SetItemData(treeItem, reinterpret_cast<DWORD_PTR>(info));
   }
}

void ProjectInfoWindow::RemoveSubitems(HTREEITEM parentTreeItem)
{
   HTREEITEM treeItem = m_treeCtrl.GetNextItem(parentTreeItem, TVGN_CHILD);
   while (treeItem != nullptr)
   {
      m_treeCtrl.DeleteItem(treeItem);
      treeItem = m_treeCtrl.GetNextItem(parentTreeItem, TVGN_CHILD);
   }
}
