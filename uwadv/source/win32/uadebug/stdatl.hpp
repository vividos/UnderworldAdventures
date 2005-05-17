/*
   Underworld Adventures Debugger - a debugger tool for Underworld Adventures
   Copyright (c) 2004,2005 Michael Fink

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
/*! \file stdatl.hpp

   \brief precompiled header support

*/
//! \ingroup uadebug

//@{

// include guard
#pragma once

// change these values to use different versions
#define WINVER      0x0400
#define _WIN32_IE   0x0400
#define _RICHEDIT_VER   0x0100

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif

// ATL includes
#include <atlbase.h>
#if (_ATL_VER >= 0x700) // >= ATL 7
#define _WTL_NO_CSTRING
#include <atlcoll.h>
#include <atlstr.h>
#endif
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>

// WTL includes
#include <atlmisc.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlcrack.h>

// Tabbed Framework includes
#define _TABBEDMDI_MESSAGES_EXTERN_REGISTER
#include <atlgdix.h>
#include <TabbedMDI.h>

// Docking Framework includes
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

// Menu XP includes
#include "MenuXP.h"

// Scintilla wrapper includes
#include <atlscintilla.h>
#include <SciLexer.h>

// Underworld Adventures includes
#include "dbgserver.hpp"

// project-specific includes
// #include "WindowBase.hpp" // TODO put back in
#include "EditListViewCtrl.hpp"


// check macro to check if the REFLECT_NOTIFICATIONS() macro was added to the parent class
#define ATLASSERT_ADDED_REFLECT_NOTIFICATIONS() CheckAddedReflectNotifications(hWnd, uMsg, wParam, lParam);

void CheckAddedReflectNotifications(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//@}
