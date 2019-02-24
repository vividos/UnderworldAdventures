//
// Underworld Adventures Debugger - a debugger tool for Underworld Adventures
// Copyright (c) 2004,2005,2019 Michael Fink
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
/// \file uadebug.cpp
/// \brief debugger entry point and main application function
//
#include "stdatl.hpp"
#include "Resource.h"
#include "MainFrame.hpp"

CAppModule _Module;

HINSTANCE g_hInstance = 0;

/// runs application
int Run(void* pDebugClient, int nCmdShow = SW_SHOWDEFAULT)
{
   CMainFrame wndMain;
   if (!wndMain.InitDebugClient(pDebugClient))
      return 1;

   CMessageLoop theLoop;
   _Module.AddMessageLoop(&theLoop);

   if (wndMain.CreateEx() == NULL)
   {
      ATLTRACE(_T("Main window creation failed!\n"));
      return 0;
   }

   wndMain.ShowWindow(nCmdShow);

   int nRet = theLoop.Run();

   _Module.RemoveMessageLoop();

   return nRet;
}

/// debugger start function
extern "C"
__declspec(dllexport)
void uadebug_start(void* pDebugClient)
{
   // init all sort of stuff
#ifdef _DEBUG
   ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

   // init COM
   HRESULT hRes = ::CoInitialize(NULL);
   ATLASSERT(SUCCEEDED(hRes));

   // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
   ::DefWindowProc(NULL, 0, 0, 0L);

#if (_ATL_VER >= 0x700) // >= ATL7
   AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);
#else
#if (_WIN32_IE >= 0x0300)
   INITCOMMONCONTROLSEX iccx;
   iccx.dwSize = sizeof(iccx);
   iccx.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES;
   BOOL bRet = ::InitCommonControlsEx(&iccx);
   bRet;
   ATLASSERT(bRet);
#else
   ::InitCommonControls();
#endif
#endif

   DWORD dwMajor = 0, dwMinor = 0;
   ::AtlGetCommCtrlVersion(&dwMajor, &dwMinor);
   ATLTRACE(_T("using common controls version %u.%u\n"), dwMajor, dwMinor);

   hRes = _Module.Init(NULL, g_hInstance/*, &LIBID_ATLLib*/);
   ATLASSERT(SUCCEEDED(hRes));

   // run WTL application
   Run(pDebugClient, SW_SHOW);

   _Module.Term();
   ::CoUninitialize();
}

/// DLL Entry Point
extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
   if (dwReason == DLL_PROCESS_ATTACH)
   {
      // remember instance handle
      g_hInstance = hInstance;
      DisableThreadLibraryCalls(hInstance);
   }

   return TRUE;
}
