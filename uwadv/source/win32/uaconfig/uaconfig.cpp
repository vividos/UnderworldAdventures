//
// Underworld Adventures - an Ultima Underworld remake project
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
/// \file uaconfig.cpp
/// \brief Underworld Adventures win32 config application
//
#include "common.hpp"
#include "uaconfig.hpp"
#include "FileSystem.hpp"
#include <string>
#include <atlconv.h>
#include <mmsystem.h> // for midiOutGet*
#include <shlobj.h> // for SHBrowseForFolder
#include <tchar.h> // for _sntprintf

#ifndef BIF_USENEWUI
#define BIF_USENEWUI 0x0050
#endif

void ConfigDlg::RunDialog()
{
   Create(NULL);
   ShowWindow(SW_SHOW);

   // run the message loop of the modeless dialog
   MSG msg;

   for (;;)
   {
      BOOL bRet = ::GetMessage(&msg, NULL, 0, 0);

      if (!bRet)
         break;

      if (!PreTranslateMessage(&msg) && !IsDialogMessage(&msg))
      {
         ::TranslateMessage(&msg);
         ::DispatchMessage(&msg);
      }
   }

   DestroyWindow();
}

LRESULT ConfigDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   CenterWindow();

   HICON icon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_ICON_UACONFIG),
      IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
   SetIcon(icon, TRUE);

   HICON iconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_ICON_UACONFIG),
      IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
   SetIcon(iconSmall, FALSE);

   // cutscene narration combobox
   SendDlgItemMessage(IDC_COMBO_CUTS_NARRATION, CB_ADDSTRING, 0, (LPARAM)_T("Sound"));
   SendDlgItemMessage(IDC_COMBO_CUTS_NARRATION, CB_ADDSTRING, 0, (LPARAM)_T("Subtitles"));
   SendDlgItemMessage(IDC_COMBO_CUTS_NARRATION, CB_ADDSTRING, 0, (LPARAM)_T("Sound+Subtitles"));

   // add all available resolutions to screen resolution combo box
   {
      DWORD iModeNum = 0;
      DEVMODE devmode;
      devmode.dmSize = sizeof(DEVMODE);
      devmode.dmDriverExtra = 0;

      while (0 != ::EnumDisplaySettings(NULL, iModeNum++, &devmode))
      {
         // only add hi-/truecolor modes
         if (devmode.dmBitsPerPel < 16) continue;

         _TCHAR buffer[32];
         _sntprintf_s(buffer, sizeof(buffer), sizeof(buffer), _T("%u x %u"), devmode.dmPelsWidth, devmode.dmPelsHeight);

         // check if we already have that one
         if (CB_ERR == SendDlgItemMessage(IDC_COMBO_SCREEN_RESOLUTION,
            CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)buffer))
         {
            // add it
            SendDlgItemMessage(IDC_COMBO_SCREEN_RESOLUTION, CB_ADDSTRING,
               0, (LPARAM)buffer);
         }
      }
   }

   // add all midi devices to combo box
   {
      MIDIOUTCAPS caps;
      UINT max = midiOutGetNumDevs();

      for (signed int n = -1; n < (signed)max; n++)
      {
         midiOutGetDevCaps((UINT)n, &caps, sizeof(caps));

         SendDlgItemMessage(IDC_COMBO_MIDI_DEVICE, CB_ADDSTRING,
            0, (LPARAM)caps.szPname);
      }
   }

   LoadConfig();

   m_tooltips.Init(m_hWnd);

   return 1; // let the system set the focus
}

static int CALLBACK WINAPI BrowseCallback(HWND hwnd, UINT uMsg, LPARAM /*lParam*/, LPARAM lpData)
{
   // set initial path on initialisation
   if (uMsg == BFFM_INITIALIZED)
      ::SendMessage(hwnd, BFFM_SETSELECTION, (WPARAM)TRUE, lpData);
   return 0;
}

LRESULT ConfigDlg::OnSetUw1Path(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   _TCHAR buffer[MAX_PATH + 1];
   {
      buffer[0] = 0;
      GetDlgItemText(IDC_EDIT_UW1_PATH, buffer, MAX_PATH);

      // add slash when needed
      size_t len = _tcslen(buffer);
      if (len > 0 && buffer[len - 1] != _T('\\') && buffer[len - 1] != _T('/'))
         _tcscat_s(buffer, _T("\\"));
   }

   // setup struct
   BROWSEINFO browseInfo;
   ZeroMemory(&browseInfo, sizeof(browseInfo));
   browseInfo.lpszTitle = _T("Please select the Ultima Underworld I folder:");
   browseInfo.pszDisplayName = buffer;
   browseInfo.pidlRoot = NULL;
   browseInfo.lpfn = BrowseCallback;
   browseInfo.lParam = (LPARAM)buffer;
   browseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;

   LPITEMIDLIST lpIL = ::SHBrowseForFolder(&browseInfo);

   if (lpIL != NULL)
      ::SHGetPathFromIDList(lpIL, buffer);

   std::string uw1path = CT2CA(buffer);
   if (uw1path.empty())
      return 0;

   char last = uw1path.at(uw1path.size() - 1);

   if (last != '\\' && last != '/')
      uw1path.append("\\");

   SetDlgItemText(IDC_EDIT_UW1_PATH, CA2CT(uw1path.c_str()));

   return 0;
}

void ConfigDlg::ShowMessageBox(LPCTSTR text, UINT type)
{
   _TCHAR caption[256];
   GetWindowText(caption, 256);

   MessageBox(text, caption, type);
}

void ConfigDlg::LoadConfig()
{
   m_settingsFilename = Base::FileSystem::GetHomePath();
   m_settingsFilename += "uwadv.cfg";

   try
   {
      m_settings.Load(m_settingsFilename.c_str());
   }
   catch (...)
   {
      // loading failed
      std::string text = "Could not find file \"";
      text += m_settingsFilename;
      text += "\"!";

      ShowMessageBox(CA2CT(text.c_str()), MB_OK | MB_ICONSTOP);

      PostMessage(WM_QUIT);
      return;
   }

   std::string text;

   // general settings

   text = m_settings.GetString(Base::settingUw1Path);
   SetDlgItemText(IDC_EDIT_UW1_PATH, CA2CT(text.c_str()));

   text = m_settings.GetString(Base::settingCutsceneNarration);
   SendDlgItemMessage(IDC_COMBO_CUTS_NARRATION, CB_SETCURSEL,
      (text.compare("both") == 0 ? 2 : text.compare("sound") == 0 ? 0 : 1));

   SendDlgItemMessage(IDC_CHECK_ENABLE_FEATURES, BM_SETCHECK,
      m_settings.GetBool(Base::settingUwadvFeatures) ? BST_CHECKED : BST_UNCHECKED);

   // graphics settings

   text = m_settings.GetString(Base::settingScreenResolution);

   int item = SendDlgItemMessageA(IDC_COMBO_SCREEN_RESOLUTION,
      CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)(LPCTSTR)(CA2CT(text.c_str())));

   if (item == CB_ERR)
   {
      SendDlgItemMessage(IDC_COMBO_SCREEN_RESOLUTION,
         CB_INSERTSTRING, 0, (LPARAM)(LPCTSTR)(CA2CT(text.c_str())));
      item = 0;
   }

   SendDlgItemMessage(IDC_COMBO_SCREEN_RESOLUTION, CB_SETCURSEL, item);

   SendDlgItemMessage(IDC_CHECK_FULLSCREEN, BM_SETCHECK,
      m_settings.GetBool(Base::settingFullscreen) ? BST_CHECKED : BST_UNCHECKED);

   SendDlgItemMessage(IDC_CHECK_SMOOTHUI, BM_SETCHECK,
      m_settings.GetBool(Base::settingUISmooth) ? BST_CHECKED : BST_UNCHECKED);

   // audio settings

   SendDlgItemMessage(IDC_CHECK_ENABLE_AUDIO, BM_SETCHECK,
      m_settings.GetBool(Base::settingAudioEnabled) ? BST_CHECKED : BST_UNCHECKED);

   SendDlgItemMessage(IDC_COMBO_MIDI_DEVICE, CB_SETCURSEL,
      (WPARAM)(m_settings.GetInt(Base::settingWin32MidiDevice) + 1), 0);
}

/// checks if a file with given filename is available
bool IsFileAvailable(const char* base, const char* fname)
{
   std::string filename(base);
   filename += fname;
   return Base::FileSystem::FileExists(filename.c_str());
}

bool ConfigDlg::CheckConfig()
{
   _TCHAR buffer[MAX_PATH];

   // check uw1 path
   {
      GetDlgItemText(IDC_EDIT_UW1_PATH, buffer, MAX_PATH);

      std::string uw1path = CT2CA(buffer);
      bool uw1_avail =
         IsFileAvailable(uw1path.c_str(), "data/cnv.ark") &&
         IsFileAvailable(uw1path.c_str(), "data/strings.pak") &&
         IsFileAvailable(uw1path.c_str(), "data/pals.dat") &&
         IsFileAvailable(uw1path.c_str(), "data/allpals.dat") &&
         (IsFileAvailable(uw1path.c_str(), "uw.exe") ||
            IsFileAvailable(uw1path.c_str(), "uwdemo.exe"));

      if (!uw1_avail)
      {
         ShowMessageBox(_T("Couldn't find Ultima Underworld I game files in specified folder!"),
            MB_OK | MB_ICONEXCLAMATION);
         return false;
      }
   }

   // TODO: check uw2 path

   // check screen resolution
   {
      GetDlgItemText(IDC_COMBO_SCREEN_RESOLUTION, buffer, MAX_PATH);
      std::string screen_res(T2CA(buffer));

      // parse resolution string, format is <xres> x <yres>
      std::string::size_type pos = screen_res.find('x');
      if (pos == std::string::npos)
      {
         ShowMessageBox(_T("Screen resolution is not in format <xres> x <yres>!"),
            MB_OK | MB_ICONEXCLAMATION);
         return false;
      }

      // check if fullscreen mode was checked
      if (BST_CHECKED == SendDlgItemMessage(IDC_CHECK_FULLSCREEN, BM_GETCHECK))
      {
         unsigned int width = static_cast<unsigned int>(strtol(screen_res.c_str(), NULL, 10));
         unsigned int height = static_cast<unsigned int>(strtol(screen_res.c_str() + pos + 1, NULL, 10));

         // check all display settings if screen resolution is available
         bool found = false;
         {
            DWORD iModeNum = 0;
            DEVMODE devmode;
            devmode.dmSize = sizeof(DEVMODE);
            devmode.dmDriverExtra = 0;

            while (0 != ::EnumDisplaySettings(NULL, iModeNum++, &devmode))
            {
               if (devmode.dmBitsPerPel < 16) continue;

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
            ShowMessageBox(_T("Selected screen resolution is not available in fullscreen mode!"),
               MB_OK | MB_ICONEXCLAMATION);
            return false;
         }
      }
   }

   return true;
}

void ConfigDlg::SaveConfig()
{
   _TCHAR buffer[MAX_PATH];

   std::string value;
   int sel;

   GetDlgItemText(IDC_EDIT_UW1_PATH, buffer, MAX_PATH);
   value.assign(CT2CA(buffer));
   m_settings.SetValue(Base::settingUw1Path, value);

   //GetDlgItemText(IDC_EDIT_UW2_PATH, buffer, MAX_PATH);
   //value.assign(CT2CA(buffer));
   //m_settings.SetValue(Base::settingUw2Path, value);

   sel = SendDlgItemMessage(IDC_COMBO_CUTS_NARRATION, CB_GETCURSEL);
   value = (sel == 0 ? "sound" : (sel == 1 ? "subtitles" : "both"));
   m_settings.SetValue(Base::settingCutsceneNarration, value);

   sel = SendDlgItemMessage(IDC_CHECK_ENABLE_FEATURES, BM_GETCHECK);
   m_settings.SetValue(Base::settingUwadvFeatures, bool(sel == BST_CHECKED));

   GetDlgItemText(IDC_COMBO_SCREEN_RESOLUTION, buffer, MAX_PATH);
   value.assign(CT2CA(buffer));
   m_settings.SetValue(Base::settingScreenResolution, value);

   sel = SendDlgItemMessage(IDC_CHECK_FULLSCREEN, BM_GETCHECK);
   m_settings.SetValue(Base::settingFullscreen, bool(sel == BST_CHECKED));

   sel = SendDlgItemMessage(IDC_CHECK_SMOOTHUI, BM_GETCHECK);
   m_settings.SetValue(Base::settingUISmooth, bool(sel == BST_CHECKED));

   sel = SendDlgItemMessage(IDC_CHECK_ENABLE_AUDIO, BM_GETCHECK);
   m_settings.SetValue(Base::settingAudioEnabled, bool(sel == BST_CHECKED));

   sel = SendDlgItemMessage(IDC_COMBO_MIDI_DEVICE, CB_GETCURSEL);
   m_settings.SetValue(Base::settingWin32MidiDevice, sel - 1);

   std::string settings_filename_backup(m_settingsFilename);
   settings_filename_backup += ".bak";

   rename(m_settingsFilename.c_str(), settings_filename_backup.c_str());
   m_settings.Save(m_settingsFilename.c_str(), settings_filename_backup.c_str());
   remove(settings_filename_backup.c_str());
}

CComModule _Module;

/// main function
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/,
   LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
   HRESULT res = _Module.Init(NULL, hInstance);
   ATLASSERT(SUCCEEDED(res)); res;

   ConfigDlg cfg;
   cfg.RunDialog();

   _Module.Term();

   return 0;
}
