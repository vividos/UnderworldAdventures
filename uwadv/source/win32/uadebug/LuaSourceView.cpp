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
/// \file LuaSourceView.cpp
/// \brief Lua source file view
//
#include "pch.hpp"
#include "LuaSourceView.hpp"

LRESULT LuaSourceView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   m_hWndClient = m_view.Create(NULL, m_hWnd, rcDefault, NULL,
      WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, WS_EX_CLIENTEDGE);

   m_view.StyleSetFont(STYLE_DEFAULT, _T("Lucida Console"));
   m_view.SetEdgeColumn(80);
   m_view.SetEdgeMode(EDGE_LINE);

   m_view.StyleSetFore(0, RGB(0x80, 0x80, 0x80));
   m_view.StyleSetFore(1, RGB(0x00, 0x7f, 0x00));
   m_view.StyleSetFore(2, RGB(0x00, 0x7f, 0x00));
   m_view.StyleSetFore(3, RGB(0x7f, 0x7f, 0x7f));
   m_view.StyleSetFore(4, RGB(0x00, 0x7f, 0x7f));
   m_view.StyleSetFore(5, RGB(0x00, 0x00, 0x7f));
   m_view.StyleSetFore(6, RGB(0x7f, 0x00, 0x7f));
   m_view.StyleSetFore(7, RGB(0x7f, 0x00, 0x7f));
   m_view.StyleSetFore(8, RGB(0x00, 0x7f, 0x7f));
   m_view.StyleSetFore(9, RGB(0x7f, 0x7f, 0x7f));
   m_view.StyleSetFore(10, RGB(0x00, 0x00, 0x00));
   m_view.StyleSetFore(11, RGB(0x00, 0x00, 0x00));
   m_view.StyleSetBold(5, true);
   m_view.StyleSetBold(10, true);

   m_view.SetLexer(SCLEX_LUA);

   m_view.SetKeyWords(0,
      "and break do else elseif end for function if in " // Lua keywords
      "local nil not or repeat return then until while "
      "uw player objlist tilemap runes conv quest prop"); // uwadv objects

   m_view.SetTabWidth(3);
   m_view.SetUseTabs(false);

   bHandled = FALSE;
   return 1;
}

LRESULT LuaSourceView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   bHandled = FALSE;
   m_mainFrame->RemoveLuaChildView(this);
   return 0;
}

void LuaSourceView::UpdateFilename()
{
   CFilename fileName(m_filename);
   CString filename = fileName.GetFilename();

   if (IsModified())
      filename += _T("*");

   CString titleText(_T("Lua Source File - "));
   titleText += fileName.Get();

   SetTitle(titleText);
   SetTabText(filename);
   SetTabToolTip(m_filename);
}
