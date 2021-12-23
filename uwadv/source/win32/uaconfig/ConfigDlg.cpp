//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2019,2021 Underworld Adventures Team
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
/// \file ConfigDlg.cpp
/// \brief Config dialog
//
#include "ConfigDlg.hpp"
#include "FileSystem.hpp"
#include "Base.hpp"
#include "Settings.hpp"
#include "ResourceManager.hpp"
#include <string>
#include <atlstr.h>
#include <atlconv.h>
#include <mmsystem.h> // for midiOutGet*
#include <ShlObj.h> // for SHBrowseForFolder
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
      BOOL ret = ::GetMessage(&msg, NULL, 0, 0);

      if (!ret)
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
   SendDlgItemMessage(IDC_COMBO_CUTS_NARRATION, CB_ADDSTRING, 0, (LPARAM)_T("Sound + Subtitles"));

   // add all available resolutions to screen resolution combo box
   {
      DWORD modeNum = 0;
      DEVMODE devmode = {};
      devmode.dmSize = sizeof(DEVMODE);
      devmode.dmDriverExtra = 0;

      while (0 != ::EnumDisplaySettings(NULL, modeNum++, &devmode))
      {
         // only add hi-/truecolor modes
         if (devmode.dmBitsPerPel < 16)
            continue;

         CString buffer;
         buffer.Format(_T("%u x %u"), devmode.dmPelsWidth, devmode.dmPelsHeight);

         // check if we already have that one
         if (CB_ERR == SendDlgItemMessage(
            IDC_COMBO_SCREEN_RESOLUTION,
            CB_FINDSTRINGEXACT,
            (WPARAM)-1,
            (LPARAM)buffer.GetString()))
         {
            // add it
            SendDlgItemMessage(
               IDC_COMBO_SCREEN_RESOLUTION,
               CB_ADDSTRING,
               0,
               (LPARAM)buffer.GetString());
         }
      }
   }

   // add all midi devices to combo box
   {
      MIDIOUTCAPS caps = {};
      UINT max = midiOutGetNumDevs();

      for (signed int index = -1; index < (signed)max; index++)
      {
         midiOutGetDevCaps((UINT)index, &caps, sizeof(caps));

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
   CString buffer;
   GetDlgItemText(IDC_EDIT_UW1_PATH, buffer);

   if (buffer.Right(1) != _T("\\"))
      buffer += _T("\\");

   // setup struct
   BROWSEINFO browseInfo = {};
   ZeroMemory(&browseInfo, sizeof(browseInfo));
   browseInfo.lpszTitle = _T("Please select the Ultima Underworld I folder:");
   browseInfo.pszDisplayName = const_cast<LPTSTR>(buffer.GetString());
   browseInfo.pidlRoot = NULL;
   browseInfo.lpfn = BrowseCallback;
   browseInfo.lParam = (LPARAM)buffer.GetString();
   browseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;

   LPITEMIDLIST lpIL = ::SHBrowseForFolder(&browseInfo);

   if (lpIL != NULL)
   {
      ::SHGetPathFromIDList(lpIL, buffer.GetBuffer(MAX_PATH));
      buffer.ReleaseBuffer();
   }

   if (buffer.IsEmpty())
      return 0;

   if (buffer.Right(1) != _T("\\"))
      buffer += _T("\\");

   SetDlgItemText(IDC_EDIT_UW1_PATH, buffer);

   CheckConfig();

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

   int item = SendDlgItemMessage(IDC_COMBO_SCREEN_RESOLUTION,
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

bool ConfigDlg::CheckConfig()
{
   CString buffer;

   // check uw1 path
   {
      GetDlgItemText(IDC_EDIT_UW1_PATH, buffer);

      if (buffer.Right(1) != _T("\\"))
         buffer += _T("\\");

      std::string uw1path = CT2CA(buffer);

      Base::Settings settings;
      settings.SetValue(Base::SettingsType::settingUnderworldPath, uw1path.c_str());
      settings.SetValue(Base::SettingsType::settingUw1Path, uw1path.c_str());
      Base::ResourceManager resourceManager{ settings };

      if (!resourceManager.DetectGameType(settings) ||
         settings.GetGameType() != Base::gameUw1)
      {
         ShowMessageBox(_T("Couldn't find Ultima Underworld 1 game files in specified folder!"),
            MB_OK | MB_ICONEXCLAMATION);
         return false;
      }
   }

   // check screen resolution
   {
      GetDlgItemText(IDC_COMBO_SCREEN_RESOLUTION, buffer);
      std::string screen_res = CT2CA(buffer);

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
            DWORD modeNum = 0;
            DEVMODE devmode = {};
            devmode.dmSize = sizeof(DEVMODE);
            devmode.dmDriverExtra = 0;

            while (0 != ::EnumDisplaySettings(NULL, modeNum++, &devmode))
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
   CString buffer;
   GetDlgItemText(IDC_EDIT_UW1_PATH, buffer);

   if (buffer.Right(1) != _T("\\"))
      buffer += _T("\\");

   std::string value = CT2CA(buffer);
   m_settings.SetValue(Base::settingUw1Path, value);

   //GetDlgItemText(IDC_EDIT_UW2_PATH, buffer);
   //value = CT2CA(buffer);
   //m_settings.SetValue(Base::settingUw2Path, value);

   int sel = SendDlgItemMessage(IDC_COMBO_CUTS_NARRATION, CB_GETCURSEL);
   value = (sel == 0 ? "sound" : (sel == 1 ? "subtitles" : "both"));
   m_settings.SetValue(Base::settingCutsceneNarration, value);

   sel = SendDlgItemMessage(IDC_CHECK_ENABLE_FEATURES, BM_GETCHECK);
   m_settings.SetValue(Base::settingUwadvFeatures, bool(sel == BST_CHECKED));

   GetDlgItemText(IDC_COMBO_SCREEN_RESOLUTION, buffer);
   value = CT2CA(buffer);
   m_settings.SetValue(Base::settingScreenResolution, value);

   sel = SendDlgItemMessage(IDC_CHECK_FULLSCREEN, BM_GETCHECK);
   m_settings.SetValue(Base::settingFullscreen, bool(sel == BST_CHECKED));

   sel = SendDlgItemMessage(IDC_CHECK_SMOOTHUI, BM_GETCHECK);
   m_settings.SetValue(Base::settingUISmooth, bool(sel == BST_CHECKED));

   sel = SendDlgItemMessage(IDC_CHECK_ENABLE_AUDIO, BM_GETCHECK);
   m_settings.SetValue(Base::settingAudioEnabled, bool(sel == BST_CHECKED));

   sel = SendDlgItemMessage(IDC_COMBO_MIDI_DEVICE, CB_GETCURSEL);
   m_settings.SetValue(Base::settingWin32MidiDevice, sel - 1);

   std::string settingsFilenameBackup(m_settingsFilename);
   settingsFilenameBackup += ".bak";

   rename(m_settingsFilename.c_str(), settingsFilenameBackup.c_str());
   m_settings.Save(m_settingsFilename.c_str(), settingsFilenameBackup.c_str());
   remove(settingsFilenameBackup.c_str());
}
