/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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
/*! \file dbgapp.cpp

   \brief debugger main application

*/

// needed includes
#include "dbgcommon.hpp"
#include "dbgapp.hpp"
#include "mainframe.hpp"


// wxWindows macros

IMPLEMENT_APP(ua_debugger)

void* ua_init_debug_data;


// ua_debugger methods

bool ua_debugger::OnInit()
{
   // init client interface
   if (!client.init_client(ua_init_debug_data))
   {
      // interface version doesn't match
      wxMessageDialog dlg(NULL,
         "Debugger Interface versions doesn't match. Please get a newer "
         "uadebug.dll.", "Underworld Adventures Debugger");
      dlg.ShowModal();
      return false;
   }

   // create document manager
   doc_manager = new wxDocManager;

   // create debugger main frame
   ua_debugger_main_frame* frame = new ua_debugger_main_frame(
      doc_manager, (wxFrame *)NULL, -1, "Underworld Adventures Debugger",
      wxPoint(-1, -1), wxSize(1024, 768),
      wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);

   // show it
   frame->Show(TRUE);
   SetTopWindow(frame);

   return true;
}

int ua_debugger::OnExit()
{
   delete doc_manager;
   doc_manager = NULL;
   return 0;
}


// global functions

//! debugger start
/*! Starts debugger instance. This function is called from Underworld
    Adventures when the user presses the debugger key. This function (and in
    fact the whole debugger) runs in another thread.
*/
void uadebug_start(void* data)
{
   if (data == NULL) return;
   ua_init_debug_data = data;

#ifdef WIN32
   // call win32 specific entry function
   HMODULE mod = ::GetModuleHandle("uadebug.dll");
   wxEntry((HINSTANCE)mod,NULL,"",0);
#else
   // generic entry function
   wxEntry(0, NULL);
#endif
}
