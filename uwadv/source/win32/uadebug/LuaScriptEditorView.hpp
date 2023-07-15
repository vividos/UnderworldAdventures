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
/// \file LuaScriptEditorView.hpp
/// \brief Lua script editor view
//
#pragma once

#include <atlscintilla.h>

/// view for editing Lua scripts
class LuaScriptEditorView :
   public CWindowImpl<LuaScriptEditorView, CScintillaWindow>,
   public CScintillaEditCommands<LuaScriptEditorView>,
   public CScintillaFindReplaceImpl<LuaScriptEditorView>
{
   /// base class type
   typedef CScintillaWindow BaseClass;

   /// edit commands mixin type
   typedef CScintillaEditCommands<LuaScriptEditorView> EditCommandsClass;

   /// find/replace impl type
   typedef CScintillaFindReplaceImpl<LuaScriptEditorView> FindReplaceClass;

public:
   /// window class infos
   DECLARE_WND_SUPERCLASS(nullptr, BaseClass::GetWndClassName());

   /// sets up source editor
   void SetupSourceEditor();

   /// sets current execution line marker
   void SetCurrentExecutionLine(int lineNumber);

   /// returns if there's a text selection in the editor
   bool IsTextSelected() const
   {
      return GetSelectionStart() < GetSelectionEnd();
   }

private:
   BEGIN_MSG_MAP(LuaScriptEditorView)
      COMMAND_CODE_HANDLER(SCEN_CHANGE, OnChangedText)
      MESSAGE_HANDLER(WM_TIMER, OnTimer)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      CHAIN_MSG_MAP_ALT(FindReplaceClass, 1)
      CHAIN_MSG_MAP_ALT(EditCommandsClass, 1)
   END_MSG_MAP()

   // Handler prototypes (uncomment arguments if needed):
   // LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
   // LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
   // LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)

   /// called when text in edit window has changed
   LRESULT OnChangedText(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   /// called when a timer event occurs
   LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// called at destruction of view
   LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// restarts syntax check timer
   void RestartSyntaxCheckTimer();

   /// checks syntax and highlights errors in view
   void CheckSyntax();
};
