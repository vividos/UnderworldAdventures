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


// macros

IMPLEMENT_DYNAMIC_CLASS(ua_lua_document, wxDocument)
IMPLEMENT_DYNAMIC_CLASS(ua_lua_view, wxView)


void ua_lua_view::OnDraw(wxDC* dc)
{
}


// ua_lua_source_frame event table

BEGIN_EVENT_TABLE(ua_lua_source_frame, wxMDIChildFrame)
END_EVENT_TABLE()


const char* frame_name = "luaSourceFile";


// ua_lua_source_frame methods

ua_lua_source_frame::ua_lua_source_frame(
   /*wxDocManager* doc_manager, */
   wxString& filename, wxMDIParentFrame* parent, wxWindowID id,
   const wxPoint& pos, const wxSize& size, long style)
:wxMDIChildFrame(parent, id, "Lua Source File", pos, size, style,frame_name)
{
   textctrl = new wxStyledTextCtrl(this, -1, wxDefaultPosition, wxDefaultSize,
      0, wxSTCNameStr);

   // set font styles
   wxFont font(8, wxMODERN, wxNORMAL, wxNORMAL);
   textctrl->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
   textctrl->StyleClearAll();

   // set color styles
   textctrl->StyleSetForeground(0,  wxColour(0x80, 0x80, 0x80));
   textctrl->StyleSetForeground(1,  wxColour(0x00, 0x7f, 0x00));
   textctrl->StyleSetForeground(2,  wxColour(0x00, 0x7f, 0x00));
   textctrl->StyleSetForeground(3,  wxColour(0x7f, 0x7f, 0x7f));
   textctrl->StyleSetForeground(4,  wxColour(0x00, 0x7f, 0x7f));
   textctrl->StyleSetForeground(5,  wxColour(0x00, 0x00, 0x7f));
   textctrl->StyleSetForeground(6,  wxColour(0x7f, 0x00, 0x7f));
   textctrl->StyleSetForeground(7,  wxColour(0x7f, 0x00, 0x7f));
   textctrl->StyleSetForeground(8,  wxColour(0x00, 0x7f, 0x7f));
   textctrl->StyleSetForeground(9,  wxColour(0x7f, 0x7f, 0x7f));
   textctrl->StyleSetForeground(10, wxColour(0x00, 0x00, 0x00));
   textctrl->StyleSetForeground(11, wxColour(0x00, 0x00, 0x00));
   textctrl->StyleSetBold(5,  TRUE);
   textctrl->StyleSetBold(10, TRUE);

   // some nice editor enhancements
   //textctrl->SetIndentationGuides(true);
   //textctrl->SetViewEOL(true);
   textctrl->SetEdgeColumn(80);
   textctrl->SetEdgeMode(wxSTC_EDGE_LINE);
   textctrl->SetViewWhiteSpace(wxSTC_WS_VISIBLEALWAYS);

   // open file and insert text
   if (filename.Length() > 0)
   {
      wxFile file(filename);
      wxString st;

      char* buff = st.GetWriteBuf(file.Length());
      file.Read(buff, file.Length());
      st.UngetWriteBuf();

      textctrl->InsertText(0, st);
      textctrl->EmptyUndoBuffer();
   }


   // markers on margins
   textctrl->SetMarginType(0, wxSTC_MARGIN_NUMBER);
   textctrl->SetMarginType(1, wxSTC_MARGIN_SYMBOL);

   textctrl->SetMarginMask(1, 1|2|4|8|16|32|64|128|256|512|1024);

   textctrl->SetMarginWidth(0, 36);
   textctrl->SetMarginWidth(1, 16);

   // define arrow marker
   textctrl->MarkerDefine(0, wxSTC_MARK_ARROWS);
   textctrl->MarkerDefine(1, wxSTC_MARK_CIRCLE, "c00000", "c00000");

   textctrl->MarkerDefine(2, wxSTC_MARK_ARROW);
   textctrl->MarkerDefine(3, wxSTC_MARK_ARROWDOWN);
   textctrl->MarkerDefine(4, wxSTC_MARK_ARROWS);
   textctrl->MarkerDefine(5, wxSTC_MARK_BOXMINUS);
   textctrl->MarkerDefine(6, wxSTC_MARK_BOXMINUSCONNECTED);
   textctrl->MarkerDefine(7, wxSTC_MARK_BOXPLUS);
   textctrl->MarkerDefine(8, wxSTC_MARK_BOXPLUSCONNECTED);
   textctrl->MarkerDefine(9, wxSTC_MARK_CIRCLE);
/*
   textctrl->SetMarginType(10, wxSTC_MARK_CIRCLEMINUS);
   textctrl->SetMarginType(1, wxSTC_MARK_CIRCLEMINUSCONNECTED);
   textctrl->SetMarginType(1, wxSTC_MARK_CIRCLEPLUS);
   textctrl->SetMarginType(1, wxSTC_MARK_CIRCLEPLUSCONNECTED);
   textctrl->SetMarginType(1, wxSTC_MARK_DOTDOTDOT);
/*
   textctrl->SetMarginType(1, wxSTC_MARK_LCORNER);
   textctrl->SetMarginType(1, wxSTC_MARK_LCORNERCURVE);
   textctrl->SetMarginType(1, wxSTC_MARK_MINUS);
   textctrl->SetMarginType(1, wxSTC_MARK_PLUS);
   textctrl->SetMarginType(1, wxSTC_MARK_ROUNDRECT);
   textctrl->SetMarginType(1, wxSTC_MARK_SHORTARROW);
   textctrl->SetMarginType(1, wxSTC_MARK_SMALLRECT);
   textctrl->SetMarginType(1, wxSTC_MARK_TCORNER);
   textctrl->SetMarginType(1, wxSTC_MARK_TCORNERCURVE);
   textctrl->SetMarginType(1, wxSTC_MARK_VLINE);
*/

   textctrl->MarkerAdd(0, 0);
   textctrl->MarkerAdd(1, 1);
   textctrl->MarkerAdd(2, 2);
   textctrl->MarkerAdd(3, 3);
   textctrl->MarkerAdd(4, 4);
   textctrl->MarkerAdd(5, 5);
   textctrl->MarkerAdd(6, 6);
   textctrl->MarkerAdd(7, 7);
   textctrl->MarkerAdd(8, 8);
   textctrl->MarkerAdd(9, 9);


   // set Lua lexer
   textctrl->SetLexer(wxSTC_LEX_LUA);

   // set some keywords to highlight
/*
   textctrl->SetKeyWords(0,
                    "asm auto bool break case catch char class const "
                    "const_cast continue default delete do double "
                    "dynamic_cast else enum explicit export extern "
                    "false float for friend goto if inline int long "
                    "mutable namespace new operator private protected "
                    "public register reinterpret_cast return short signed "
                    "sizeof static static_cast struct switch template this "
                    "throw true try typedef typeid typename union unsigned "
                    "using virtual void volatile wchar_t while");
*/
}
