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
#include "playerinfo.hpp"


// ua_playerinfo_list misc. stuff

const char* ua_playerinfo_list::frame_name = "playerinfo";

// event table
BEGIN_EVENT_TABLE(ua_playerinfo_list, wxListCtrl)
END_EVENT_TABLE()


// ua_playerinfo_list methods

ua_playerinfo_list::ua_playerinfo_list(wxWindow *parent,
   const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
:wxListCtrl(parent, id, pos, size, style)
{
   wxListItem column;
   column.m_format = wxLIST_FORMAT_LEFT;
   column.m_width = 120;
   column.m_text = "Player Attribute";
   column.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_WIDTH | wxLIST_MASK_FORMAT;

   InsertColumn(0,column);

   column.m_width = 200-120-10;
   column.m_text = "Value";
   InsertColumn(1,column);
}

void ua_playerinfo_list::AddBar(wxFrameLayout* pLayout)
{
   // add bar to frame layout

   cbDimInfo sizes( 600,200, // when docked horizontally
      200,800, // when docked vertically
      200,400, // when floated
      FALSE,   // the bar is not fixed-size
      4,       // vertical gap (bar border)
      4        // horizontal gap (bar border)
   );

   pLayout->AddBar(this,    // bar window
      sizes, FL_ALIGN_RIGHT, // alignment ( 0-top,1-bottom, etc)
      0,                    // insert into 0th row (vert. position)
      0,                    // offset from the start of row (in pixels)
      frame_name,           // name to refere in customization pop-ups
      TRUE
   );
}
