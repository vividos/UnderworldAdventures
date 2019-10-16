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
/// \file HotspotListWindow.hpp
/// \brief hotspot list
//
#pragma once

/// hotspot list docking window
class HotspotListWindow : public DockingWindowBase
{
   typedef HotspotListWindow thisClass;
   typedef DockingWindowBase baseClass;
public:
   /// ctor
   HotspotListWindow() :baseClass(idHotspotListWindow) {}
   /// dtor
   virtual ~HotspotListWindow() {}

   DECLARE_DOCKING_WINDOW(_T("Hotspot List"), CSize(250, 100)/*docked*/, CSize(450, 300)/*floating*/, dockwins::CDockingSide::sLeft)

   DECLARE_WND_CLASS(_T("HotspotListWindow"))

   BEGIN_MSG_MAP(thisClass)
      ATLASSERT_ADDED_REFLECT_NOTIFICATIONS()
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
      NOTIFY_CODE_HANDLER(NM_DBLCLK, OnDblClick);
   CHAIN_MSG_MAP(baseClass)
   END_MSG_MAP()

private:
   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnDblClick(WPARAM /*wParam*/, NMHDR* pNMHDR, BOOL& /*bHandled*/);

private:
   /// list with all hotspots
   CListViewCtrl m_listCtrl;
};
