/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Underworld Adventures Team

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
/*! \file dbgapp.hpp

   \brief debugger main application

*/

// needed includes
#include "dbgcommon.hpp"
#include "mainframe.hpp"

// fl headers
#include "wx/fl/controlbar.h"     // core API

// extra plugins
#include "wx/fl/barhintspl.h"    // beveal for bars with "X"s and grooves
#include "wx/fl/rowdragpl.h"     // NC-look with dragable rows
#include "wx/fl/cbcustom.h"      // customization plugin
#include "wx/fl/hintanimpl.h"

// beauty-care
#include "wx/fl/gcupdatesmgr.h"  // smooth d&d
#include "wx/fl/antiflickpl.h"   // double-buffered repaint of decorations
#include "wx/fl/dyntbar.h"       // auto-layouting toolbar
#include "wx/fl/dyntbarhnd.h"    // control-bar dimension handler for it


// event table

BEGIN_EVENT_TABLE(ua_debugger_main_frame, wxMDIParentFrame)
END_EVENT_TABLE()


// ua_debugger_main_frame methods

ua_debugger_main_frame::ua_debugger_main_frame(wxWindow *parent,
   const wxWindowID id, const wxString& title,
   const wxPoint& pos, const wxSize& size, const long style)
:wxMDIParentFrame(parent, id, title, pos, size, style)
{
   CreateStatusBar(3);
   SetStatusText("Ready", 0);
}
