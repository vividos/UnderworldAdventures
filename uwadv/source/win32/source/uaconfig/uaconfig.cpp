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
   bi.lpszTitle = "select Ultima Underworld I folder:";
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
   settings.load("./uwadv.cfg");

   std::string uw1_path(settings.get_string(ua_setting_uw1_path));
   ::SetDlgItemText(m_hWnd,IDC_EDIT_UW1_PATH,uw1_path.c_str());
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
         ::MessageBox(m_hWnd,"couldn't find Ultima Underworld I game files!",
            "Underworld Adventures Configuration",MB_OK);
         return false;
      }
   }

   return true;
}

void ua_config_prog::save_config()
{
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
