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
/*! \file mainframe.hpp

   \brief debugger main frame: MDI parent window

*/

// include guard
#ifndef __uadebug__mainframe_hpp__
#define __uadebug__mainframe_hpp__

// needed includes


// classes

//! MDI parent frame
class ua_debugger_main_frame: public wxMDIParentFrame
{
public:
   //! ctor
   ua_debugger_main_frame(wxWindow *parent, const wxWindowID id,
      const wxString& title,const wxPoint& pos, const wxSize& size,
      const long style);

protected:
   //! returns false when bar with given name is not found; else activates bar
   bool CheckBarAvail(wxString& barname);

   // message handler

   //! menu handler: Underworld | Player Info
   void OnMenuUnderwPlayer(wxCommandEvent &event);

protected:
   //! frame layout object
   wxFrameLayout* m_pLayout;

   //! menu bar
   wxMenuBar *m_pMenuBar;

   //! underworld menu
   wxMenu *m_pUnderwMenu;

   //! menu id's
   enum
   {
      MENU_UNDERW_PLAYER=1000, // shows player info
   };

   DECLARE_EVENT_TABLE()
};

#endif
