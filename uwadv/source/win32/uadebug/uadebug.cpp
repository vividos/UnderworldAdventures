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

/// runs application
int Run(void* debugClient, int cmdShow = SW_SHOWDEFAULT)
{
   MainFrame mainFrame;
   if (!mainFrame.InitDebugClient(debugClient))
      return 1;

   CMessageLoop theLoop;
   _Module.AddMessageLoop(&theLoop);

   if (mainFrame.CreateEx() == NULL)
   {
      ATLTRACE(_T("Main window creation failed!\n"));
      return 0;
   }

   mainFrame.ShowWindow(cmdShow);

   int nRet = theLoop.Run();

   _Module.RemoveMessageLoop();

   return nRet;
}

/// debugger start function
extern "C"
__declspec(dllexport)
void uadebug_start(void* debugClient)
{
#ifdef _DEBUG
   ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

   AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);

   DWORD major = 0, minor = 0;
   ::AtlGetCommCtrlVersion(&major, &minor);
   ATLTRACE(_T("using common controls version %u.%u\n"), major, minor);

   // run WTL application
   Run(debugClient, SW_SHOW);
}

/// DLL Entry Point
extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
   if (dwReason == DLL_PROCESS_ATTACH)
   {
      DisableThreadLibraryCalls(hInstance);

      HRESULT res = _Module.Init(NULL, hInstance);
      ATLASSERT(SUCCEEDED(res));
      res;
   }
   else if (dwReason == DLL_PROCESS_DETACH)
   {
      _Module.Term();
   }

   return TRUE;
}
