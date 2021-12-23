//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2006,2019,2020 Underworld Adventures Team
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
/// \file CrashReporting.hpp
/// \brief crash dump reporting for win32 platform
//
#include "pch.hpp"
#include "CrashReporting.hpp"
#include <tchar.h> // for _sntprintf, _tcslen
#include <strsafe.h>
#include <ctime>
#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")

#ifdef HAVE_WIN64
#error Please port to 64-bit!
#endif

/// base path for writing minidump file
static TCHAR g_minidumpBasePath[MAX_PATH] = { 0 };

/// creates minidump filename in given buffer
void GetMinidumpFilename(LPTSTR minidumpFilename, UINT numMaxChars)
{
   UaVerify(S_OK == StringCchCopy(
      minidumpFilename,
      numMaxChars,
      g_minidumpBasePath));

   size_t lenBasePath = _tcslen(minidumpFilename);
   TCHAR* start = minidumpFilename + lenBasePath;
   size_t numRemaining = numMaxChars - lenBasePath;

   // add date
   time_t nowt = ::time(&nowt);

   struct tm now = { 0 };
   localtime_s(&now, &nowt);

   _sntprintf_s(start, numRemaining, numRemaining,
      _T("%04i-%02i-%02i %02i_%02i_%02i.mdmp"),
      now.tm_year + 1900,
      now.tm_mon + 1,
      now.tm_mday,
      now.tm_hour,
      now.tm_min,
      now.tm_sec);
}

bool WriteMinidump(LPCTSTR minidumpFilename, _EXCEPTION_POINTERS* exceptionInfo)
{
   // open file
   UaTrace("Writing minidump file %s\n", minidumpFilename);

   HANDLE fileHandle = CreateFile(
      minidumpFilename,
      GENERIC_WRITE,
      0,
      nullptr,
      CREATE_ALWAYS,
      FILE_ATTRIBUTE_NORMAL,
      nullptr);

   if (INVALID_HANDLE_VALUE == fileHandle)
      return false;

   std::shared_ptr<void> file{ fileHandle, &::CloseHandle };

   // write minidump
   MINIDUMP_EXCEPTION_INFORMATION minidumpExceptionInfo = { 0 };
   if (exceptionInfo != nullptr)
   {
      minidumpExceptionInfo.ExceptionPointers = exceptionInfo;
      minidumpExceptionInfo.ThreadId = GetCurrentThreadId();
      minidumpExceptionInfo.ClientPointers = TRUE;
   }

   MINIDUMP_TYPE type =
      MiniDumpWithHandleData;

   ::MiniDumpWriteDump(GetCurrentProcess(),
      ::GetCurrentProcessId(),
      fileHandle, type,
      exceptionInfo != nullptr ? &minidumpExceptionInfo : nullptr,
      nullptr, nullptr);

   return true;
}

/// exception filter function to write minidump file
LONG WINAPI ExceptionFilterWriteMinidump(_EXCEPTION_POINTERS* exceptionInfo)
{
   if (exceptionInfo != nullptr)
   {
      UaTrace("Caught exception 0x%08x at address 0x%08x\n",
         exceptionInfo->ExceptionRecord->ExceptionCode,
         exceptionInfo->ExceptionRecord->ExceptionAddress);
   }

   static TCHAR minidumpFilename[MAX_PATH];
   GetMinidumpFilename(minidumpFilename, sizeof(minidumpFilename) / sizeof(*minidumpFilename));

   if (!WriteMinidump(minidumpFilename, exceptionInfo))
      return EXCEPTION_CONTINUE_SEARCH;

   static char errorMessage[MAX_PATH + 200];
   snprintf(errorMessage, sizeof(errorMessage) / sizeof(*errorMessage),
      "The application has crashed and a crash dump was written: %s", minidumpFilename);

   SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Underworld Adventures", errorMessage, nullptr);

   return EXCEPTION_EXECUTE_HANDLER;
}

/// handler function for std::terminate
void OnStdTerminate()
{
   UaTrace("OnStdTerminate() called!\n");

   ExceptionFilterWriteMinidump(nullptr);
}

void InitCrashReporting(const char* appName)
{
   TCHAR tempPath[MAX_PATH];
   GetTempPath(MAX_PATH, tempPath);

   _sntprintf(g_minidumpBasePath, sizeof(g_minidumpBasePath) / sizeof(*g_minidumpBasePath),
      _T("%scrashdump-%hs-"), tempPath, appName);

   // set exception filter
   SetUnhandledExceptionFilter(&ExceptionFilterWriteMinidump);

   // catch all std::terminate() calls
   std::set_terminate(&OnStdTerminate);
}
