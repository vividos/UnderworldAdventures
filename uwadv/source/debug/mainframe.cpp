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
/*! \file mainframe.cpp

   \brief debugger main MDI frame

*/

// needed includes
#include "dbgcommon.hpp"
#include "dbgapp.hpp"
#include "mainframe.hpp"
#include "playerinfo.hpp"
#include "objectlist.hpp"
#include "hotspotlist.hpp"
#include "tilemapview.hpp"
#include "luasrcframe.hpp"

// wxWindows includes
#include "wx/filedlg.h"

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

BEGIN_EVENT_TABLE(ua_debugger_main_frame, wxDocMDIParentFrame)
   EVT_MENU(MENU_FILE_NEW, ua_debugger_main_frame::OnMenuFileNew)
   EVT_MENU(MENU_FILE_OPEN, ua_debugger_main_frame::OnMenuFileOpen)
   EVT_MENU(MENU_FILE_CLOSE, ua_debugger_main_frame::OnMenuFileClose)
   EVT_MENU(MENU_FILE_SAVE, ua_debugger_main_frame::OnMenuFileSave)
   EVT_MENU(MENU_FILE_SAVEAS, ua_debugger_main_frame::OnMenuFileSaveAs)
   EVT_MENU(MENU_FILE_SAVEALL, ua_debugger_main_frame::OnMenuFileSaveAll)
   EVT_MENU(MENU_FILE_QUIT, ua_debugger_main_frame::OnMenuFileQuit)

   EVT_UPDATE_UI(MENU_FILE_CLOSE, ua_debugger_main_frame::OnUpdateUIFileClose)

   EVT_MENU(MENU_UNDERW_UPDATE, ua_debugger_main_frame::OnMenuUnderwUpdate)
   EVT_MENU(MENU_UNDERW_SUSPEND, ua_debugger_main_frame::OnMenuUnderwSuspend)
   EVT_MENU(MENU_UNDERW_RESUME, ua_debugger_main_frame::OnMenuUnderwResume)
   EVT_MENU(MENU_UNDERW_TILEMAPVIEW, ua_debugger_main_frame::OnMenuUnderwTilemapView)
   EVT_MENU(MENU_UNDERW_HOTSPOTLIST, ua_debugger_main_frame::OnMenuUnderwHotspotList)
   EVT_MENU(MENU_UNDERW_PLAYER, ua_debugger_main_frame::OnMenuUnderwPlayer)
   EVT_MENU(MENU_UNDERW_OBJECTLIST, ua_debugger_main_frame::OnMenuUnderwObjectList)
END_EVENT_TABLE()


// ua_debugger_main_frame methods

ua_debugger_main_frame::ua_debugger_main_frame(
   wxDocManager* doc_manager, wxFrame* parent,
   const wxWindowID id, const wxString& title,
   const wxPoint& pos, const wxSize& size, const long style)
:wxDocMDIParentFrame(doc_manager, parent, id, title, pos, size, style)
{
   // frame layout manager
   {
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
   }


   // docview stuff
   {
      new wxDocTemplate(doc_manager,
         "lua source file", "*.lua", "", "lua", "lua source file", "loa source file view",
          CLASSINFO(ua_lua_document), CLASSINFO(ua_lua_view));
         /*
   dir_view_templ = new d64view_doc_template((wxDocManager*)doc_manager,
      "d64 disk image", "*.d64", "", "d64", "d64 disk image", "d64 disk image directory view",
          CLASSINFO(disk_document), CLASSINFO(dir_view));
*/
   }


   // application icon
#ifdef __WXMSW__
   SetIcon(wxIcon("uadebug"));
#endif


   // frame menu
   menubar = new wxMenuBar();
   AddFrameMenus(menubar);

   SetMenuBar(menubar);


   // status bar
   CreateStatusBar(3);
   SetStatusText("ready.", 0);

/*
   // toolbar
   {
      wxToolBar* tb = CreateToolBar(wxTB_FLAT | wxTB_HORIZONTAL,-1,"toolbar");

      wxBitmap toolBarBitmaps[3];

      toolBarBitmaps[0] = wxBitmap("new");
      toolBarBitmaps[1] = wxBitmap("open");
      toolBarBitmaps[2] = wxBitmap("save");

      tb->AddTool(MENU_FILE_NEW/ *wxID_NEW* /, _T("New"),   toolBarBitmaps[0], _T("New file"));
      tb->AddTool(MENU_FILE_OPEN/ *wxID_OPEN* /, _T("Open"), toolBarBitmaps[1], _T("Open file"));
      tb->AddTool(MENU_FILE_SAVE/ *wxID_SAVE* /, _T("Save"), toolBarBitmaps[2], _T("Save file"), wxITEM_CHECK);

      tb->Realize();
   }
*/

   // update all bars and windows
   UpdateAll();
}

void ua_debugger_main_frame::AddFrameMenus(wxMenuBar* menubar)
{
   // file menu
   wxMenu* filemenu = new wxMenu();
   filemenu->Append(MENU_FILE_NEW, "&New\tCtrl+N", "creates an empty Lua source file");
   filemenu->Append(MENU_FILE_OPEN, "&Open ...\tCtrl+O", "opens a Lua source file");
   filemenu->Append(MENU_FILE_CLOSE, "&Close", "opens a Lua source file");
   filemenu->AppendSeparator();
   filemenu->Append(MENU_FILE_SAVE, "&Save\tCtrl+S", "saves the current Lua source file");
   filemenu->Append(MENU_FILE_SAVEAS, "Save &As ...", "saves the current Lua source file under another name");
   filemenu->Append(MENU_FILE_SAVEALL, "Save A&ll", "saves all open Lua source files");
   filemenu->AppendSeparator();
   filemenu->Append(MENU_FILE_QUIT, "&Quit", "quits the debugger");
   menubar->Append(filemenu, "&File");

   // underworld menu
   wxMenu* uwmenu = new wxMenu();
   uwmenu->Append(MENU_UNDERW_UPDATE, "&Update", "updates all windows");
   uwmenu->Append(MENU_UNDERW_SUSPEND, "&Suspend Game", "suspends a running game");
   uwmenu->Append(MENU_UNDERW_RESUME, "&Resume Game", "resumes a suspended game");
   uwmenu->AppendSeparator();
   uwmenu->Append(MENU_UNDERW_TILEMAPVIEW, "&Tilemap View", "shows Tilemap");
   uwmenu->Append(MENU_UNDERW_HOTSPOTLIST, "&Hotspot List", "shows Hotspot List");
   uwmenu->Append(MENU_UNDERW_PLAYER, "&Player Info", "shows Player Infos");
   uwmenu->Append(MENU_UNDERW_OBJECTLIST, "Master &Object List", "shows Master Object List");
   menubar->Append(uwmenu, "&Underworld");
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

void ua_debugger_main_frame::OnMenuFileNew(wxCommandEvent& event)
{
   wxString filename("");
   new ua_lua_source_frame(filename, this, -1, wxDefaultPosition,
      wxDefaultSize, wxDEFAULT_FRAME_STYLE);
}

void ua_debugger_main_frame::OnMenuFileOpen(wxCommandEvent& event)
{
   wxFileDialog filedlg(this,
      "Please locate the Lua source file to open",
      "", "", "Lua Source Files (*.lua)|*.lua");

   filedlg.SetDirectory(wxGetHomeDir());

   if (filedlg.ShowModal() == wxID_OK)
   {
      wxString filename(filedlg.GetPath());
      new ua_lua_source_frame(filename, this, -1, wxDefaultPosition,
         wxDefaultSize, wxDEFAULT_FRAME_STYLE);
   }
}

void ua_debugger_main_frame::OnMenuFileClose(wxCommandEvent& event)
{
}

void ua_debugger_main_frame::OnMenuFileSave(wxCommandEvent& event)
{
}

void ua_debugger_main_frame::OnMenuFileSaveAs(wxCommandEvent& event)
{
}

void ua_debugger_main_frame::OnMenuFileSaveAll(wxCommandEvent& event)
{
}

void ua_debugger_main_frame::OnMenuFileQuit(wxCommandEvent& event)
{
   Close(TRUE);
}

void ua_debugger_main_frame::OnUpdateUIFileClose(wxUpdateUIEvent& event)
{
   event.Enable(false);
}

void ua_debugger_main_frame::OnMenuUnderwUpdate(wxCommandEvent& event)
{
   UpdateAll();
}

void ua_debugger_main_frame::OnMenuUnderwSuspend(wxCommandEvent& event)
{
//   wxGetApp().command(udc_game_suspend,0,NULL,NULL);
}

void ua_debugger_main_frame::OnMenuUnderwResume(wxCommandEvent& event)
{
//   wxGetApp().command(udc_game_resume,0,NULL,NULL);
}

void ua_debugger_main_frame::OnMenuUnderwTilemapView(wxCommandEvent& event)
{
   // find window using FindWindowByName
   wxString name(ua_tilemapview_frame::frame_name);
   wxWindow* wnd = wxWindow::FindWindowByName(name);
   if (wnd==NULL)
   {
      new ua_tilemapview_frame(this,-1,wxDefaultPosition,
         wxSize(600,400),wxDEFAULT_FRAME_STYLE);
   }
   else
   {
      // show the window
      wnd->Raise();
      wnd->Show();
   }
}

void ua_debugger_main_frame::OnMenuUnderwHotspotList(wxCommandEvent& event)
{
   // find window using FindWindowByName
   wxString name(ua_hotspotlist_frame::frame_name);
   wxWindow* wnd = wxWindow::FindWindowByName(name);
   if (wnd==NULL)
   {
      new ua_hotspotlist_frame(this,-1,wxDefaultPosition,
         wxSize(600,400),wxDEFAULT_FRAME_STYLE);
   }
   else
   {
      // show the window
      wnd->Raise();
      wnd->Show();
   }
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
         wxSize(750,500),wxDEFAULT_FRAME_STYLE);
   }
   else
   {
      // show the window
      wnd->Raise();
      wnd->Show();
   }
}
