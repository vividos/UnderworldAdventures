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
/// \file ChildWindowBase.hpp
/// \brief child debug window base class
//
#pragma once

#include "DebugWindowBase.hpp"

/// base class for child windows with tabbing support
template <unsigned IDR_WINDOW>
class ChildWindowBase :
   public CTabbedMDIChildWindowImpl<ChildWindowBase<IDR_WINDOW>>,
   public DebugWindowBase
{
   typedef ChildWindowBase thisClass;
   typedef CTabbedMDIChildWindowImpl<ChildWindowBase> baseClass;

public:
   /// ctor
   ChildWindowBase()
      :m_isDynamicWindow(false)
   {
   }

   DECLARE_FRAME_WND_CLASS(NULL, IDR_WINDOW)

   /// called when the final message for this window has processed
   virtual void OnFinalMessage(HWND /*hWnd*/)
   {
      if (m_isDynamicWindow)
         delete this;
   }

protected:
   /// indicates if window is dynamically created and should be destroyed on final message
   bool m_isDynamicWindow;
};
