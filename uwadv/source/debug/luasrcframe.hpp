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
/*! \file luasrcframe.hpp

   \brief Lua source file frame

*/

// include guard
#ifndef uadebug_luasrcframe_hpp_
#define uadebug_luasrcframe_hpp_

// needed includes
#include <wx/docview.h>


// forward references
class wxStyledTextCtrl;


// classes

//! lua source file frame
class ua_lua_source_frame: public wxMDIChildFrame
{
public:
   //! ctor
   ua_lua_source_frame( /*wxDocManager* doc_manager, */
      wxString& filename, wxMDIParentFrame* parent, wxWindowID id,
      const wxPoint& pos, const wxSize& size, long style);

protected:
   //! scintilla text edit control
   wxStyledTextCtrl* textctrl;

   DECLARE_EVENT_TABLE()
};


#endif
