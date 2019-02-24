//
// Underworld Adventures Debugger - a debugger tool for Underworld Adventures
// Copyright (c) 2004,2005,2019 Michael Fink
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
/// \file ProjectInfo.hpp
/// \brief project info docking window
//
#pragma once

#include "DebugClient.hpp"

class CDockingWindowBase;

/// window info about code debugger
struct SCodeDebuggerInfo
{
   SCodeDebuggerInfo()
      :m_pWatchesWindow(NULL), m_pBreakpointWindow(NULL), m_pCallstackWindow(NULL) {}

   CDockingWindowBase* m_pBreakpointWindow;
   CDockingWindowBase* m_pWatchesWindow;
   CDockingWindowBase* m_pCallstackWindow;
   //CDockingWindowBase* m_pMemoryWindow;
};

/// player info docking window
class CProjectInfoWindow : public CDockingWindowBase
{
   typedef CProjectInfoWindow thisClass;
   typedef CDockingWindowBase baseClass;

   enum T_enTreeItemType
   {
      tiNone = 1, tiLevel, tiLuaFilename, tiConvCodeFilename, tiCodeDebugger, tiWindow
   };

   enum T_enWindowType
   {
      wtNone = 0,
      wtBreakpoints,
      wtWatches,
      wtCallstack,
      //wtMemory,
   };

   struct STreeItemInfo
   {
      /// default ctor
      STreeItemInfo() : m_enType(tiNone), m_nInfo(0) {}

      /// item info ctor for tiLevel, tiCodeDebugger, tiWindow
      STreeItemInfo(T_enTreeItemType enType, unsigned int nInfo, unsigned int nCodeDebuggerID = 0)
         : m_enType(enType), m_nCodeDebuggerID(nCodeDebuggerID), m_nInfo(nInfo) {}

      /// item info ctor for tiLuaFilename, tiConvCodeFilename
      STreeItemInfo(LPCTSTR pszInfo, unsigned int nInfo = wtNone, T_enTreeItemType enType = tiNone, unsigned int nCodeDebuggerID = 0)
         : m_enType(enType), m_nCodeDebuggerID(nCodeDebuggerID), m_nInfo(nInfo), m_cszInfo(pszInfo) {}

      /// type of tree item
      T_enTreeItemType m_enType;

      /// code debugger id, if any
      unsigned int m_nCodeDebuggerID;

      /// string info; for filename items
      CString m_cszInfo;

      /// integer info; for window items
      unsigned int m_nInfo;
   };

public:
   /// ctor
   CProjectInfoWindow() :baseClass(idProjectInfoWindow), m_bIgnoreSelections(false) {}
   virtual ~CProjectInfoWindow() {}

   DECLARE_DOCKING_WINDOW(_T("Project"), CSize(250, 100)/*docked*/, CSize(250, 400)/*floating*/, dockwins::CDockingSide::sLeft)

   DECLARE_WND_CLASS(_T("ProjectInfoWindow"))
   BEGIN_MSG_MAP(thisClass)
      ATLASSERT_ADDED_REFLECT_NOTIFICATIONS()
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
      NOTIFY_CODE_HANDLER(TVN_SELCHANGED, OnSelChanged);
   NOTIFY_CODE_HANDLER(NM_DBLCLK, OnDblClick);
   NOTIFY_CODE_HANDLER(TVN_DELETEITEM, OnDeleteItem);
   CHAIN_MSG_MAP(baseClass)
   END_MSG_MAP()

protected:
   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnSelChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
   LRESULT OnDblClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
   LRESULT OnDeleteItem(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);

   // virtual methods from CDockingWindowBase
   virtual void ReceiveNotification(CDebugWindowNotification& notify) override;

   /// updates project info data
   void UpdateData();

   /// handles code debugger update message
   void OnCodeDebuggerUpdate(CDebugWindowNotification& notify);

   /// creates and docks or activates code debugger window
   void CreateActivateWindow(CDockingWindowBase& dockingWindow);

   /// undocks and closes a code debugger window
   void UndockCloseWindow(CDockingWindowBase& dockingWindow);

   /// refreshes level list
   void RefreshLevelList();

   /// refreshes code debugger list
   void RefreshCodeDebuggerList();

   /// inserts source file; display name is relative to given path
   void InsertSourceFile(HTREEITEM hParentItem, T_enCodeDebuggerType enType,
      LPCTSTR pszFilename, LPCTSTR pszPathRelativeTo, unsigned int nCodeDebuggerID);

   /// returns tree item info for a given item
   STreeItemInfo GetTreeItemInfo(HTREEITEM hItem);

   /// sets item info for a tree item
   void SetTreeItemInfo(HTREEITEM hItem, const STreeItemInfo& itemInfo);

   /// removes all subitems from a parent item
   void RemoveSubitems(HTREEITEM hParentItem);

protected:
   /// project info tree
   CTreeViewCtrl m_treeCtrl;

   /// tree image list
   CImageList m_ilIcons;

   /// indicates if TVN_SELCHANGED messages should be ignored
   bool m_bIgnoreSelections;

   /// tree item with all level entries
   HTREEITEM m_hItemLevels;

   /// tree item with all code debuggers
   HTREEITEM m_hItemCodeDebugger;

   /// map with all code debugger infos
   CAtlMap<unsigned int, SCodeDebuggerInfo> m_aCodeDebuggerInfos;
};
