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
/*! \file mainframe.hpp

   \brief debugger main frame: MDI parent window

*/

// include guard
#ifndef uadebug_mainframe_hpp_
#define uadebug_mainframe_hpp_

// needed includes


// forward references
class wxMDIChildWindow;


// classes

//! MDI parent frame
class ua_debugger_main_frame: public wxMDIParentFrame
{
public:
   //! ctor
   ua_debugger_main_frame(wxWindow* parent, const wxWindowID id,
      const wxString& title,const wxPoint& pos, const wxSize& size,
      const long style);

   //! adds standard frame menus
   void AddFrameMenus(wxMenuBar* menubar);

protected:
   //! updates all bars and windows
   void UpdateAll();

   //! returns false when bar with given name is not found; else activates bar
   bool CheckBarAvail(wxString& barname);

   // message handler

   //! menu handler: File | Open
   void OnMenuFileOpen(wxCommandEvent& event);

   //! menu handler: File | Quit
   void OnMenuFileQuit(wxCommandEvent& event);

   //! menu handler: Underworld | Update
   void OnMenuUnderwUpdate(wxCommandEvent& event);

   //! menu handler: Underworld | Suspend Game
   void OnMenuUnderwSuspend(wxCommandEvent& event);

   //! menu handler: Underworld | Resume Game
   void OnMenuUnderwResume(wxCommandEvent& event);

   //! menu handler: Underworld | Tilemap View
   void OnMenuUnderwTilemapView(wxCommandEvent& event);

   //! menu handler: Underworld | Hotspot List
   void OnMenuUnderwHotspotList(wxCommandEvent& event);

   //! menu handler: Underworld | Player Info
   void OnMenuUnderwPlayer(wxCommandEvent& event);

   //! menu handler: Underworld | Master Object List
   void OnMenuUnderwObjectList(wxCommandEvent& event);

protected:
   // layout / menu stuff

   //! frame layout object
   wxFrameLayout* m_pLayout;

   //! menu bar
   wxMenuBar* menubar;

   //! menu id's
   enum
   {
      MENU_FILE_QUIT=1000,
      MENU_FILE_OPEN,
      MENU_UNDERW_SUSPEND,
      MENU_UNDERW_RESUME,
      MENU_UNDERW_UPDATE,
      MENU_UNDERW_TILEMAPVIEW,//!< shows tilemap view
      MENU_UNDERW_HOTSPOTLIST,//!< shows hotspot list
      MENU_UNDERW_PLAYER,     //!< shows player info
      MENU_UNDERW_OBJECTLIST, //!< shows master object list
   };

   DECLARE_EVENT_TABLE()
};

#endif
