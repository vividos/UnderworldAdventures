//
// Underworld Adventures Debugger - a debugger tool for Underworld Adventures
// Copyright (c) 2004,2005,2019 Underworld Adventures Team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
/// \file LuaScriptEditorView.cpp
/// \brief Lua script editor view
//
#include "pch.hpp"
#include "LuaScriptEditorView.hpp"

const int c_markerCurrentExecution = 1;
const int c_markerBreakpoint = 2;

void LuaScriptEditorView::SetupSourceEditor()
{
   StyleSetFont(STYLE_DEFAULT, "Lucida Console");
   StyleSetSize(STYLE_DEFAULT, 11);

   SetEdgeColumn(100);
   SetEdgeMode(EDGE_LINE);

   SetMarginWidthN(0, 50); // margin 0 displays line numbers, but has default width 0

   MarkerDefine(c_markerCurrentExecution, SC_MARK_ARROW);
   MarkerSetBack(c_markerCurrentExecution, RGB(255, 255, 0));

   StyleSetFore(0, RGB(0x80, 0x80, 0x80));
   StyleSetFore(1, RGB(0x00, 0x7f, 0x00));
   StyleSetFore(2, RGB(0x00, 0x7f, 0x00));
   StyleSetFore(3, RGB(0x7f, 0x7f, 0x7f));
   StyleSetFore(4, RGB(0x00, 0x7f, 0x7f));
   StyleSetFore(5, RGB(0x00, 0x00, 0x7f));
   StyleSetFore(6, RGB(0x7f, 0x00, 0x7f));
   StyleSetFore(7, RGB(0x7f, 0x00, 0x7f));
   StyleSetFore(8, RGB(0x00, 0x7f, 0x7f));
   StyleSetFore(9, RGB(0x7f, 0x7f, 0x7f));
   StyleSetFore(10, RGB(0x00, 0x00, 0x00));
   StyleSetFore(11, RGB(0x00, 0x00, 0x00));
   StyleSetBold(5, true);
   StyleSetBold(10, true);

   int previousLexer = GetLexer();
   SetLexerByName("lua");
   int nowLexer = GetLexer();
   ATLVERIFY(previousLexer != nowLexer);

   // set Lua keywords and add uwadv specific keywords
   SetKeyWords(0,
      "and break do else elseif end for function if in " // Lua keywords
      "local nil not or repeat return then until while "
      "uw player objectlist tilemap runebag conv quest prop"); // uwadv objects

   SetTabWidth(3);
   SetUseTabs(false);

   // only notify about text edits in SCEN_CHANGE messages
   //SetModEventMask(SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT);
}

void LuaScriptEditorView::SetCurrentExecutionLine(int lineNumber)
{
   MarkerDeleteAll(c_markerCurrentExecution);

   if (lineNumber == -1)
   {
      return;
   }

   MarkerAdd(lineNumber - 1, c_markerCurrentExecution);
   ScrollToLine(lineNumber - 1);
}
