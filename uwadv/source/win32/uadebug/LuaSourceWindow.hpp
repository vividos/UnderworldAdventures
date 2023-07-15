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
/// \file LuaSourceWindow.hpp
/// \brief Lua source window
//
#pragma once

#include "resource.h"
#include "LuaScriptEditorView.hpp"

/// child window showing a Lua source code editor
class LuaSourceWindow : public ChildWindowBase<IDR_LUA_SOURCE_FRAME>
{
   typedef LuaSourceWindow thisClass;
   typedef ChildWindowBase<IDR_LUA_SOURCE_FRAME> baseClass;

public:
   /// ctor
   LuaSourceWindow()
   {
      m_isDynamicWindow = true;
   }

   /// returns filename
   LPCTSTR GetFilename() const { return m_filename; }

   /// creates a new unnamed file
   void NewFile()
   {
      m_filename = _T("unnamed.lua");
      UpdateFilename();
   }

   /// opens file with given name
   bool OpenFile(LPCTSTR filename)
   {
      m_filename = filename;
      UpdateFilename();
      return m_view.Load(filename);
   }

   /// saves file
   bool SaveFile()
   {
      bool ret = m_view.Save(m_filename);
      SetModified(false);
      UpdateFilename();
      return ret;
   }

   /// saves file under another name
   bool SaveAs(CString newFilename)
   {
      m_filename = newFilename;
      return SaveFile();
   }

   /// returns "modified" state of file
   bool IsModified() const { return m_isModified; }

   /// sets "modified" state of file
   void SetModified(bool isModified) { m_isModified = isModified; }

   /// sets current execution line marker
   void SetCurrentExecutionLine(int lineNumber)
   {
      m_view.SetCurrentExecutionLine(lineNumber);
   }

   /// returns editor view
   const LuaScriptEditorView& GetEditorView() const { return m_view; }

private:
   BEGIN_MSG_MAP(thisClass)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      CHAIN_MSG_MAP(baseClass)
      CHAIN_MSG_MAP_MEMBER(m_view)
      FORWARD_NOTIFICATIONS()
   END_MSG_MAP()

   /// called when the window is created
   LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// called when the window is destroyed
   LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// updates displayed filename
   void UpdateFilename();

private:
   /// Lua script editor view
   LuaScriptEditorView m_view;

   /// indicates if the file is modified
   bool m_isModified = false;

   /// filename
   CString m_filename;
};
