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
/// \file ProjectInfoWindow.hpp
/// \brief project info docking window
//
#pragma once

#include "DebugClient.hpp"

class DockingWindowBase;

/// window info about code debugger
struct CodeDebuggerInfo
{
   CodeDebuggerInfo() {}

   DockingWindowBase* m_breakpointWindow = nullptr;
   DockingWindowBase* m_watchesWindow = nullptr;
   DockingWindowBase* m_callStackWindow = nullptr;
   //DockingWindowBase* m_memoryWindow = nullptr;
};

/// player info docking window
class ProjectInfoWindow : public DockingWindowBase
{
   /// this class type
   typedef ProjectInfoWindow ThisClass;

   /// base class type
   typedef DockingWindowBase BaseClass;

   enum ProjectTreeItemType
   {
      treeItemNone = 1,
      treeItemLevel,
      treeItemLuaFile,
      treeItemConvCodeFile,
      treeItemCodeDebugger,
      treeItemWindow
   };

   enum T_enWindowType
   {
      wtNone = 0,
      wtBreakpoints,
      wtWatches,
      wtCallstack,
      //wtMemory,
   };

   struct ProjectTreeItemInfo
   {
      /// default ctor
      ProjectTreeItemInfo()
      {
      }

      /// item info ctor for treeItemLevel, treeItemCodeDebugger, treeItemWindow
      ProjectTreeItemInfo(ProjectTreeItemType treeItemType,
         unsigned int infoNumber,
         unsigned int codeDebuggerId = 0)
         : m_treeItemType(treeItemType),
         m_codeDebuggerId(codeDebuggerId),
         m_infoNumber(infoNumber)
      {
      }

      /// item info ctor for treeItemLuaFile, treeItemConvCodeFile
      ProjectTreeItemInfo(LPCTSTR infoText,
         unsigned int nInfo = wtNone,
         ProjectTreeItemType treeItemType = treeItemNone,
         unsigned int codeDebuggerId = 0)
         : m_treeItemType(treeItemType),
         m_codeDebuggerId(codeDebuggerId),
         m_infoNumber(nInfo),
         m_infoText(infoText)
      {
      }

      /// type of tree item
      ProjectTreeItemType m_treeItemType = treeItemNone;

      /// code debugger id, if any
      unsigned int m_codeDebuggerId = 0;

      /// string info; for filename items
      CString m_infoText;

      /// integer info; for window items
      unsigned int m_infoNumber = 0;
   };

public:
   /// ctor
   ProjectInfoWindow()
      :BaseClass(idProjectInfoWindow)
   {
   }

   /// dtor
   virtual ~ProjectInfoWindow() {}

   DECLARE_DOCKING_WINDOW(_T("Project"), CSize(250, 100)/*docked*/, CSize(250, 400)/*floating*/, dockwins::CDockingSide::sLeft);

   DECLARE_WND_CLASS(_T("ProjectInfoWindow"))

   BEGIN_MSG_MAP(thisClass)
      ATLASSERT_ADDED_REFLECT_NOTIFICATIONS()
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
      NOTIFY_CODE_HANDLER(TVN_SELCHANGED, OnSelChanged)
      NOTIFY_CODE_HANDLER(NM_DBLCLK, OnDblClick)
      NOTIFY_CODE_HANDLER(TVN_DELETEITEM, OnDeleteItem)
      CHAIN_MSG_MAP(BaseClass)
   END_MSG_MAP()

private:
   LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnSelChanged(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
   LRESULT OnDblClick(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
   LRESULT OnDeleteItem(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

   // virtual methods from DockingWindowBase
   virtual void ReceiveNotification(DebugWindowNotification& notify) override;

   /// initializes tree
   void InitTree();

   /// updates project info data
   void UpdateData();

   /// handles code debugger update message
   void OnCodeDebuggerUpdate(DebugWindowNotification& notify);

   /// creates and docks or activates code debugger window
   void CreateActivateWindow(DockingWindowBase& dockingWindow);

   /// undocks and closes a code debugger window
   void UndockCloseWindow(DockingWindowBase& dockingWindow);

   /// refreshes level list
   void RefreshLevelList();

   /// refreshes code debugger list
   void RefreshCodeDebuggerList();

   /// inserts source file; display name is relative to given path
   void InsertSourceFile(HTREEITEM parentTreeItem, CodeDebuggerType codeDebuggerType,
      LPCTSTR filename, LPCTSTR pathRelativeTo, unsigned int codeDebuggerId);

   /// returns tree item info for a given item
   ProjectTreeItemInfo GetTreeItemInfo(HTREEITEM treeItem);

   /// sets item info for a tree item
   void SetTreeItemInfo(HTREEITEM treeItem, const ProjectTreeItemInfo& itemInfo);

   /// removes all subitems from a parent item
   void RemoveSubitems(HTREEITEM parentTreeItem);

private:
   /// project info tree
   CTreeViewCtrl m_treeCtrl;

   /// tree image list
   CImageList m_treeIconList;

   /// indicates if TVN_SELCHANGED messages should be ignored
   bool m_ignoreSelections = false;

   /// tree item with all level entries
   HTREEITEM m_treeItemLevels = nullptr;

   /// tree item with all code debuggers
   HTREEITEM m_treeItemCodeDebugger = nullptr;

   /// map with all code debugger infos
   CAtlMap<unsigned int, CodeDebuggerInfo> m_codeDebuggerInfoMap;
};
