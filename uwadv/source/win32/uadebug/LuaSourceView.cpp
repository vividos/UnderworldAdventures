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

   m_view.SetInitialTabWidth(3);

   bHandled = FALSE;
   return 1;
}

LRESULT CLuaSourceView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   bHandled = FALSE;
   m_pMainFrame->RemoveLuaChildView(this);
   return 0;
}

void CLuaSourceView::UpdateFilename()
{
   int nPos = m_cszFilename.ReverseFind(_T('\\'));
   CString cszFilename = m_cszFilename.Mid(nPos+1);

   if (IsModified())
      cszFilename += _T("*");

   CString cszTitleName(_T("Lua Source File - "));
   cszTitleName += cszFilename;

   SetTitle(cszTitleName);
   SetTabText(cszFilename);
   SetTabToolTip(m_cszFilename);
}
