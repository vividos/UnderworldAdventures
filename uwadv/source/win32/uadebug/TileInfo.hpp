//
// Underworld Adventures Debugger - a debugger tool for Underworld Adventures
// Copyright (c) 2005,2019 Michael Fink
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
/// \file TileInfo.hpp
/// \brief tile info docking window
//
#pragma once

#include "WindowBase.hpp"
#include "EditListViewCtrl.hpp"
#include "Resource.h"

/// form with tile infos
class CTileInfoForm :
   public CDialogImpl<CTileInfoForm>,
   public CDebugWindowBase
{
public:
   CTileInfoForm();
   virtual ~CTileInfoForm() {}

   // dialog form id // TODO change id
   enum { IDD = IDD_TILE_INFO };

   BOOL PreTranslateMessage(MSG* pMsg)
   {
      return IsDialogMessage(pMsg);
   }

   BEGIN_MSG_MAP(CTileMapViewWindow)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      COMMAND_HANDLER(IDC_BUTTON_BEAM, BN_CLICKED, OnButtonBeam)
      NOTIFY_HANDLER(IDC_LIST_OBJECTS, LVN_ITEMCHANGED, OnListItemChanged)
      REFLECT_NOTIFICATIONS()
   END_MSG_MAP()

protected:
   LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnButtonBeam(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnListItemChanged(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

   // virtual methods from CDebugWindowBase

   virtual void ReceiveNotification(CDebugWindowNotification& notify) override;

   void UpdateData();

   void UpdateTileInfo();
   void UpdateObjectInfo();

protected:
   CEditListViewCtrl m_tileInfoList;
   CEditListViewCtrl m_objectList;

   bool m_bInited;

   unsigned int m_nTileX, m_nTileY;
};


/// player info docking window
class CTileInfoWindow : public CDockingWindowBase
{
   typedef CTileInfoWindow thisClass;
   typedef CDockingWindowBase baseClass;
public:
   /// ctor
   CTileInfoWindow() :baseClass(idTileInfoWindow) {}
   virtual ~CTileInfoWindow() {}

   BOOL PreTranslateMessage(MSG* pMsg)
   {
      return m_form.PreTranslateMessage(pMsg);
   }

   DECLARE_DOCKING_WINDOW(_T("Tile Info"), CSize(300, 700)/*docked*/, CSize(200, 300)/*floating*/, dockwins::CDockingSide::sRight)

   DECLARE_WND_CLASS_EX(_T("TileInfo"), CS_DBLCLKS, COLOR_WINDOW)

   BEGIN_MSG_MAP(thisClass)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
      CHAIN_MSG_MAP(baseClass)
   END_MSG_MAP()

protected:

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      bHandled = FALSE;
      m_form.Create(m_hWnd);

      return 0;
   }

   LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

   // virtual methods from CDebugWindowBase

   virtual void InitDebugWindow(IMainFrame* pMainFrame) override
   {
      CDebugWindowBase::InitDebugWindow(pMainFrame);
      pMainFrame->AddDebugWindow(&m_form);
   }

   virtual void DoneDebugWindow() override
   {
      m_pMainFrame->RemoveDebugWindow(&m_form);
      CDebugWindowBase::DoneDebugWindow();
   }

   virtual void ReceiveNotification(CDebugWindowNotification& notify) override
   {
      // relay notification to descendant window, if needed
      if (notify.m_bRelayToDescendants)
         m_pMainFrame->SendNotification(notify, &m_form);
   }

protected:
   /// tile info dialog
   CTileInfoForm m_form;
};
