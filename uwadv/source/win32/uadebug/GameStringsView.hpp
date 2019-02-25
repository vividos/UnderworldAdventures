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
/// \file GameStringsView.hpp
/// \brief game strings view
//
#pragma once

#include "Resource.h"

/// gamestrings view form
class GameStringsView :
   public CDialogImpl<GameStringsView>,
   public DebugWindowBase
{
public:
   GameStringsView() {}
   virtual ~GameStringsView();

   enum { IDD = IDD_GAMESTRINGS };

   BOOL PreTranslateMessage(MSG* pMsg)
   {
      return IsDialogMessage(pMsg);
   }

protected:
   BEGIN_MSG_MAP(GameStringsView)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      COMMAND_CODE_HANDLER(CBN_SELCHANGE, OnComboSelChange);
   END_MSG_MAP()

   LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnComboSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   virtual void InitDebugWindow(IMainFrame* mainFrame)
   {
      DebugWindowBase::InitDebugWindow(mainFrame);
      InitCombobox();
   }

   /// initializes combobox content
   void InitCombobox();

protected:
   /// list view with all strings
   CListViewCtrl m_listStrings;

   /// combobox to select string blocks
   CComboBox m_comboBlocks;
};

/// gamestrings frame
class GameStringsViewChildFrame : public ChildWindowBase<IDR_GAME_STRINGS>
{
   typedef GameStringsViewChildFrame thisClass;
   typedef ChildWindowBase<IDR_GAME_STRINGS> baseClass;
public:
   GameStringsViewChildFrame()
   {
      m_isDynamicWindow = true;
   }

protected:
   BEGIN_MSG_MAP(thisClass)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
      CHAIN_MSG_MAP(baseClass)
   END_MSG_MAP()

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL);

      bHandled = FALSE;
      return 1;
   }

   LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      bHandled = FALSE;
      m_mainFrame->RemoveDebugWindow(&m_view);
      m_mainFrame->RemoveDebugWindow(this);
      return 0;
   }

   LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      LPMSG msg = (LPMSG)lParam;

      if (baseClass::PreTranslateMessage(msg))
         return TRUE;

      return m_view.PreTranslateMessage(msg);
   }

   virtual void InitDebugWindow(IMainFrame* mainFrame)
   {
      baseClass::InitDebugWindow(mainFrame);
      mainFrame->AddDebugWindow(&m_view);
   }

   virtual void DoneDebugWindow()
   {
      m_mainFrame->RemoveDebugWindow(&m_view);
      baseClass::DoneDebugWindow();
   }

   virtual void ReceiveNotification(DebugWindowNotification& notify)
   {
      // relay notification to descendant window, if needed
      if (notify.m_relayToDescendants)
         m_mainFrame->SendNotification(notify, &m_view);
   }

protected:
   GameStringsView m_view;
};
