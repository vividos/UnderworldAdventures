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
/*! \file luasrcframe.cpp

   \brief Lua source file frame

*/

// needed includes
#include "dbgcommon.hpp"
#include "luasrcframe.hpp"
#include "wx/stc/stc.h"
#include "wx/file.h"


// ua_lua_source_frame event table

BEGIN_EVENT_TABLE(ua_lua_source_frame, wxMDIChildFrame)
END_EVENT_TABLE()


const char* frame_name = "luaSourceFile";

// ua_lua_source_frame methods

ua_lua_source_frame::ua_lua_source_frame(
   /*wxDocManager* doc_manager, */
   wxMDIParentFrame* parent, wxWindowID id,
   const wxPoint& pos, const wxSize& size, long style)
:wxMDIChildFrame(parent, id, "Lua Source File", pos, size, style,frame_name)
{
   textctrl = new wxStyledTextCtrl(this, -1, wxDefaultPosition, wxDefaultSize,
      0, wxSTCNameStr);

   // give it some text to play with
#if 0
   {
      wxFile   file("D:\\uwadv\\uwadv\\uadata\\uw1\\scripts\\traps.lua");
      wxString st;

      char* buff = st.GetWriteBuf(file.Length());
      file.Read(buff, file.Length());
      st.UngetWriteBuf();

      textctrl->InsertText(0, st);
      textctrl->EmptyUndoBuffer();
   }
#endif

#ifdef __WXMSW__
    textctrl->StyleSetSpec(2, "fore:#007f00,bold,face:Courier New,size:9");
#else
    textctrl->StyleSetSpec(2, "fore:#007f00,bold,face:Helvetica,size:9");
#endif

   // set Lua lexer
   textctrl->SetLexer(wxSTC_LEX_LUA);

   /*this, -1, 
      wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL,
      wxDefaultValidator, "luaSourceFile");


    wxStyledTextCtrl(wxWindow *parent, wxWindowID id,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize, long style = 0,
                     const wxString& name = wxSTCNameStr);

*/
}
