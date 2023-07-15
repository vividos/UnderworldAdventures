//
// Underworld Adventures Debugger - a debugger tool for Underworld Adventures
// Copyright (c) 2004,2005,2019,2023 Underworld Adventures Team
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
#include "script/LuaState.hpp"

/// marker index for current execution arrow
const int c_markerCurrentExecution = 1;

/// marker index for breakpoint circle
const int c_markerBreakpoint = 2;

/// delay time in ms, after which a syntax check of the text in the view occurs
const UINT c_syntaxCheckDelayTimeInMs = 1000;

/// timer ID for syntax check
const UINT c_syntaxCheckTimerId = 64;

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
   SetEOLMode(SC_EOL_CRLF);

   // only notify about text edits in SCEN_CHANGE messages
   SetModEventMask(SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT);
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

LRESULT LuaScriptEditorView::OnChangedText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
   bHandled = false;

   RestartSyntaxCheckTimer();

   return 0;
}

LRESULT LuaScriptEditorView::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   if (wParam == c_syntaxCheckTimerId)
   {
      KillTimer(c_syntaxCheckTimerId);

      CheckSyntax();
   }

   return 0;
}

LRESULT LuaScriptEditorView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   KillTimer(c_syntaxCheckTimerId);

   bHandled = false;
   return 0;
}

void LuaScriptEditorView::RestartSyntaxCheckTimer()
{
   KillTimer(c_syntaxCheckTimerId);
   SetTimer(c_syntaxCheckTimerId, c_syntaxCheckDelayTimeInMs);
}

void LuaScriptEditorView::CheckSyntax()
{
   CStringA text;
   GetText(text);

   int indicatorNumber = INDIC_CONTAINER;

   AnnotationClearAll();

   std::vector<std::string> errorMessages;
   if (LuaState::CheckSyntax(std::string{text}, errorMessages))
   {
      IndicSetStyle(indicatorNumber, INDIC_HIDDEN);
      AnnotationSetVisible(ANNOTATION_HIDDEN);
      return;
   }

   IndicSetStyle(indicatorNumber, INDIC_SQUIGGLE);
   IndicSetFore(indicatorNumber, RGB(255, 0, 0)); // red

   SetIndicatorCurrent(indicatorNumber);

   for (size_t index = 0, maxIndex = errorMessages.size(); index < maxIndex; index++)
   {
      CString errorMessage = errorMessages[0].c_str();

      int pos = errorMessage.Find(_T("]:"));
      int pos2 = errorMessage.Find(_T(':'), pos + 2);

      int lineNumber = _ttoi(errorMessage.Mid(pos + 2, pos2 - (pos + 2)));
      CString error = errorMessage.Mid(pos2 + 1).Trim();

      SetIndicatorValue(index);

      int textStart = static_cast<int>(PositionFromLine(lineNumber - 1));
      int textEnd = GetLineEndPosition(lineNumber - 1);

      IndicatorFillRange(textStart, textEnd - textStart);

      AnnotationSetText(lineNumber - 1, CStringA(error).GetString());
      AnnotationSetVisible(ANNOTATION_BOXED);
   }
}
