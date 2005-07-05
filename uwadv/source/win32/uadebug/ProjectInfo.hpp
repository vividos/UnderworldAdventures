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
/*! \file ProjectInfo.hpp

   \brief project info docking window

*/
//! \ingroup uadebug

//@{

// include guard
#pragma once

// includes

// classes

//! player info docking window
class CProjectInfoWindow : public CDockingWindowBase
{
   typedef CProjectInfoWindow thisClass;
   typedef CDockingWindowBase baseClass;

   enum T_enTreeItemType
   {
      tiNone=1, tiLevel, tiLuaFilename, tiCodeDebugger, tiWindowType
   };

   struct STreeItemInfo
   {
      STreeItemInfo(): m_enType(tiNone), m_nInfo(0){}

      STreeItemInfo(T_enTreeItemType enType, unsigned int nInfo): m_enType(enType), m_nInfo(nInfo){}

      STreeItemInfo(LPCTSTR pszInfo, T_enTreeItemType enType): m_enType(enType), m_nInfo(0), m_cszInfo(pszInfo){}

      T_enTreeItemType m_enType;

      CString m_cszInfo;
      unsigned int m_nInfo;
   };

public:
   //! ctor
   CProjectInfoWindow():baseClass(idProjectInfoWindow), m_bIgnoreSelections(false){}
   virtual ~CProjectInfoWindow(){}

   DECLARE_DOCKING_WINDOW(_T("Project"), CSize(250,100)/*docked*/, CSize(250,400)/*floating*/, dockwins::CDockingSide::sLeft)

   DECLARE_WND_CLASS(_T("ProjectInfoWindow"))
   BEGIN_MSG_MAP(thisClass)
      ATLASSERT_ADDED_REFLECT_NOTIFICATIONS()
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
      NOTIFY_CODE_HANDLER(TVN_SELCHANGED, OnSelChanged);
      NOTIFY_CODE_HANDLER(TVN_DELETEITEM, OnDeleteItem);
      CHAIN_MSG_MAP(baseClass)      
   END_MSG_MAP()

protected:
   // message handler
   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnSelChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
   LRESULT OnDeleteItem(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);

   // virtual methods from CDockingWindowBase
   virtual void ReceiveNotification(CDebugWindowNotification& notify);

   //! updates project info data
   void UpdateData();

   //! refreshes level list
   void RefreshLevelList();

   //! refreshes code debugger list
   void RefreshCodeDebuggerList();

   //! inserts lua source file; display name is relative to given path
   void InsertLuaSourceFile(HTREEITEM hParentItem, LPCTSTR pszFilename, LPCTSTR pszPathRelativeTo);

   //! returns tree item info for a given item
   STreeItemInfo GetTreeItemInfo(HTREEITEM hItem);

   //! sets item info for a tree item
   void SetTreeItemInfo(HTREEITEM hItem, const STreeItemInfo& itemInfo);

   //! removes all subitems from a parent item
   void RemoveSubitems(HTREEITEM hParentItem);

protected:
   //! project info tree
   CTreeViewCtrl m_treeCtrl;

   //! tree image list
   CImageList m_ilIcons;

   //! indicates if TVN_SELCHANGED messages should be ignored
   bool m_bIgnoreSelections;

   //! tree item with all level entries
   HTREEITEM m_hItemLevels;

   //! tree item with all code debuggers
   HTREEITEM m_hItemCodeDebugger;
};

//@}
