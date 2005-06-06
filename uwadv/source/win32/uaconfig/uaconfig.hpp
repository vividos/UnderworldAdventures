/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004,2005 Underworld Adventures Team

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

// exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#define VC_EXTRALEAN
#define NOSERVICE
#define NOMCX
#define NOIME
#define NOSOUND
#define NOCOMM
#define NOKANJI
#define NORPC
#define NOPROXYSTUB
#define NOTAPE
#define NOCRYPT
#define NOIMAGE
#include <atlbase.h>
#include <atlapp.h>
extern CAppModule _Module;
#include <atlwin.h>

#include "resource.h"
#include "settings.hpp"


// classes

class ua_tooltip_ctrl
{
public:
   void init(HWND parent)
   {
      // create tooltip window
      hwnd = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, 0,
         0,0,0,0,parent, NULL, _Module.GetResourceInstance(), NULL);

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
      toolinfo.hinst = _Module.GetResourceInstance();
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


//! config dialog class
class ua_config_dlg: public CDialogImpl<ua_config_dlg>
{
public:
   enum { IDD = IDD_UACONFIG };

   //! ctor
   ua_config_dlg(){}

   //! runs dialog main loop
   void RunDialog();

   // message map
   BEGIN_MSG_MAP(ua_config_dlg)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      COMMAND_ID_HANDLER(IDOK, OnSaveExit)
      COMMAND_ID_HANDLER(IDCANCEL, OnExit)
      COMMAND_ID_HANDLER(IDC_BUTTON_SET_UW1_PATH, OnSetUw1Path);
   END_MSG_MAP()

   //! translates messages before processing
   BOOL PreTranslateMessage(MSG* pMsg)
   {
      if (pMsg->message==WM_MOUSEMOVE)
         tooltips.relay_event(pMsg);
      return FALSE;
   }

protected:
   // message handler

   //! called on initing dialog
   LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   //! called when leaving dialog
   LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      bHandled = FALSE;
      tooltips.done();
      return 0;
   }

   //! called to save settings and exit dialog
   LRESULT OnSaveExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      if (check_config())
      {
         save_config();
         PostMessage(WM_QUIT);
      }
      return 0;
   }

   //! called to exit dialog
   LRESULT OnExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      PostMessage(WM_QUIT);
      return 0;
   }

   //! called on button press to set uw1 path
   LRESULT OnSetUw1Path(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   //! shows message box
   void MessageBox(LPCTSTR pszText, UINT nType = MB_OK);

protected:
   //! loads config file
   void load_config();

   //! checks configuration
   bool check_config();

   //! saves config file
   void save_config();

protected:
   //! window caption
   std::string caption;

   //! settings filename
   std::string settings_filename;

   //! window icons
   HICON wndicon, wndicon_small;

   //! tooltip control
   ua_tooltip_ctrl tooltips;

   //! settings
   ua_settings settings;
};

#endif
