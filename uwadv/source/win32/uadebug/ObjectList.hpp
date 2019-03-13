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
/// \file ObjectList.hpp
/// \brief object list docking window
//
#pragma once

#include <set>

class ObjectListInfo
{
public:
   ObjectListInfo() :m_objectList(NULL), m_columns(0) {}
   ~ObjectListInfo() { delete m_objectList; }

   void Init(unsigned int nColumns);
   unsigned int GetItem(unsigned int pos, unsigned int index);
   void SetItem(unsigned int pos, unsigned int index, unsigned int value);

protected:
   unsigned int m_columns;
   unsigned int* m_objectList;
};

class ObjectListWindow : public DockingWindowBase,
   public IEditListViewCallback
{
   typedef ObjectListWindow thisClass;
   typedef DockingWindowBase baseClass;
public:
   ObjectListWindow();
   virtual ~ObjectListWindow();

   DECLARE_DOCKING_WINDOW(_T("Object List"), CSize(200, 100)/*docked*/, CSize(500, 150)/*floating*/, dockwins::CDockingSide::sBottom)

   DECLARE_WND_CLASS_EX(_T("ObjectListWindow"), CS_DBLCLKS, COLOR_WINDOW)

   BEGIN_MSG_MAP(thisClass)
      ATLASSERT_ADDED_REFLECT_NOTIFICATIONS()
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
      NOTIFY_CODE_HANDLER(LVN_GETDISPINFO, OnGetDispInfo)
      NOTIFY_CODE_HANDLER(LVN_BEGINLABELEDIT, OnBeginLabelEdit)
      CHAIN_MSG_MAP(baseClass)
      REFLECT_NOTIFICATIONS()
      DEFAULT_REFLECTION_HANDLER()
   END_MSG_MAP()

protected:
   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnGetDispInfo(WPARAM /*wParam*/, NMHDR* /*pNMHDR*/, BOOL& /*bHandled*/);
   LRESULT OnBeginLabelEdit(WPARAM /*wParam*/, NMHDR* /*pNMHDR*/, BOOL& /*bHandled*/);

   virtual void ReceiveNotification(DebugWindowNotification& notify);

   void UpdateData();

   virtual void OnUpdatedValue(unsigned int item, unsigned int subItem, LPCTSTR value) override;

protected:
   EditListViewCtrl m_listCtrl;

   bool m_objectListInfoInited;

   ObjectListInfo m_objectList;
   CSimpleArray<CString> m_itemNameList;
   std::set<unsigned int> m_unallocatedItemListIndices;
};
