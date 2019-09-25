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
/// \file GameStringsView.hpp
/// \brief game strings view
//
#pragma once

#include "Resource.h"

/// game strings view form
class GameStringsView :
   public CDialogImpl<GameStringsView>,
   public CDialogResize<GameStringsView>,
   public CWinDataExchange<GameStringsView>,
   public DebugWindowBase
{
public:
   /// ctor
   GameStringsView() {}

   /// dialog form ID
   enum { IDD = IDD_GAMESTRINGS };

   /// called to translate dialog messages
   BOOL PreTranslateMessage(MSG* pMsg)
   {
      return IsDialogMessage(pMsg);
   }

private:
   BEGIN_MSG_MAP(GameStringsView)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      COMMAND_CODE_HANDLER(CBN_SELCHANGE, OnComboSelChange)
      CHAIN_MSG_MAP(CDialogResize<GameStringsView>)
   END_MSG_MAP()

   BEGIN_DLGRESIZE_MAP(GameStringsView)
      DLGRESIZE_CONTROL(IDC_COMBO_TEXTBLOCK, DLSZ_SIZE_X)
      DLGRESIZE_CONTROL(IDC_LIST_STRINGS, DLSZ_SIZE_X | DLSZ_SIZE_Y)
   END_DLGRESIZE_MAP()

   BEGIN_DDX_MAP(GameStringsView)
      DDX_CONTROL_HANDLE(IDC_LIST_STRINGS, m_listStrings)
      DDX_CONTROL_HANDLE(IDC_COMBO_TEXTBLOCK, m_comboBlocks)
   END_DDX_MAP()

   /// called to init form
   LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// called when block combobox has changed selection
   LRESULT OnComboSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   // virtual methods from DebugWindowBase

   /// initializes debug window
   virtual void InitDebugWindow(IMainFrame* mainFrame) override
   {
      DebugWindowBase::InitDebugWindow(mainFrame);
      InitCombobox();
   }

   /// initializes combobox content
   void InitCombobox();

   /// updates strings list with strings from given block number
   void UpdateStringsList(unsigned int blockNumber);

private:
   /// list view with all strings
   CListViewCtrl m_listStrings;

   /// combobox to select string blocks
   CComboBox m_comboBlocks;
};

/// game strings child frame
class GameStringsViewChildFrame : public ChildWindowBase<IDR_GAME_STRINGS>
{
   typedef GameStringsViewChildFrame thisClass;
   typedef ChildWindowBase<IDR_GAME_STRINGS> baseClass;

public:
   /// ctor
   GameStringsViewChildFrame()
   {
      m_isDynamicWindow = true;
   }

private:
   BEGIN_MSG_MAP(thisClass)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
      CHAIN_MSG_MAP(baseClass)
   END_MSG_MAP()

   /// called when child frame is created
   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL);

      bHandled = FALSE;
      return 1;
   }

   /// called when child frame is destroyed
   LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      bHandled = FALSE;
      m_mainFrame->RemoveDebugWindow(&m_view);
      m_mainFrame->RemoveDebugWindow(this);
      return 0;
   }

   /// called when a window message should be forwarded
   LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      LPMSG msg = (LPMSG)lParam;

      if (baseClass::PreTranslateMessage(msg))
         return TRUE;

      return m_view.PreTranslateMessage(msg);
   }

   // virtual methods from DebugWindowBase

   /// initializes debug window
   virtual void InitDebugWindow(IMainFrame* mainFrame)
   {
      baseClass::InitDebugWindow(mainFrame);
      mainFrame->AddDebugWindow(&m_view);
   }

   /// cleans up debug window
   virtual void DoneDebugWindow()
   {
      m_mainFrame->RemoveDebugWindow(&m_view);
      baseClass::DoneDebugWindow();
   }

   /// receives debug window notifications
   virtual void ReceiveNotification(DebugWindowNotification& notify)
   {
      // relay notification to descendant window, if needed
      if (notify.m_relayToDescendants)
         m_mainFrame->SendNotification(notify, &m_view);
   }

private:
   /// game strings form view
   GameStringsView m_view;
};
