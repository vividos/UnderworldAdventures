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
/// \file LuaSourceWindow.cpp
/// \brief Lua source window
//
#include "pch.hpp"
#include "LuaSourceWindow.hpp"

LRESULT LuaSourceWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL,
      WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, WS_EX_CLIENTEDGE);

   m_view.SetupSourceEditor();

   bHandled = FALSE;
   return 1;
}

LRESULT LuaSourceWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   bHandled = FALSE;
   m_mainFrame->RemoveLuaChildView(this);
   return 0;
}

void LuaSourceWindow::UpdateFilename()
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
