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
/// \file LuaSourceView.hpp
/// \brief Lua source file view
//
#pragma once

#include "resource.h"
#include <atlscintilla.h>

class LuaSourceView : public ChildWindowBase<IDR_LUA_SOURCE_FRAME>
{
   typedef LuaSourceView thisClass;
   typedef ChildWindowBase<IDR_LUA_SOURCE_FRAME> baseClass;

public:
   /// ctor
   LuaSourceView() :m_isModified(false)
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
   void SetModified(bool modified) { m_isModified = modified; }

private:
   BEGIN_MSG_MAP(thisClass)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      // TODO MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
      COMMAND_ID_HANDLER(ID_EDIT_CUT, OnEditCut)
      COMMAND_ID_HANDLER(ID_EDIT_COPY, OnEditCopy)
      COMMAND_ID_HANDLER(ID_EDIT_PASTE, OnEditPaste)
      COMMAND_ID_HANDLER(ID_EDIT_UNDO, OnEditUndo)
      COMMAND_ID_HANDLER(ID_EDIT_REDO, OnEditRedo)
      CHAIN_MSG_MAP(baseClass)
   END_MSG_MAP()

   LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   // TODO needed?
   //LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   //{
   //   LPMSG pMsg = (LPMSG)lParam;

   //   if(baseClass::PreTranslateMessage(pMsg))
   //      return TRUE;

   //   return m_view.PreTranslateMessage(pMsg);
   //}

   LRESULT OnEditCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      m_view.Cut();
      return 0;
   }

   LRESULT OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      m_view.Copy();
      return 0;
   }

   LRESULT OnEditPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      m_view.Paste();
      return 0;
   }

   LRESULT OnEditUndo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      m_view.Undo();
      return 0;
   }

   LRESULT OnEditRedo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      m_view.Redo();
      return 0;
   }

   /// updates displayed filename
   void UpdateFilename();

private:
   /// scintilla edit view
   CScintillaWindow m_view;

   /// indicates if file is modified
   bool m_isModified;

   /// filename
   CString m_filename;
};
