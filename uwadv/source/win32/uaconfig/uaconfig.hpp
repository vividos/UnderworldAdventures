/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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

class ua_tooltip_ctrl
{
public:
   void init(HWND parent, HINSTANCE hinst)
   {
      // create tooltip window
      hwnd = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, 0,
         0,0,0,0,parent, NULL, hinst, NULL);

      // get first child window
      HWND child = ::GetWindow(parent,GW_CHILD);

      char tooltext[256];

      // go through all child windows
      while (child!=NULL)
      {
         // get ctrl id
         int id = ::GetDlgCtrlID(child);

         if (id != -1 && id != 0)
         {
            // try to load text from string table
            tooltext[0] = 0;
            ::LoadString(NULL, id, tooltext, 256);

            // when successful, add text as tooltip text
            if (strlen(tooltext) != 0)
               add_tool(child, tooltext);
         }

         // get next window
         child = GetWindow(child,GW_HWNDNEXT);
      }

      // activate tool tips
      ::SendMessage(hwnd, TTM_ACTIVATE, TRUE, 0L);

      // set autopop delay time to 10s
      ::SendMessage(hwnd, TTM_SETDELAYTIME, TTDT_AUTOPOP, MAKELPARAM(10*1000, 0));
   }

   //! adds tooltip for given control
   void add_tool(HWND ctrl, char* text)
   {
      // set up toolinfo struct
      TOOLINFO toolinfo;
      memset(&toolinfo, 0, sizeof(toolinfo));

      toolinfo.cbSize = sizeof(toolinfo);
      toolinfo.uFlags = TTF_IDISHWND;
      toolinfo.hwnd = ::GetParent(ctrl);
      toolinfo.uId = (UINT_PTR)ctrl;
      toolinfo.hinst = NULL;
      toolinfo.lpszText = text;

      ::SendMessageA(hwnd, TTM_ADDTOOL, 0, (LPARAM)&toolinfo);
   }

   //! relay mouse move event to tooltip control
   void relay_event(MSG* msg)
   {
      ::SendMessage(hwnd, TTM_RELAYEVENT, 0, (LPARAM)msg);
   }

   void done()
   {
      DestroyWindow(hwnd);
      hwnd = NULL;
   }

protected:
   HWND hwnd;
};


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
   MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
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

   //! called when leaving dialog
   LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
   {
      tooltips.done();
      return 0;
   }

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
   static ua_config_prog* current_dlg;

   //! window caption
   std::string caption;

   //! win32 program instance
   HINSTANCE m_hInstance;

   //! dialog window handle
   HWND m_hWnd;

   //! window icons
   HICON wndicon, wndicon_small;

   //! tooltip control
   ua_tooltip_ctrl tooltips;

   //! settings
   ua_settings settings;
};

#endif
