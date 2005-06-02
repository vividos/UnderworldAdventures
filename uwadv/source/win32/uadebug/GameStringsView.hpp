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
/*! \file GameStringsView.hpp

   \brief game strings view

*/
//! \ingroup uadebug

//@{

// include guard
#pragma once

// includes
#include "Resource.h"

// classes

//! gamestrings view form
class CGameStringsView:
   public CDialogImpl<CGameStringsView>,
   public CDebugWindowBase
{
public:
   CGameStringsView(){}
   virtual ~CGameStringsView();

   enum { IDD = IDD_GAMESTRINGSVIEW };

   BOOL PreTranslateMessage(MSG* pMsg)
   {
      return IsDialogMessage(pMsg);
   }

protected:
   BEGIN_MSG_MAP(CGameStringsView)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      COMMAND_CODE_HANDLER(CBN_SELCHANGE, OnComboSelChange);
   END_MSG_MAP()

   LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnComboSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   virtual void InitDebugWindow(IMainFrame* pMainFrame)
   {
      CDebugWindowBase::InitDebugWindow(pMainFrame);
      if (m_pMainFrame != NULL)
         InitCombobox();
   }

   //! initializes combobox content
   void InitCombobox();

protected:
   //! list view with all strings
   CListViewCtrl m_listStrings;

   //! combobox to select string blocks
   CComboBox m_comboBlocks;
};

//! gamestrings frame
class CGameStringsViewChildFrame : public CChildWindowBase<IDR_GAME_STRINGS>
{
   typedef CGameStringsViewChildFrame thisClass;
   typedef CChildWindowBase<IDR_GAME_STRINGS> baseClass;
public:
   CGameStringsViewChildFrame()
   {
      m_bDynamicWindow = true;
   }

protected:
   // message map
   BEGIN_MSG_MAP(thisClass)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
      CHAIN_MSG_MAP(baseClass)
   END_MSG_MAP()

   // message handler

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL);

      bHandled = FALSE;
      return 1;
   }

   LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      bHandled = FALSE;
      m_pMainFrame->RemoveDebugWindow(&m_view);
      m_pMainFrame->RemoveDebugWindow(this);
      return 0;
   }

   LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      LPMSG pMsg = (LPMSG)lParam;

      if (baseClass::PreTranslateMessage(pMsg))
         return TRUE;

      return m_view.PreTranslateMessage(pMsg);
   }

   virtual void InitDebugWindow(IMainFrame* pMainFrame)
   {
      baseClass::InitDebugWindow(pMainFrame);
      if (pMainFrame != NULL)
         pMainFrame->AddDebugWindow(&m_view);
   }

   virtual void ReceiveNotification(CDebugWindowNotification& notify)
   {
      // relay notification to descendant window, if needed
      if (notify.m_bRelayToDescendants)
         m_pMainFrame->SendNotification(notify, &m_view);
   }

protected:
   CGameStringsView m_view;
};

//@}
