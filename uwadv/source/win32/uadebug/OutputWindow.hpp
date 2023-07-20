//
// Underworld Adventures Debugger - a debugger tool for Underworld Adventures
// Copyright (c) 2023 Underworld Adventures Team
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
/// \file OutputWindow.hpp
/// \brief output window
//
#pragma once

#include <atlscintilla.h>

/// \brief output window
/// \details a read-only output window for outputting debug text
class OutputWindow :
   public CWindowImpl<OutputWindow, CScintillaWindow>,
   public CScintillaEditCommands<OutputWindow>
{
   /// base class type
   typedef CWindowImpl<OutputWindow, CScintillaWindow> BaseClass;

   /// edit commands mixin type
   typedef CScintillaEditCommands<OutputWindow> EditCommandsClass;

public:
   /// ctor
   OutputWindow()
   {
   }

   /// window class infos
   DECLARE_WND_SUPERCLASS(nullptr, BaseClass::GetWndClassName())

   /// creates new window, with parent
   HWND Create(HWND hWndParent)
   {
      HWND hWnd = BaseClass::Create(hWndParent, rcDefault);

      StyleSetFont(STYLE_DEFAULT, "Lucida Console");
      StyleSetSize(STYLE_DEFAULT, 11);

      SetLexerByName("errorlist");

      SetReadOnly(true);

      return hWnd;
   }

   /// outputs text by appending and scrolling to the output line
   void OutputText(LPCTSTR text)
   {
      CStringA ansiText{ text };

      SetReadOnly(false);
      AppendText(ansiText.GetString(), ansiText.GetLength());
      SetReadOnly(true);

      ScrollToLine(GetLineCount());
   }

private:
   // message map
   BEGIN_MSG_MAP(OutputWindow)
      CHAIN_MSG_MAP_ALT(EditCommandsClass, 1)
   END_MSG_MAP()
};
