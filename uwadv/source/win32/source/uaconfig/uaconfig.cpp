/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

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

   \brief underworld adventures win32 config program

   dialog message system is built using ATL-like message handling macros.
   messages are passed from win32 to the StartDialogProc() which passes
   the message on to ua_config_prog::DialogProc(). therefore there can only
   be one ua_config_prog running (current dialog is stored in
   ua_config_prog::current_dlg). finally the message is processed by
   ProcessWindowMessage() that is defined with the message map macros.

*/

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
#include <windows.h>
#include <mmsystem.h> // for midiOutGet*
#include <shlobj.h> // for SHBrowseForFolder

// needed includes
#include "uaconfig.hpp"
#include <string>


// constants

const UINT ua_config_prog::dialog_id = IDD_DIALOG_UACONFIG;


// ua_config_prog methods

ua_config_prog::ua_config_prog(HINSTANCE hInst)
:m_hWnd(NULL),m_hInstance(hInst)
{
}

void ua_config_prog::RunDialog()
{
   // create the dialog
   current_dlg = this;
   HWND hWnd = ::CreateDialogParam(m_hInstance,
      MAKEINTRESOURCE(dialog_id), NULL,
      (DLGPROC)StartDialogProc, 0);
   m_hWnd = hWnd;

   // runs the message loop of the modeless dialog
   MSG msg;

   for(;;)
   {
      // retrieve next message
      BOOL bRet = ::GetMessage(&msg, NULL, 0, 0);

      // quit message loop on WM_QUIT
      if (!bRet) break;

      // deliver message
      if(!PreTranslateMessage(&msg) && !::IsDialogMessage(m_hWnd,&msg))
      {
         ::TranslateMessage(&msg);
         ::DispatchMessage(&msg);
      }
   }

   // destroy the dialog window
   ::DestroyWindow(m_hWnd);
   current_dlg = NULL;
}

ua_config_prog *ua_config_prog::current_dlg = NULL;

LRESULT CALLBACK ua_config_prog::StartDialogProc(HWND hWnd, UINT uMsg,
   WPARAM wParam, LPARAM lParam)
{
   return current_dlg->DialogProc(hWnd,uMsg,wParam,lParam);
}

LRESULT CALLBACK ua_config_prog::DialogProc(HWND hWnd, UINT uMsg,
   WPARAM wParam, LPARAM lParam)
{
   if (m_hWnd==NULL) m_hWnd = hWnd;

   LONG lResult=0;
   ProcessWindowMessage(hWnd,uMsg,wParam,lParam,lResult);
   return lResult;
}

BOOL ua_config_prog::PreTranslateMessage(MSG* pMsg)
{
   return FALSE;
}


// message handler methods

LRESULT ua_config_prog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   // center main window
   {
      HWND parent = ::GetDesktopWindow();

      RECT bigger,ours;
      ::GetWindowRect(parent,&bigger);
      ::GetWindowRect(m_hWnd,&ours);

      UINT xLeft = ((bigger.right-bigger.left)-(ours.right-ours.left))/2;
      UINT yTop = ((bigger.bottom-bigger.top)-(ours.bottom-ours.top))/2;

      ::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1,
         SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
   }

   // load icons
   wndicon = ::LoadIcon(m_hInstance,MAKEINTRESOURCE(IDI_ICON_UACONFIG));
   wndicon_small = (HICON)::LoadImage(m_hInstance,
      MAKEINTRESOURCE(IDI_ICON_UACONFIG),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);

   // set icon
   ::SendMessage(m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)wndicon);
   ::SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)wndicon_small);
   ::ShowWindow(m_hWnd,SW_SHOW);

   // get window caption
   {
      char buffer[256];
      ::GetWindowText(m_hWnd,buffer,256);
      caption.assign(buffer);
   }

   // cutscene narration combobox
   ::SendDlgItemMessage(m_hWnd,IDC_COMBO_CUTS_NARRATION,CB_ADDSTRING,0,(LPARAM)"Sound");
   ::SendDlgItemMessage(m_hWnd,IDC_COMBO_CUTS_NARRATION,CB_ADDSTRING,0,(LPARAM)"Subtitles");
   ::SendDlgItemMessage(m_hWnd,IDC_COMBO_CUTS_NARRATION,CB_ADDSTRING,0,(LPARAM)"Sound+Subtitles");

   // add all available resolutions to screen resolution combo box
   {
      DWORD iModeNum=0;
      DEVMODE devmode;
      devmode.dmSize = sizeof(DEVMODE);
      devmode.dmDriverExtra = 0;

      while(0 != ::EnumDisplaySettings(NULL,iModeNum++,&devmode))
      {
         // only add hi-/truecolor modes
         if (devmode.dmBitsPerPel<16) continue;

         char buffer[32];
         sprintf(buffer,"%u x %u",devmode.dmPelsWidth,devmode.dmPelsHeight);

         // check if we already have that one
         if (CB_ERR == ::SendDlgItemMessage(m_hWnd,IDC_COMBO_SCREEN_RESOLUTION,
            CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)buffer))
         {
            // add it
            ::SendDlgItemMessage(m_hWnd,IDC_COMBO_SCREEN_RESOLUTION,CB_ADDSTRING,
               0,(LPARAM)buffer);
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
         ::SendDlgItemMessage(m_hWnd,IDC_COMBO_MIDI_DEVICE,CB_ADDSTRING,
            0,(LPARAM)caps.szPname);
      }
   }

   // load configuration
   load_config();

   return 1;  // let the system set the focus
}

static int CALLBACK WINAPI ua_browse_callback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
   // set initial path on initialisation
   if (uMsg == BFFM_INITIALIZED)
      ::SendMessage(hwnd,BFFM_SETSELECTION,(WPARAM)TRUE,lpData);
   return 0;
}

LRESULT ua_config_prog::OnSetUw1Path(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
   char buffer[MAX_PATH];
   {
      buffer[0] = 0;
      ::GetDlgItemText(m_hWnd,IDC_EDIT_UW1_PATH,buffer,MAX_PATH);

      // remove slash; 
      unsigned int len = strlen(buffer);
      if (len>0 && (buffer[len-1]=='\\' || buffer[len-1]=='/'))
         buffer[len-1]=0;
   }

   // setup struct
   BROWSEINFO bi = {0};
   bi.lpszTitle = "Please select the Ultima Underworld I folder:";
   bi.pszDisplayName = buffer;
   bi.pidlRoot = NULL;
   bi.lpfn = ua_browse_callback;
   bi.lParam = (LPARAM)buffer;

   // browse
   LPITEMIDLIST lpIL = ::SHBrowseForFolder(&bi);

   // retrieve path
   if (lpIL!=NULL)
      ::SHGetPathFromIDList(lpIL, buffer);

   // set path in edit box
   {
      std::string uw1_path(buffer);
      if (uw1_path.size()==0) return 0;

      char last = uw1_path.at(uw1_path.size()-1);

      if (last!='\\' && last != '/')
         uw1_path.append("\\");

      ::SetDlgItemText(m_hWnd,IDC_EDIT_UW1_PATH,uw1_path.c_str());
   }

   return 0;
}


void ua_config_prog::load_config()
{
   // try loading settings
   try
   {
      settings.load("./uwadv.cfg");
   }
   catch(...)
   {
      // loading failed
      ::MessageBox(m_hWnd,"Could not find file \"uwadv.cfg\" in current folder!",
         caption.c_str(),MB_OK|MB_ICONSTOP);

      // exit program
      ::PostMessage(m_hWnd,WM_QUIT,0,0);
      return;
   }

   std::string text;

   // sets uw1 path
   text = settings.get_string(ua_setting_uw1_path);
   ::SetDlgItemText(m_hWnd,IDC_EDIT_UW1_PATH,text.c_str());

   // set cutscene narration option
   text = settings.get_string(ua_setting_cuts_narration);
   ::SendDlgItemMessage(m_hWnd,IDC_COMBO_CUTS_NARRATION,CB_SETCURSEL,
      (text.compare("both")==0 ? 2 : text.compare("sound")==0 ? 0 : 1),
      0);

   // set screen resolution text
   text = settings.get_string(ua_setting_screen_resolution);

   // check if we already have that one
   int item = ::SendDlgItemMessage(m_hWnd,IDC_COMBO_SCREEN_RESOLUTION,
      CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)text.c_str());

   if (item == CB_ERR)
   {
      // add current resolution value
      ::SendDlgItemMessage(m_hWnd,IDC_COMBO_SCREEN_RESOLUTION,
         CB_INSERTSTRING,0,(LPARAM)text.c_str());
      item = 0;
   }
   // set selection
   ::SendDlgItemMessage(m_hWnd,IDC_COMBO_SCREEN_RESOLUTION,CB_SETCURSEL,item,0);

   // set "fullscreen" check
   ::SendDlgItemMessage(m_hWnd,IDC_CHECK_FULLSCREEN,BM_SETCHECK,
      settings.get_bool(ua_setting_fullscreen) ? BST_CHECKED : BST_UNCHECKED, 0);

   // set "audio enabled" check
   ::SendDlgItemMessage(m_hWnd,IDC_CHECK_ENABLE_AUDIO,BM_SETCHECK,
      settings.get_bool(ua_setting_audio_enabled) ? BST_CHECKED : BST_UNCHECKED, 0);

   // set midi device
   ::SendDlgItemMessage(m_hWnd,IDC_COMBO_MIDI_DEVICE,CB_SETCURSEL,
      (WPARAM)(settings.get_int(ua_setting_win32_midi_device)+1),0);
}

//! checks if a file with given filename is available
bool ua_file_isavail(const char *base, const char *fname)
{
   std::string filename(base);
   filename += fname;

   FILE *fd = fopen(filename.c_str(),"rb");
   if (fd==NULL)
      return false;

   fclose(fd);
   return true;
}

bool ua_config_prog::check_config()
{
   char buffer[MAX_PATH];

   // check uw1 path
   {
      ::GetDlgItemText(m_hWnd,IDC_EDIT_UW1_PATH,buffer,MAX_PATH);

      std::string uw1_path(buffer);
      bool uw1_avail =
         ua_file_isavail(uw1_path.c_str(),"data/cnv.ark") &&
         ua_file_isavail(uw1_path.c_str(),"data/strings.pak") &&
         ua_file_isavail(uw1_path.c_str(),"data/pals.dat") &&
         ua_file_isavail(uw1_path.c_str(),"data/allpals.dat") &&
         (ua_file_isavail(uw1_path.c_str(),"uw.exe") ||
          ua_file_isavail(uw1_path.c_str(),"uwdemo.exe"));

      if (!uw1_avail)
      {
         ::MessageBox(m_hWnd,"Couldn't find Ultima Underworld I game files in specified folder!",
            caption.c_str(),MB_OK|MB_ICONEXCLAMATION);
         return false;
      }
   }

   // check screen resolution
   {
      ::GetDlgItemText(m_hWnd,IDC_COMBO_SCREEN_RESOLUTION,buffer,MAX_PATH);
      std::string screen_res(buffer);

      // parse resolution string, format is <xres> x <yres>
      std::string::size_type pos = screen_res.find('x');
      if (pos == std::string::npos)
      {
         ::MessageBox(m_hWnd,"Screen resolution is not in format <xres> x <yres>!",
            caption.c_str(),MB_OK|MB_ICONEXCLAMATION);
         return false;
      }

      // check if fullscreen mode was checked
      if (BST_CHECKED == ::SendDlgItemMessage(m_hWnd,IDC_CHECK_FULLSCREEN,BM_GETCHECK,0,0))
      {
         unsigned int width = static_cast<unsigned int>( strtol(screen_res.c_str(),NULL,10) );
         unsigned int height = static_cast<unsigned int>( strtol(screen_res.c_str()+pos+1,NULL,10) );

         // check all display settings if screen resolution is available
         bool found = false;
         {
            DWORD iModeNum=0;
            DEVMODE devmode;
            devmode.dmSize = sizeof(DEVMODE);
            devmode.dmDriverExtra = 0;

            while(0 != ::EnumDisplaySettings(NULL,iModeNum++,&devmode))
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
            ::MessageBox(m_hWnd,"Selected screen resolution is not available in fullscreen mode!",
               caption.c_str(),MB_OK|MB_ICONEXCLAMATION);
            return false;
         }
      }
   }

   return true;
}

void ua_config_prog::save_config()
{
   char buffer[MAX_PATH];
   std::string value;
   int sel;

   // uw1 path
   ::GetDlgItemText(m_hWnd,IDC_EDIT_UW1_PATH,buffer,MAX_PATH);
   value.assign(buffer);
   settings.set_value(ua_setting_uw1_path,value);

   // cutscene narration
   sel = ::SendDlgItemMessage(m_hWnd,IDC_COMBO_CUTS_NARRATION,CB_GETCURSEL,0,0);
   value = (sel==0 ? "sound" : (sel==1 ? "subtitles" : "both"));
   settings.set_value(ua_setting_cuts_narration,value);

   // screen resolution text
   ::GetDlgItemText(m_hWnd,IDC_COMBO_SCREEN_RESOLUTION,buffer,MAX_PATH);
   value.assign(buffer);
   settings.set_value(ua_setting_screen_resolution,value);

   // fullscreen check
   sel = ::SendDlgItemMessage(m_hWnd,IDC_CHECK_FULLSCREEN,BM_GETCHECK,0,0);
   settings.set_value(ua_setting_fullscreen, bool(sel==BST_CHECKED));

   // "audio enabled" check
   sel = ::SendDlgItemMessage(m_hWnd,IDC_CHECK_ENABLE_AUDIO,BM_GETCHECK,0,0);
   settings.set_value(ua_setting_audio_enabled, bool(sel==BST_CHECKED));

   // midi device
   sel = ::SendDlgItemMessage(m_hWnd,IDC_COMBO_MIDI_DEVICE,CB_GETCURSEL,0,0);
   settings.set_value(ua_setting_win32_midi_device, sel-1);

   // write config file
   settings.write("./uwadv.cfg","./uwadv.cfg.new");
   remove("./uwadv.cfg.old");
   rename("./uwadv.cfg","./uwadv.cfg.old");
   rename("./uwadv.cfg.new","./uwadv.cfg");
}


// main function

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
   LPSTR lpCmdLine, int nCmdShow)
{
   // run dialog
   ua_config_prog cfg(hInstance);
   cfg.RunDialog();

   return 0;
}
