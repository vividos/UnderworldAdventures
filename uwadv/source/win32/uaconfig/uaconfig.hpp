/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

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
/*! \file uaconfig.hpp

   \brief underworld adventures win32 config program
*/

// include guard
#ifndef uwadv_uaconfig_hpp_
#define uwadv_uaconfig_hpp_

// needed includes
#include "common.hpp"
#include "resource.h"
#include "settings.hpp"


// message map macros (inspired by ATL)

#define BEGIN_MSG_MAP(x) \
   BOOL ProcessWindowMessage( \
      HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, \
      LRESULT& lResult){ \
         BOOL bHandled = FALSE;

#define END_MSG_MAP() \
   return FALSE; }

#define MESSAGE_HANDLER(msg,func) \
   if (uMsg==msg){ \
      bHandled = TRUE; \
      lResult = func(uMsg,wParam,lParam,bHandled); \
      if (bHandled) return TRUE; }

#define COMMAND_ID_HANDLER(id,func) \
   if(uMsg==WM_COMMAND && id==LOWORD(wParam)){ \
      bHandled = TRUE; \
      lResult = func(HIWORD(wParam), LOWORD(wParam), (HWND)lParam, bHandled); \
      if(bHandled) return TRUE; }


// classes

//! config program class
class ua_config_prog
{
public:
   //! ctor
   ua_config_prog(HINSTANCE hInst);

   //! runs dialog main loop
   void RunDialog();

   // dialog id
   static const UINT dialog_id;
   enum { IDD = IDD_DIALOG_UACONFIG };

   // message map
BEGIN_MSG_MAP(ua_config_prog)
   MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
   COMMAND_ID_HANDLER(IDOK, OnSaveExit)
   COMMAND_ID_HANDLER(IDCANCEL, OnExit)
   COMMAND_ID_HANDLER(IDC_BUTTON_SET_UW1_PATH, OnSetUw1Path);
END_MSG_MAP()

   //! translates messages before processing
   BOOL PreTranslateMessage(MSG* pMsg);

protected:
   //! static dialog proc function
   static LRESULT CALLBACK StartDialogProc(HWND hWnd, UINT uMsg,
      WPARAM wParam, LPARAM lParam);

   //! dialog proc function
   LRESULT CALLBACK DialogProc(HWND hWnd, UINT uMsg,
      WPARAM wParam, LPARAM lParam);

protected:
   // message handler

   //! called on initing dialog
   LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   //! called to save settings and exit dialog
   LRESULT OnSaveExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
   {
      if (check_config())
      {
         save_config();
         ::PostMessage(m_hWnd,WM_QUIT,0,0);
      }
      return 0;
   }

   //! called to exit dialog
   LRESULT OnExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
   {
      ::PostMessage(m_hWnd,WM_QUIT,0,0);
      return 0;
   }

   //! called on button press to set uw1 path
   LRESULT OnSetUw1Path(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

protected:
   //! loads config file
   void load_config();

   //! checks configuration
   bool check_config();

   //! saves config file
   void save_config();

protected:
   //! current config dialog
   static ua_config_prog *current_dlg;

   //! window caption
   std::string caption;

   //! win32 program instance
   HINSTANCE m_hInstance;

   //! dialog window handle
   HWND m_hWnd;

   //! window icons
   HICON wndicon, wndicon_small;

   //! settings
   ua_settings settings;
};

#endif
