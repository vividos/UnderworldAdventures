//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2004,2005,2019 Underworld Adventures Team
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
/// \file uaconfig.hpp
/// \brief Underworld Adventures win32 config program
//
#pragma once

#define _ATL_NO_COM_SUPPORT
#include <atlbase.h>
extern CComModule _Module;
#include <atlwin.h>
#include "resource.h"
#include "settings.hpp"

/// tooltip control
class TooltipCtrl
{
public:
   /// Adds tooltips for all controls
   void Init(HWND parent)
   {
      m_hwnd = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, 0,
         0, 0, 0, 0, parent, NULL, _Module.GetResourceInstance(), NULL);

      HWND child = ::GetWindow(parent, GW_CHILD);

      char tooltipText[256];

      while (child != NULL)
      {
         int id = ::GetDlgCtrlID(child);

         if (id != -1 && id != 0)
         {
            // try to load text from string table
            tooltipText[0] = 0;
            ::LoadString(NULL, id, tooltipText, 256);

            // when successful, add text as tooltip text
            if (strlen(tooltipText) != 0)
               AddTooltip(child, tooltipText);
         }

         child = GetWindow(child, GW_HWNDNEXT);
      }

      ::SendMessage(m_hwnd, TTM_ACTIVATE, TRUE, 0L);

      // set autopop delay time to 10s
      ::SendMessage(m_hwnd, TTM_SETDELAYTIME, TTDT_AUTOPOP, MAKELPARAM(10 * 1000, 0));
   }

   /// adds tooltip for given control
   void AddTooltip(HWND ctrl, char* text)
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

      ::SendMessageA(m_hwnd, TTM_ADDTOOL, 0, (LPARAM)&toolinfo);
   }

   /// relay mouse move event to tooltip control
   void RelayEvent(MSG* msg)
   {
      ::SendMessage(m_hwnd, TTM_RELAYEVENT, 0, (LPARAM)msg);
   }

   /// cleans up tooltip window
   void Done()
   {
      DestroyWindow(m_hwnd);
      m_hwnd = NULL;
   }

private:
   /// tooltip window handle
   HWND m_hwnd;
};


/// config dialog class
class ConfigDlg : public CDialogImpl<ConfigDlg>
{
public:
   /// dialog ID
   enum { IDD = IDD_UACONFIG };

   /// ctor
   ConfigDlg() {}

   /// runs dialog main loop
   void RunDialog();

   // message map
   BEGIN_MSG_MAP(ConfigDlg)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      COMMAND_ID_HANDLER(IDOK, OnSaveExit)
      COMMAND_ID_HANDLER(IDCANCEL, OnExit)
      COMMAND_ID_HANDLER(IDC_BUTTON_SET_UW1_PATH, OnSetUw1Path);
   END_MSG_MAP()

   /// translates messages before processing
   BOOL PreTranslateMessage(MSG* pMsg)
   {
      if (pMsg->message == WM_MOUSEMOVE)
         m_tooltips.RelayEvent(pMsg);
      return FALSE;
   }

protected:
   // message handler

   /// called on initing dialog
   LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// called when leaving dialog
   LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      bHandled = FALSE;
      m_tooltips.Done();
      return 0;
   }

   /// called to save settings and exit dialog
   LRESULT OnSaveExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      if (CheckConfig())
      {
         SaveConfig();
         PostMessage(WM_QUIT);
      }
      return 0;
   }

   /// called to exit dialog
   LRESULT OnExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      PostMessage(WM_QUIT);
      return 0;
   }

   /// called on button press to set uw1 path
   LRESULT OnSetUw1Path(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   /// shows message box
   void ShowMessageBox(LPCTSTR text, UINT type = MB_OK);

protected:
   /// loads config file
   void LoadConfig();

   /// checks configuration
   bool CheckConfig();

   /// saves config file
   void SaveConfig();

protected:
   /// window m_caption
   std::string m_caption;

   /// settings filename
   std::string m_settingsFilename;

   /// tooltip control
   TooltipCtrl m_tooltips;

   /// settings
   Base::Settings m_settings;
};
