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
/// \file PlayerInfoWindow.hpp
/// \brief player info docking window
//
#pragma once

#include "PropertyListCtrl.hpp"

/// player info docking window
class PlayerInfoWindow :
   public DockingWindowBase,
   //public IEditListViewCallback,
   public PropertyList::IPropertyStorage
{
   typedef PlayerInfoWindow thisClass;
   typedef DockingWindowBase baseClass;

public:
   /// ctor
   PlayerInfoWindow() :baseClass(idPlayerInfoWindow) {}

   DECLARE_DOCKING_WINDOW(_T("Player Info"), CSize(200, 100)/*docked*/, CSize(200, 600)/*floating*/, dockwins::CDockingSide::sRight)

   DECLARE_WND_CLASS(_T("PlayerInfoWindow"))
   BEGIN_MSG_MAP(thisClass)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
      CHAIN_MSG_MAP(baseClass)
      REFLECT_NOTIFICATIONS()
   END_MSG_MAP()

protected:
   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

   // virtual methods from DockingWindowBase
   void ReceiveNotification(DebugWindowNotification& notify) override;

   /// updates player info data
   void UpdateData();

   // virtual methods from IEditListViewCallback
   //virtual void OnUpdatedValue(unsigned int item, unsigned int subItem, LPCTSTR value) override;

   // virtual methods from PropertyList::IPropertyStorage
   virtual void GetProperty(unsigned int propertyItemId, CString& propertyValue) override;
   virtual void SetProperty(unsigned int propertyItemId, const CString& propertyValue) override;
   virtual bool VerifyProperty(unsigned int propertyItemId, const CString& propertyValue) override;

protected:
   /// player info list
   //EditListViewCtrl m_listCtrl;
   PropertyListCtrl m_listCtrl;

   double m_doubleValues[4];
   unsigned int m_intValues[42];

   /// indicates if control is read-only
   bool m_isReadOnly;
};
