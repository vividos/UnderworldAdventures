/*
   Underworld Adventures Debugger - a debugger tool for Underworld Adventures
   Copyright (c) 2004,2005 Michael Fink

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
/*! \file LuaSourceView.cpp

   \brief Lua source file view

*/

// includes
#include "stdatl.hpp"
#include "LuaSourceView.hpp"

// methods

LRESULT CLuaSourceView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL,
      WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, WS_EX_CLIENTEDGE);

   m_view.StyleSetFont(STYLE_DEFAULT, _T("Lucida Console"));
   m_view.SetEdgeColumn(80);
   m_view.SetEdgeMode(EDGE_LINE);

   m_view.SetKeyWords(0,
      "asm auto bool break case catch char class const "
      "const_cast continue default delete do double "
      "dynamic_cast else enum explicit export extern "
      "false float for friend goto if inline int long "
      "mutable namespace new operator private protected "
      "public register reinterpret_cast return short signed "
      "sizeof static static_cast struct switch template this "
      "throw true try typedef typeid typename union unsigned "
      "using virtual void volatile wchar_t while");

//   m_view.
   m_view.StyleSetFore(0,  RGB(0x80, 0x80, 0x80));
   m_view.StyleSetFore(1,  RGB(0x00, 0x7f, 0x00));
   m_view.StyleSetFore(2,  RGB(0x00, 0x7f, 0x00));
   m_view.StyleSetFore(3,  RGB(0x7f, 0x7f, 0x7f));
   m_view.StyleSetFore(4,  RGB(0x00, 0x7f, 0x7f));
   m_view.StyleSetFore(5,  RGB(0x00, 0x00, 0x7f));
   m_view.StyleSetFore(6,  RGB(0x7f, 0x00, 0x7f));
   m_view.StyleSetFore(7,  RGB(0x7f, 0x00, 0x7f));
   m_view.StyleSetFore(8,  RGB(0x00, 0x7f, 0x7f));
   m_view.StyleSetFore(9,  RGB(0x7f, 0x7f, 0x7f));
   m_view.StyleSetFore(10, RGB(0x00, 0x00, 0x00));
   m_view.StyleSetFore(11, RGB(0x00, 0x00, 0x00));
   m_view.StyleSetBold(5,  true);
   m_view.StyleSetBold(10, true);

   m_view.SetLexer(SCLEX_LUA);

   m_view.SetWindowText(
      "--\n"
      "-- Underworld Adventures - an Ultima Underworld hacking project\n"
      "-- Copyright (c) 2002,2003,2004 Underworld Adventures Team\n"
      "--\n"
      "-- This program is free software; you can redistribute it and/or modify\n"
      "-- it under the terms of the GNU General Public License as published by\n"
      "-- the Free Software Foundation; either version 2 of the License, or\n"
      "-- (at your option) any later version.\n"
      "--\n"
      "-- This program is distributed in the hope that it will be useful,\n"
      "-- but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
      "-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
      "-- GNU General Public License for more details.\n"
      "--\n"
      "-- You should have received a copy of the GNU General Public License\n"
      "-- along with this program; if not, write to the Free Software\n"
      "-- Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n"
      "--\n"
      "-- $Id$\n"
      "--\n"
      "\n"
      "--\n"
      "-- triggers.lua - trigger handling\n"
      "--\n"
      "\n"
      "-- constants\n"
      "\n"
      "-- trigger item_id's\n"
      "trig_move    = 0x01a0  -- a_move trigger\n"
      "trig_pick_up = 0x01a1  -- a_pick up trigger\n"
      "trig_use     = 0x01a2  -- a_use trigger\n"
      "trig_look    = 0x01a3  -- a_look trigger\n"
      "trig_step_on = 0x01a4  -- a_step on trigger\n"
      "trig_open    = 0x01a5  -- an_open trigger\n"
      "trig_unlock  = 0x01a6  -- an_unlock trigger\n"
      "\n"
      "trig_first  = trig_move\n"
      "trig_last   = trig_unlock\n"
      "\n"
      "-- tables\n"
      "\n"
      "\n"
      "-- functions\n"
      "\n"
      "-- checks a given handle for a given trigger type and sets it off\n"
      "function trigger_check_handle(trig_id,objpos)\n"
      "\n"
      "   objinfo = objlist.get_info(objpos)\n"
      "\n"
      "   -- do we have a special linked object?\n"
      "   if objinfo.is_quantity ~= 0\n"
      "   then\n"
      "      return\n"
      "   end\n"
      "\n"
      "   -- check if special linked obj is trigger\n"
      "   objinfo2 = objlist.get_info(objinfo.quantity)\n"
      "\n"
      "   if objinfo2.item_id == trig_id and objinfo2.is_quantity == 0\n"
      "   then\n"
      "      print( \"trigger set off: \" .. uw.get_string(4,objinfo2.item_id))\n"
      "\n"
      "      trap_set_off(objinfo2.quantity,objpos)\n"
      "   end\n"
      "\n"
      "end\n"
      "\n"
      "\n"
      "-- handles all triggers that are set off\n"
      "function trigger_set_off(objpos)\n"
      "\n"
      "   dump_objinfo_table(objpos)\n"
      "\n"
      "   local objinfo = objlist.get_info(objpos)\n"
      "\n"
      "   if objinfo.item_id < trig_first or objinfo.item_id > trig_last\n"
      "   then\n"
      "      return -- no known trigger to set off\n"
      "   end\n"
      "\n"
      "   trap_set_off(objinfo.quantity,objpos)\n"
      "\n"
      "end\n"
      );

   bHandled = FALSE;
   return 1;
}
