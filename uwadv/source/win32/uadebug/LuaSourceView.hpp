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
/*! \file LuaSourceView.hpp

   \brief Lua source file view

*/
//! \ingroup uadebug

//@{

// include guard
#pragma once

// includes
#include "Resource.h"

// classes

class CLuaSourceCtrl: public CScintillaWindowImpl<CLuaSourceCtrl>
{
   typedef CScintillaWindowImpl<CLuaSourceCtrl> baseClass;

public:
   CLuaSourceCtrl(){}
   virtual ~CLuaSourceCtrl(){}

   DECLARE_WND_CLASS(NULL)

   BOOL PreTranslateMessage(MSG* pMsg)
   {
      pMsg;
      return FALSE;
   }

protected:
   BEGIN_MSG_MAP(CLuaSourceCtrl)
      CHAIN_MSG_MAP(baseClass)
   END_MSG_MAP()

   int HandleNotify(LPARAM lParam)
   {
      return baseClass::HandleNotify(lParam);
   }

// Handler prototypes (uncomment arguments if needed):
//   LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//   LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//   LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
};


class CLuaSourceView : public CChildWindowBase<IDR_LUA_SOURCE_FRAME>
{
   typedef CLuaSourceView thisClass;
   typedef CChildWindowBase<IDR_LUA_SOURCE_FRAME> baseClass;

public:
   //! ctor
   CLuaSourceView():m_bModified(false){ m_bDynamicWindow = true; }

   //! returns filename
   LPCTSTR GetFilename() const { return m_cszFilename; }

   //! creates a new unnamed file
   void NewFile()
   {
      m_cszFilename = _T("unnamed.lua");
      UpdateFilename();
   }

   //! opens file with given name
   bool OpenFile(LPCTSTR pszFilename)
   {
      m_cszFilename = pszFilename;
      UpdateFilename();
      return m_view.OpenFile(pszFilename);
   }

   //! saves file
   bool SaveFile()
   {
      bool bRet = m_view.SaveFile(m_cszFilename);
      SetModified(false);
      UpdateFilename();
      return bRet;
   }

   //! saves file under another name
   bool SaveAs(CString cszNewFilename)
   {
      m_cszFilename = cszNewFilename;
      return SaveFile();
   }

   //! returns "modified" state of file
   bool IsModified() const { return m_bModified; }

   //! sets "modified" state of file
   void SetModified(bool bModified){ m_bModified = bModified; }

protected:
   // message map
   BEGIN_MSG_MAP(thisClass)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
      CHAIN_MSG_MAP(baseClass)
   END_MSG_MAP()

   // message map handler

   LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      LPMSG pMsg = (LPMSG)lParam;

      if(baseClass::PreTranslateMessage(pMsg))
         return TRUE;

      return m_view.PreTranslateMessage(pMsg);
   }

   //! updates displayed filename
   void UpdateFilename();

protected:
   //! scintilla edit view
   CLuaSourceCtrl m_view;

   //! indicates if file is modified
   bool m_bModified;

   //! filename
   CString m_cszFilename;
};

//@}
