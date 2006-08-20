/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2006 Underworld Adventures Team

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
/*! \file crashdump.cpp

   \brief crash dump reporting

*/

// needed includes
#include "base.hpp"
#include "crashdump.hpp"
#include <windows.h> // for EXCEPTION_ACCESS_VIOLATION
#include <tchar.h> // for _sntprintf
#undef _CRT_SECURE_NO_DEPRECATE
#include <cstdio> // for _snprintf
#include <excpt.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

#ifdef HAVE_WIN64
#error Please port to 64-bit!
#endif


int ReportCrashFilter(unsigned int uiCode, struct _EXCEPTION_POINTERS* ep)
{
   UaTrace("\nStart of exception report\n");

/*
   // write minidump
   MINIDUMP_EXCEPTION_INFORMATION mdExceptionInfo;
   mdExceptionInfo.ExceptionPointers = ep;
   mdExceptionInfo.ThreadId = GetCurrentThreadId();
   mdExceptionInfo.ClientPointers = TRUE;

   MINIDUMP_TYPE type =
      MiniDumpWithHandleData;

   ::MiniDumpWriteDump(GetCurrentProcess(),
      ::GetCurrentProcessId(),
      hFile, type, &mdExceptionInfo, NULL, NULL);
*/

   UaTrace("Caught exception 0x%08x at 0x%08x\n",
      uiCode,
      (DWORD_PTR)ep->ExceptionRecord->ExceptionAddress);

   // initialize symbol resolution
   TCHAR szCurrentFolder[MAX_PATH];
   GetCurrentDirectory(MAX_PATH, szCurrentFolder);
   SymInitialize(GetCurrentProcess(), szCurrentFolder, TRUE);

   UaTrace("Stack trace:\n");

   CONTEXT context = *ep->ContextRecord;

   // crate stack frame
   STACKFRAME stackFrame;
   ZeroMemory(&stackFrame, sizeof(stackFrame));

   stackFrame.AddrPC.Mode = AddrModeFlat;
   stackFrame.AddrPC.Offset = context.Eip;

   stackFrame.AddrFrame.Mode = AddrModeFlat;
   stackFrame.AddrFrame.Offset = context.Ebp;

   // set up symbol info buffer and ptr
   ULONG64 ulBuffer[(sizeof(SYMBOL_INFO) + MAX_SYM_NAME*sizeof(TCHAR) + sizeof(ULONG64) - 1) / sizeof(ULONG64)];
   PSYMBOL_INFO pSymbol = reinterpret_cast<PSYMBOL_INFO>(ulBuffer);

   pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
   pSymbol->MaxNameLen = MAX_SYM_NAME;

   bool bFirstSymbol = false;
   BOOL bRetWalk;
   unsigned int uiFrameCount = 0;
   do
   {
      bRetWalk = StackWalk(IMAGE_FILE_MACHINE_I386,
         GetCurrentProcess(),
         GetCurrentThread(),
         &stackFrame,
         &context,
         NULL, NULL, NULL, NULL);

      if (bRetWalk != FALSE)
      {
         // find out function name
         DWORD64 dwFuncDisplacement = 0;
         BOOL bRet = SymFromAddr(GetCurrentProcess(), stackFrame.AddrPC.Offset, &dwFuncDisplacement, pSymbol);

         UaTrace("   frame %u at eip=0x%08x, ebp=0x%08x\n      name=%s, displacement=0x%08x\n",
            uiFrameCount, stackFrame.AddrPC.Offset, stackFrame.AddrFrame.Offset,
            bRet != FALSE ? pSymbol->Name : "unknown",
            dwFuncDisplacement);

         // find out source file and line number
         DWORD dwLineDisplacement = 0;
         IMAGEHLP_LINE line;
         line.SizeOfStruct = sizeof(line);
         bRet = ::SymGetLineFromAddr(GetCurrentProcess(),
            static_cast<DWORD>(stackFrame.AddrPC.Offset), &dwLineDisplacement, &line);

         if (bRet != FALSE)
         {
            UaTrace("      at %s(%u)\n      line displacement=0x%08x\n",
               line.FileName,
               line.LineNumber,
               dwLineDisplacement);
         }

         // also do debug window output
         if (!bFirstSymbol && bRet != FALSE)
         {
            bFirstSymbol = true;

            TCHAR szBuffer[1024];

            _sntprintf(szBuffer, SDL_TABLESIZE(szBuffer),
               _T("%s(%u): caught exception 0x%08x\n"),
               line.FileName,
               line.LineNumber,
               uiCode);

            OutputDebugString(szBuffer);
         }
      }

      uiFrameCount++;

   } while(bRetWalk != FALSE);

   SymCleanup(GetCurrentProcess());

   UaTrace("End of exception report\n\n");

   // pass on C++ exceptions to real handler
   return uiCode == 0xe06d7363 ? EXCEPTION_CONTINUE_SEARCH : EXCEPTION_EXECUTE_HANDLER;
}
