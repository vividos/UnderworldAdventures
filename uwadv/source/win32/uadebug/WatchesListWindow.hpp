//
// Underworld Adventures Debugger - a debugger tool for Underworld Adventures
// Copyright (c) 2005,2019 Underworld Adventures Team
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
/// \file WatchesListWindow.hpp
/// \brief code debugger watches window
//
#pragma once

class WatchesListWindow :
   public DockingWindowBase//,
   //public IEditListViewCallback
{
   typedef WatchesListWindow thisClass;
   typedef DockingWindowBase baseClass;

public:
   WatchesListWindow(unsigned int codeDebuggerId);
   ~WatchesListWindow() {}

   DECLARE_DOCKING_WINDOW(_T("Watches"), CSize(200, 100)/*docked*/, CSize(500, 150)/*floating*/, dockwins::CDockingSide::sBottom)

   DECLARE_WND_CLASS_EX(_T("WatchesListWindow"), CS_DBLCLKS, COLOR_WINDOW)

   BEGIN_MSG_MAP(thisClass)
      ATLASSERT_ADDED_REFLECT_NOTIFICATIONS()
      CHAIN_MSG_MAP(baseClass)
      DEFAULT_REFLECTION_HANDLER()
   END_MSG_MAP()

   virtual void ReceiveNotification(DebugWindowNotification& notify) override;

   //virtual void OnUpdatedValue(unsigned int item, unsigned int subItem, LPCTSTR value) override;

private:
   /// edit list view with all breakpoints
   CListViewCtrl m_listCtrl;

   /// code debugger id
   unsigned int m_codeDebuggerId;
};
