/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

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
/*! \file mainframe.cpp

   \brief debugger main MDI frame

*/

// needed includes
#include "dbgcommon.hpp"
#include "dbgapp.hpp"
#include "mainframe.hpp"
#include "playerinfo.hpp"
#include "objectlist.hpp"

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


// ua_debugger_main_frame event table

BEGIN_EVENT_TABLE(ua_debugger_main_frame, wxMDIParentFrame)
   EVT_MENU(MENU_FILE_QUIT, ua_debugger_main_frame::OnMenuFileQuit)
   EVT_MENU(MENU_UNDERW_UPDATE, ua_debugger_main_frame::OnMenuUnderwUpdate)
   EVT_MENU(MENU_UNDERW_SUSPEND, ua_debugger_main_frame::OnMenuUnderwSuspend)
   EVT_MENU(MENU_UNDERW_RESUME, ua_debugger_main_frame::OnMenuUnderwResume)
   EVT_MENU(MENU_UNDERW_PLAYER, ua_debugger_main_frame::OnMenuUnderwPlayer)
   EVT_MENU(MENU_UNDERW_OBJECTLIST, ua_debugger_main_frame::OnMenuUnderwObjectList)
END_EVENT_TABLE()


// ua_debugger_main_frame methods

ua_debugger_main_frame::ua_debugger_main_frame(wxWindow* parent,
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
//   m_pLayout->AddPlugin( CLASSINFO(cbRowDragPlugin) ); // mozilla-like bars
   m_pLayout->AddPlugin( CLASSINFO(cbAntiflickerPlugin) );
   m_pLayout->AddPlugin( CLASSINFO(cbSimpleCustomizationPlugin) );

   m_pLayout->EnableFloating(TRUE);


   // menu stuff
   m_pMenuBar = new wxMenuBar();

   // file menu
   m_pFileMenu = new wxMenu();
//   m_pUnderwMenu->AppendSeparator();
   m_pFileMenu->Append(MENU_FILE_QUIT, "&Quit", "quits the debugger");
   m_pMenuBar->Append(m_pFileMenu, "&File");

   // underworld menu
   m_pUnderwMenu = new wxMenu();
   m_pUnderwMenu->Append(MENU_UNDERW_UPDATE, "&Update", "updates all windows");
   m_pUnderwMenu->Append(MENU_UNDERW_SUSPEND, "&Suspend Game", "suspends a running game");
   m_pUnderwMenu->Append(MENU_UNDERW_RESUME, "&Resume Game", "resumes a suspended game");
   m_pUnderwMenu->AppendSeparator();
   m_pUnderwMenu->Append(MENU_UNDERW_PLAYER, "&Player Info", "shows Player Infos");
   m_pUnderwMenu->Append(MENU_UNDERW_OBJECTLIST, "Master &Object List", "shows Master Object List");
   m_pMenuBar->Append(m_pUnderwMenu, "&Underworld");

   SetMenuBar(m_pMenuBar);


   // status bar
   CreateStatusBar(3);
   SetStatusText("ready.", 0);

   // update all bars and windows
   UpdateAll();
}

void ua_debugger_main_frame::UpdateAll()
{
   // update player info
   {
      cbBarInfo* barinfo = m_pLayout->FindBarByName(ua_playerinfo_list::frame_name);
      if (barinfo!=NULL && barinfo->mState != wxCBAR_HIDDEN)
      {
         ua_playerinfo_list* pilist =
            reinterpret_cast<ua_playerinfo_list*>(barinfo->mpBarWnd);

         pilist->UpdateData();
      }
   }

   // todo: update more windows

   // update master object list window
   wxString name(ua_objectlist_frame::frame_name);
   wxWindow* wnd = wxWindow::FindWindowByName(name);
   if (wnd!=NULL)
      reinterpret_cast<ua_objectlist_frame*>(wnd)->UpdateData();
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
         m_pLayout->SetBarState(barinfo,wxCBAR_DOCKED_HORIZONTALLY,true);
         m_pLayout->RefreshNow();
      }
   }

   return barinfo != NULL;
}

void ua_debugger_main_frame::OnMenuFileQuit(wxCommandEvent& event)
{
   Close(TRUE);
}

void ua_debugger_main_frame::OnMenuUnderwUpdate(wxCommandEvent& event)
{
   UpdateAll();
}

void ua_debugger_main_frame::OnMenuUnderwSuspend(wxCommandEvent& event)
{
   wxGetApp().command(udc_game_suspend,0,NULL,NULL);
}

void ua_debugger_main_frame::OnMenuUnderwResume(wxCommandEvent& event)
{
   wxGetApp().command(udc_game_resume,0,NULL,NULL);
}

void ua_debugger_main_frame::OnMenuUnderwPlayer(wxCommandEvent& event)
{
   wxString name(ua_playerinfo_list::frame_name);
   if (!CheckBarAvail(name))
   {
      // create new player info list and add it
      ua_playerinfo_list* pilist = new ua_playerinfo_list(
         this, -1, wxDefaultPosition, wxDefaultSize, 0);

      pilist->AddBar(m_pLayout);
      pilist->UpdateData();

      m_pLayout->RefreshNow();
   }
}

void ua_debugger_main_frame::OnMenuUnderwObjectList(wxCommandEvent& event)
{
   // find window using FindWindowByName
   wxString name(ua_objectlist_frame::frame_name);
   wxWindow* wnd = wxWindow::FindWindowByName(name);
   if (wnd==NULL)
   {
      new ua_objectlist_frame(-1,this,-1,wxDefaultPosition,
         wxSize(700,500),wxDEFAULT_FRAME_STYLE);
   }
   else
   {
      // show the window
      wnd->Raise();
      wnd->Show();
   }
}
