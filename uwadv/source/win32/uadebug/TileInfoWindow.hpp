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
/// \file TileInfoWindow.hpp
/// \brief tile info docking window
//
#pragma once

#include "WindowBase.hpp"
#include "EditListViewCtrl.hpp"
#include "Resource.h"

/// form with tile infos
class TileInfoForm :
   public CDialogImpl<TileInfoForm>,
   public CDialogResize<TileInfoForm>,
   public CWinDataExchange<TileInfoForm>,
   public DebugWindowBase
{
public:
   /// ctor
   TileInfoForm();
   /// dtor
   virtual ~TileInfoForm() {}

   /// dialog form ID
   enum { IDD = IDD_TILE_INFO };

   /// called to translate dialog messages
   BOOL PreTranslateMessage(MSG* msg)
   {
      return IsDialogMessage(msg);
   }

private:
   BEGIN_MSG_MAP(TileInfoForm)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      COMMAND_HANDLER(IDC_BUTTON_BEAM, BN_CLICKED, OnButtonBeam)
      NOTIFY_HANDLER(IDC_LIST_OBJECTS, LVN_ITEMCHANGED, OnListItemChanged)
      CHAIN_MSG_MAP(CDialogResize<TileInfoForm>)
      REFLECT_NOTIFICATIONS()
   END_MSG_MAP()

   BEGIN_DLGRESIZE_MAP(TileInfoForm)
      DLGRESIZE_CONTROL(IDC_EDIT_TILEPOS, DLSZ_SIZE_X)
      DLGRESIZE_CONTROL(IDC_BUTTON_BEAM, DLSZ_MOVE_X)
      DLGRESIZE_CONTROL(IDC_LIST_TILE_PROPERTIES, DLSZ_SIZE_X)
      DLGRESIZE_CONTROL(IDC_LIST_OBJECTS, DLSZ_SIZE_X | DLSZ_SIZE_Y)
   END_DLGRESIZE_MAP()

   BEGIN_DDX_MAP(TileInfoForm)
      DDX_CONTROL_HANDLE(IDC_EDIT_TILEPOS, m_tilePos)
      DDX_CONTROL(IDC_LIST_TILE_PROPERTIES, m_tileInfoList)
      DDX_CONTROL(IDC_LIST_OBJECTS, m_objectList)
   END_DDX_MAP()

   /// called to init dialog
   LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// called when the user clicked on the "Beam" button
   LRESULT OnButtonBeam(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   /// called when selected object list item has changed
   LRESULT OnListItemChanged(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

   // virtual methods from DebugWindowBase

   /// receives debug window notifications
   virtual void ReceiveNotification(DebugWindowNotification& notify) override;

   /// updates tile info
   void UpdateTileInfo();

   /// updates object info
   void UpdateObjectInfo();

private:
   CEdit m_tilePos;  ///< current tile position text
   EditListViewCtrl m_tileInfoList; ///< tile info list
   EditListViewCtrl m_objectList;   ///< object list for current tile

   unsigned int m_tileX;   ///< current tile X position
   unsigned int m_tileY;   ///< current tile Y position
};

/// tile info docking window
class TileInfoWindow : public DockingWindowBase
{
   typedef TileInfoWindow thisClass;
   typedef DockingWindowBase baseClass;

public:
   /// ctor
   TileInfoWindow() :baseClass(idTileInfoWindow) {}
   /// dtor
   virtual ~TileInfoWindow() {}

   /// called to translate dialog messages
   BOOL PreTranslateMessage(MSG* msg)
   {
      return m_form.PreTranslateMessage(msg);
   }

   DECLARE_DOCKING_WINDOW(_T("Tile Info"), CSize(300, 700)/*docked*/, CSize(200, 300)/*floating*/, dockwins::CDockingSide::sRight)

   DECLARE_WND_CLASS_EX(_T("TileInfo"), CS_DBLCLKS, COLOR_WINDOW)

private:
   BEGIN_MSG_MAP(thisClass)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
      CHAIN_MSG_MAP(baseClass)
   END_MSG_MAP()

   /// called when window is created
   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      bHandled = FALSE;
      m_form.Create(m_hWnd);

      return 0;
   }

   /// called to resize form window
   LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

   /// called when focus is set to this window
   LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

   // virtual methods from DebugWindowBase

   /// initializes debug window
   virtual void InitDebugWindow(IMainFrame* mainFrame) override
   {
      DebugWindowBase::InitDebugWindow(mainFrame);
      mainFrame->AddDebugWindow(&m_form);
   }

   /// cleans up debug window
   virtual void DoneDebugWindow() override
   {
      m_mainFrame->RemoveDebugWindow(&m_form);
      DebugWindowBase::DoneDebugWindow();
   }

   /// receives debug window notifications
   virtual void ReceiveNotification(DebugWindowNotification& notify) override
   {
      // relay notification to descendant window, if needed
      if (notify.m_relayToDescendants)
         m_mainFrame->SendNotification(notify, &m_form);
   }

private:
   /// tile info dialog
   TileInfoForm m_form;
};
