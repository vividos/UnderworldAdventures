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
/// \file DockingWindowBase.hpp
/// \brief docking debug window base class
//
#pragma once

#include "DebugWindowBase.hpp"

/// macro to define docking window properties
#define DECLARE_DOCKING_WINDOW(dockcaption, docksize, floatsize, dockside) \
   public: \
      virtual dockwins::CDockingSide GetPreferredDockingSide() const { return dockside; } \
      virtual CSize GetDockingSize() const { return docksize; } \
      virtual CSize GetFloatingSize() const { return floatsize; } \
      virtual CString GetDockWindowCaption() const { return dockcaption; }

/// macro to define docking window properties; same as above but uses string resource id for docking caption
#define DECLARE_DOCKING_WINDOW_ID(dockcaption_id, docksize, floatsize, dockside) \
   public: \
      virtual dockwins::CDockingSide GetPreferredDockingSide() const { return dockside; } \
      virtual CSize GetDockingSize() const { return docksize; } \
      virtual CSize GetFloatingSize() const { return floatsize; } \
      virtual CString GetDockWindowCaption() const { return CString().LoadString(dockcaption_id); }


/// base class for docking windows
class DockingWindowBase :
   public dockwins::CBoxedDockingWindowImpl<DockingWindowBase, CWindow, dockwins::CVC7LikeExBoxedDockingWindowTraits>,
   public DebugWindowBase
{
   typedef DockingWindowBase thisClass;
   typedef dockwins::CBoxedDockingWindowImpl<DockingWindowBase, CWindow, dockwins::CVC7LikeExBoxedDockingWindowTraits> baseClass;

public:
   /// ctor
   DockingWindowBase(DockingWindowId windowId) : m_windowId(windowId) {}
   /// dtor
   virtual ~DockingWindowBase() {}

   DECLARE_WND_CLASS_EX(_T("DockingWindow"), CS_DBLCLKS, COLOR_WINDOW)

   /// returns window ID
   DockingWindowId GetWindowId() const { return m_windowId; }

   /// returns if window is floating
   bool IsFloating() { return !m_rcUndock.IsRectEmpty(); }

   /// called when docking window is hidden
   void OnUndocked(HDOCKBAR hBar)
   {
      ATLASSERT(m_mainFrame != NULL);

      baseClass::OnUndocked(hBar);
      m_mainFrame->UndockWindow(m_windowId, this);
   }

   // pure virtual methods; implement by using DECLARE_DOCKING_WINDOW or DECLARE_DOCKING_WINDOW_ID macro
   virtual dockwins::CDockingSide GetPreferredDockingSide() const = 0;
   virtual CSize GetDockingSize() const = 0;
   virtual CSize GetFloatingSize() const = 0;
   virtual CString GetDockWindowCaption() const = 0;

private:
   /// docking window id
   DockingWindowId m_windowId;
};
