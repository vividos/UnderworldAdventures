//
// Underworld Adventures Debugger - a debugger tool for Underworld Adventures
// Copyright (c) 2004,2005,2019 Underworld Adventures Team
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
/// \file uadebug/pch.hpp
/// \brief precompiled header support
//
#pragma once

// change these values to use different versions
#define _WIN32_WINNT   _WIN32_WINNT_WIN7

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif

// ATL includes
#include <atlbase.h>

// runtime-check safe macros for COLORREF
#undef GetRValue
#undef GetGValue
#undef GetBValue
#define GetRValue(rgb)      ((BYTE)(rgb&0xFF))
#define GetGValue(rgb)      ((BYTE)((((WORD)(rgb&0xFFFF)) >> 8)&0xFF))
#define GetBValue(rgb)      ((BYTE)(((rgb)>>16)&0xFF))

#include <atlcoll.h>
#include <atlstr.h>

// WTL includes
#define _WTL_NO_CSTRING
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>
#include <atlmisc.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlddx.h>
#include <atlctrlw.h>
#include <atlcrack.h>
#include <atlribbon.h>

// Tabbed Framework includes
#pragma warning(push)
#pragma warning(disable: 4838) // conversion from 'int' to 'UINT_PTR' requires a narrowing conversion
#if (_MSC_VER < 1300)
#  define _TABBEDMDI_MESSAGES_EXTERN_REGISTER
#endif
#include <atlgdix.h>
#include <TabbedMDI.h>
#pragma warning(pop)

// Docking Framework includes
#pragma warning(push)
#pragma warning(disable: 4100) // 'hWnd': unreferenced formal parameter)
#pragma warning(disable: 4244) // '=' : conversion from 'BOOL' to 'ATOM', possible loss of data
#pragma warning(disable: 4456) // declaration of 'bRes' hides previous local declaration
#include <DockMisc.h>
#include <ExtDockingWindow.h>
#include <DockingFrame.h>
#include <DotNetTabCtrl.h>
#include <CustomTabCtrl.h>
#include <TabbedFrame.h>
#include <TabbedDockingWindow.h>
#include <DockingBox.h>
#include <TabDockingBox.h>
#include <VC7LikeCaption.h>
#pragma warning(pop)

// Scintilla wrapper includes
#include <atlscintilla.h>
#include <SciLexer.h>

// Underworld Adventures includes
#include "common.hpp"
#include "game/DebugServer.hpp"


// array length macro
#define ARRAYLENGTH(arr) (sizeof(arr) / sizeof((arr)[0]) )


/// check macro to check if the REFLECT_NOTIFICATIONS() macro was added to the parent class
#define ATLASSERT_ADDED_REFLECT_NOTIFICATIONS() CheckAddedReflectNotifications(hWnd, uMsg, wParam, lParam);

/// helper function to check if the REFLECT_NOTIFICATIONS() macro was added to the parent class
void CheckAddedReflectNotifications(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


/// macro to route individual command messages to member variables
#define COMMAND_ROUTE_TO_MEMBER(uCmd, m_Member) \
   if (uMsg == WM_COMMAND && LOWORD(wParam) == uCmd) \
      CHAIN_MSG_MAP_MEMBER(m_Member)


// project-specific often-used includes
#include "Filename.hpp"
#include "DebugWindowBase.hpp"
#include "DockingWindowBase.hpp"
#include "ChildWindowBase.hpp"
#include "EditListViewCtrl.hpp"
