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
/*! \file uaconfig.cpp

   \brief underworld adventures win32 config application

*/

// needed includes
#include "common.hpp"
#include "uaconfig.hpp"
#include <string>
#include <atlconv.h>
#include <mmsystem.h> // for midiOutGet*
#include <shlobj.h> // for SHBrowseForFolder
#include <tchar.h> // for _sntprintf


// ua_config_dlg methods

void ua_config_dlg::RunDialog()
{
   // create and show the dialog
   Create(NULL);
   ShowWindow(SW_SHOW);

   // run the message loop of the modeless dialog
   MSG msg;

   for(;;)
   {
      // retrieve next message
      BOOL bRet = ::GetMessage(&msg, NULL, 0, 0);

      // quit message loop on WM_QUIT
      if (!bRet)
         break;

      // deliver message
      if (!PreTranslateMessage(&msg) && !IsDialogMessage(&msg))
      {
         ::TranslateMessage(&msg);
         ::DispatchMessage(&msg);
      }
   }

   // destroy the dialog window
   DestroyWindow();
}


// message handler methods

LRESULT ua_config_dlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   // center main window
   CenterWindow();

   // set icons
   HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_ICON_UACONFIG), 
      IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
   SetIcon(hIcon, TRUE);

   HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_ICON_UACONFIG), 
      IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
   SetIcon(hIconSmall, FALSE);

   // cutscene narration combobox
   SendDlgItemMessage(IDC_COMBO_CUTS_NARRATION, CB_ADDSTRING, 0, (LPARAM)_T("Sound"));
   SendDlgItemMessage(IDC_COMBO_CUTS_NARRATION, CB_ADDSTRING, 0, (LPARAM)_T("Subtitles"));
   SendDlgItemMessage(IDC_COMBO_CUTS_NARRATION, CB_ADDSTRING, 0, (LPARAM)_T("Sound+Subtitles"));

   // add all available resolutions to screen resolution combo box
   {
      DWORD iModeNum=0;
      DEVMODE devmode;
      devmode.dmSize = sizeof(DEVMODE);
      devmode.dmDriverExtra = 0;

      while (0 != ::EnumDisplaySettings(NULL, iModeNum++, &devmode))
      {
         // only add hi-/truecolor modes
         if (devmode.dmBitsPerPel<16) continue;

         _TCHAR szBuffer[32];
         _sntprintf(szBuffer, 32, _T("%u x %u"), devmode.dmPelsWidth, devmode.dmPelsHeight);

         // check if we already have that one
         if (CB_ERR == SendDlgItemMessage(IDC_COMBO_SCREEN_RESOLUTION,
            CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)szBuffer))
         {
            // add it
            SendDlgItemMessage(IDC_COMBO_SCREEN_RESOLUTION, CB_ADDSTRING,
               0, (LPARAM)szBuffer);
         }
      }
   }

   // add all midi devices to combo box
   {
      MIDIOUTCAPS caps;
      UINT max = midiOutGetNumDevs();

      for(signed int n=-1; n<(signed)max; n++)
      {
         // get midi device caps
         midiOutGetDevCaps((UINT)n, &caps, sizeof(caps));

         // add to combo box
         SendDlgItemMessage(IDC_COMBO_MIDI_DEVICE, CB_ADDSTRING,
            0, (LPARAM)caps.szPname);
      }
   }

   // load configuration
   load_config();

   // add tooltips for all controls
   tooltips.init(m_hWnd);

   return 1;  // let the system set the focus
}

static int CALLBACK WINAPI ua_browse_callback(HWND hwnd, UINT uMsg, LPARAM /*lParam*/, LPARAM lpData)
{
   // set initial path on initialisation
   if (uMsg == BFFM_INITIALIZED)
      ::SendMessage(hwnd, BFFM_SETSELECTION, (WPARAM)TRUE, lpData);
   return 0;
}

LRESULT ua_config_dlg::OnSetUw1Path(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   _TCHAR buffer[MAX_PATH+1];
   {
      buffer[0] = 0;
      GetDlgItemText(IDC_EDIT_UW1_PATH, buffer, MAX_PATH);

      // add slash when needed
      unsigned int len = _tcslen(buffer);
      if (len>0 && buffer[len-1] != _T('\\') && buffer[len-1] != _T('/'))
         _tcscat(buffer, _T("\\"));
   }

#ifndef BIF_USENEWUI
#define BIF_USENEWUI 0x0050
#endif

   // setup struct
   BROWSEINFO bi;
   ZeroMemory(&bi, sizeof(bi));
   bi.lpszTitle = _T("Please select the Ultima Underworld I folder:");
   bi.pszDisplayName = buffer;
   bi.pidlRoot = NULL;
   bi.lpfn = ua_browse_callback;
   bi.lParam = (LPARAM)buffer;
   bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;

   // browse
   LPITEMIDLIST lpIL = ::SHBrowseForFolder(&bi);

   // retrieve path
   if (lpIL!=NULL)
      ::SHGetPathFromIDList(lpIL, buffer);

   // set path in edit box
   {
      USES_CONVERSION;
      std::string uw1_path(T2CA(buffer));
      if (uw1_path.size()==0) return 0;

      char last = uw1_path.at(uw1_path.size()-1);

      if (last != '\\' && last != '/')
         uw1_path.append("\\");

      SetDlgItemText(IDC_EDIT_UW1_PATH, A2CT(uw1_path.c_str()));
   }

   return 0;
}

void ua_config_dlg::MessageBox(LPCTSTR pszText, UINT nType)
{
   // get window caption
   _TCHAR szCaption[256];
   GetWindowText(szCaption, 256);

   AtlMessageBox(m_hWnd, pszText, szCaption, nType);
}


void ua_config_dlg::load_config()
{
   // try loading settings
   settings_filename = ua_get_home_path();
   settings_filename += "uwadv.cfg";
   try
   {
      settings.load(settings_filename.c_str());
   }
   catch(...)
   {
      // loading failed
      std::string text = "Could not find file \"";
      text += settings_filename;
      text += "\"!";

      USES_CONVERSION;
      MessageBox(A2CT(text.c_str()), MB_OK | MB_ICONSTOP);

      // exit program
      PostMessage(WM_QUIT);
      return;
   }

   std::string text;
   USES_CONVERSION;

   // general settings

   // sets uw1 path
   text = settings.get_string(ua_setting_uw1_path);
   SetDlgItemText(IDC_EDIT_UW1_PATH, A2CT(text.c_str()));

   // set cutscene narration option
   text = settings.get_string(ua_setting_cuts_narration);
   SendDlgItemMessage(IDC_COMBO_CUTS_NARRATION, CB_SETCURSEL,
      (text.compare("both")==0 ? 2 : text.compare("sound")==0 ? 0 : 1));

   // set "uwadv features" check
   SendDlgItemMessage(IDC_CHECK_ENABLE_FEATURES, BM_SETCHECK,
      settings.get_bool(ua_setting_uwadv_features) ? BST_CHECKED : BST_UNCHECKED);

   // graphics settings

   // set screen resolution text
   text = settings.get_string(ua_setting_screen_resolution);

   // check if we already have that one
   int item = SendDlgItemMessageA(IDC_COMBO_SCREEN_RESOLUTION,
      CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)(A2CT(text.c_str())));

   if (item == CB_ERR)
   {
      // add current resolution value
      SendDlgItemMessage(IDC_COMBO_SCREEN_RESOLUTION,
         CB_INSERTSTRING, 0, (LPARAM)(A2CT(text.c_str())));
      item = 0;
   }
   // set selection
   SendDlgItemMessage(IDC_COMBO_SCREEN_RESOLUTION, CB_SETCURSEL, item);

   // set "fullscreen" check
   SendDlgItemMessage(IDC_CHECK_FULLSCREEN, BM_SETCHECK,
      settings.get_bool(ua_setting_fullscreen) ? BST_CHECKED : BST_UNCHECKED);

   // set "smooth ui" check
   SendDlgItemMessage(IDC_CHECK_SMOOTHUI, BM_SETCHECK,
      settings.get_bool(ua_setting_ui_smooth) ? BST_CHECKED : BST_UNCHECKED);

   // audio settings

   // set "audio enabled" check
   SendDlgItemMessage(IDC_CHECK_ENABLE_AUDIO, BM_SETCHECK,
      settings.get_bool(ua_setting_audio_enabled) ? BST_CHECKED : BST_UNCHECKED);

   // set midi device
   SendDlgItemMessage(IDC_COMBO_MIDI_DEVICE, CB_SETCURSEL,
      (WPARAM)(settings.get_int(ua_setting_win32_midi_device)+1), 0);
}

//! checks if a file with given filename is available
bool ua_file_isavail(const char* base, const char* fname)
{
   std::string filename(base);
   filename += fname;
   return ua_file_exists(filename.c_str());
}

bool ua_config_dlg::check_config()
{
   _TCHAR szBuffer[MAX_PATH];

   // check uw1 path
   {
      GetDlgItemText(IDC_EDIT_UW1_PATH, szBuffer, MAX_PATH);

      std::string uw1_path(T2CA(szBuffer));
      bool uw1_avail =
         ua_file_isavail(uw1_path.c_str(), "data/cnv.ark") &&
         ua_file_isavail(uw1_path.c_str(), "data/strings.pak") &&
         ua_file_isavail(uw1_path.c_str(), "data/pals.dat") &&
         ua_file_isavail(uw1_path.c_str(), "data/allpals.dat") &&
        (ua_file_isavail(uw1_path.c_str(), "uw.exe") ||
         ua_file_isavail(uw1_path.c_str(), "uwdemo.exe"));

      if (!uw1_avail)
      {
         MessageBox(_T("Couldn't find Ultima Underworld I game files in specified folder!"),
            MB_OK | MB_ICONEXCLAMATION);
         return false;
      }
   }

   // TODO: check uw2 path

   // check screen resolution
   {
      GetDlgItemText(IDC_COMBO_SCREEN_RESOLUTION, szBuffer, MAX_PATH);
      std::string screen_res(T2CA(szBuffer));

      // parse resolution string, format is <xres> x <yres>
      std::string::size_type pos = screen_res.find('x');
      if (pos == std::string::npos)
      {
         MessageBox(_T("Screen resolution is not in format <xres> x <yres>!"),
            MB_OK | MB_ICONEXCLAMATION);
         return false;
      }

      // check if fullscreen mode was checked
      if (BST_CHECKED == SendDlgItemMessage(IDC_CHECK_FULLSCREEN, BM_GETCHECK))
      {
         unsigned int width = static_cast<unsigned int>( strtol(screen_res.c_str(), NULL, 10) );
         unsigned int height = static_cast<unsigned int>( strtol(screen_res.c_str() + pos + 1, NULL, 10) );

         // check all display settings if screen resolution is available
         bool found = false;
         {
            DWORD iModeNum=0;
            DEVMODE devmode;
            devmode.dmSize = sizeof(DEVMODE);
            devmode.dmDriverExtra = 0;

            while(0 != ::EnumDisplaySettings(NULL, iModeNum++, &devmode))
            {
               if (devmode.dmBitsPerPel<16) continue;

               if (devmode.dmPelsWidth == width && devmode.dmPelsHeight == height)
               {
                  // mode found
                  found = true;
                  break;
               }
            }
         }

         if (!found)
         {
            MessageBox(_T("Selected screen resolution is not available in fullscreen mode!"),
               MB_OK | MB_ICONEXCLAMATION);
            return false;
         }
      }
   }

   return true;
}

void ua_config_dlg::save_config()
{
   _TCHAR szBuffer[MAX_PATH];
   USES_CONVERSION;

   std::string value;
   int sel;

   // get uw1 path
   GetDlgItemText(IDC_EDIT_UW1_PATH, szBuffer, MAX_PATH);
   value.assign(T2CA(szBuffer));
   settings.set_value(ua_setting_uw1_path, value);

   // get uw2 path
/*
   GetDlgItemText(IDC_EDIT_UW2_PATH, szBuffer, MAX_PATH);
   value.assign(T2CA(buffer));
   settings.set_value(ua_setting_uw2_path, value);
*/

   // get cutscene narration type
   sel = SendDlgItemMessage(IDC_COMBO_CUTS_NARRATION, CB_GETCURSEL);
   value = (sel==0 ? "sound" : (sel==1 ? "subtitles" : "both"));
   settings.set_value(ua_setting_cuts_narration, value);

   // get "uwadv features" check
   sel = SendDlgItemMessage(IDC_CHECK_ENABLE_FEATURES, BM_GETCHECK);
   settings.set_value(ua_setting_uwadv_features, bool(sel==BST_CHECKED));

   // get "screen resolution" text
   GetDlgItemText(IDC_COMBO_SCREEN_RESOLUTION, szBuffer, MAX_PATH);
   value.assign(T2CA(szBuffer));
   settings.set_value(ua_setting_screen_resolution, value);

   // get "fullscreen" check
   sel = SendDlgItemMessage(IDC_CHECK_FULLSCREEN, BM_GETCHECK);
   settings.set_value(ua_setting_fullscreen, bool(sel==BST_CHECKED));

   // get "smooth ui" check
   sel = SendDlgItemMessage(IDC_CHECK_SMOOTHUI, BM_GETCHECK);
   settings.set_value(ua_setting_ui_smooth, bool(sel==BST_CHECKED));

   // get "audio enabled" check
   sel = SendDlgItemMessage(IDC_CHECK_ENABLE_AUDIO, BM_GETCHECK);
   settings.set_value(ua_setting_audio_enabled, bool(sel==BST_CHECKED));

   // get midi device
   sel = SendDlgItemMessage(IDC_COMBO_MIDI_DEVICE, CB_GETCURSEL);
   settings.set_value(ua_setting_win32_midi_device, sel-1);

   // write config file
   std::string settings_filename_backup(settings_filename);
   settings_filename_backup += ".bak";

   rename(settings_filename.c_str(), settings_filename_backup.c_str());
   settings.write(settings_filename_backup.c_str(), settings_filename.c_str());
   remove(settings_filename_backup.c_str());
}


CAppModule _Module;

// main function

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/,
   LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
   // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
   ::DefWindowProc(NULL, 0, 0, 0L);

   AtlInitCommonControls(ICC_WIN95_CLASSES);

   HRESULT hRes = _Module.Init(NULL, hInstance);
   ATLASSERT(SUCCEEDED(hRes));

   // run dialog
   ua_config_dlg cfg;
   cfg.RunDialog();

   _Module.Term();

   return 0;
}
