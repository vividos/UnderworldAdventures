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
#include "playerinfo.hpp"

// frame layout library

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
   EVT_MENU(MENU_UNDERW_PLAYER, ua_debugger_main_frame::OnMenuUnderwPlayer)
END_EVENT_TABLE()


// ua_debugger_main_frame methods

ua_debugger_main_frame::ua_debugger_main_frame(wxWindow *parent,
   const wxWindowID id, const wxString& title,
   const wxPoint& pos, const wxSize& size, const long style)
:wxMDIParentFrame(parent, id, title, pos, size, style)
{
   // frame layout manager
   m_pLayout = new wxFrameLayout(this, GetClientWindow());

   m_pLayout->SetUpdatesManager(new cbGCUpdatesMgr());

   // setup plugins
   m_pLayout->PushDefaultPlugins();

   m_pLayout->AddPlugin( CLASSINFO(cbBarHintsPlugin) );
   m_pLayout->AddPlugin( CLASSINFO(cbHintAnimationPlugin) );
//   m_pLayout->AddPlugin( CLASSINFO(cbRowDragPlugin) );
   m_pLayout->AddPlugin( CLASSINFO(cbAntiflickerPlugin) );
   m_pLayout->AddPlugin( CLASSINFO(cbSimpleCustomizationPlugin) );

   m_pLayout->EnableFloating(TRUE);



   // menu stuff
   m_pMenuBar = new wxMenuBar();

   // underworld menu
   m_pUnderwMenu = new wxMenu();
   m_pUnderwMenu->Append(MENU_UNDERW_PLAYER, "&Player Info", "shows Player Infos");
   m_pUnderwMenu->AppendSeparator();
   m_pMenuBar->Append(m_pUnderwMenu, "&Underworld");

   SetMenuBar(m_pMenuBar);


   // status bar
   CreateStatusBar(3);
   SetStatusText("ready.", 0);
}

bool ua_debugger_main_frame::CheckBarAvail(wxString& barname)
{
   // check if bar frame already exists
   cbBarInfo* barinfo = m_pLayout->FindBarByName(barname);
   if (barinfo != NULL)
   {
      // bar exists
      if (barinfo->mState == wxCBAR_HIDDEN)
      {
         // unhide bar when necessary
         barinfo->mState = wxCBAR_DOCKED_HORIZONTALLY;

         m_pLayout->DoSetBarState(barinfo);
         m_pLayout->RefreshNow();
      }
   }

   return barinfo != NULL;
}

void ua_debugger_main_frame::OnMenuUnderwPlayer(wxCommandEvent &event)
{
   if (!CheckBarAvail(wxString(ua_playerinfo_list::frame_name)))
   {
      // create new player info list and add it
      ua_playerinfo_list* pilist = new ua_playerinfo_list(
         this, -1, wxDefaultPosition, wxSize(0,0), wxLC_REPORT);

      pilist->AddBar(m_pLayout);

      m_pLayout->RefreshNow();
   }
}
