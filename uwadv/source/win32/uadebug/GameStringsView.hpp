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
#include "WindowBase.hpp"
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
   LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
   LRESULT OnComboSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

protected:
   //! list view with all strings
   CListViewCtrl m_listStrings;

   //! combobox to select string blocks
   CComboBox m_comboBlocks;
};

//! gamestrings 
class CGameStringsViewChildFrame: public CChildWindowBase
{
   typedef CGameStringsViewChildFrame thisClass;
   typedef CChildWindowBase baseClass;
public:
   DECLARE_FRAME_WND_CLASS(NULL, IDR_GAME_STRINGS)

   CGameStringsView& GetView(){ return m_view; }

//   void UpdateData(){ m_view.UpdateData(); }

protected:
   virtual void OnFinalMessage(HWND /*hWnd*/)
   {
      delete this;
   }

   BEGIN_MSG_MAP(thisClass)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
      CHAIN_MSG_MAP(baseClass)
   END_MSG_MAP()

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL);

      bHandled = FALSE;
      return 1;
   }

   LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      LPMSG pMsg = (LPMSG)lParam;

      if (baseClass::PreTranslateMessage(pMsg))
         return TRUE;

      return m_view.PreTranslateMessage(pMsg);
   }

protected:
   CGameStringsView m_view;
};

//@}
