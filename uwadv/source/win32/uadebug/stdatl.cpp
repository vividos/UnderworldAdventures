//
// Underworld Adventures Debugger - a debugger tool for Underworld Adventures
// Copyright (c) 2004,2005,2019 Michael Fink
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
/// \file stdatl.cpp
/// \brief precompiled header support
//
#include "stdatl.hpp"

#if (_ATL_VER < 0x0700)
#include <atlimpl.cpp>
#endif //(_ATL_VER < 0x0700)

#if (_MSC_VER < 1300)
RegisterTabbedMDIMessages g_RegisterTabbedMDIMessages;
#endif

#include <dockimpl.cpp>

/// \brief checks if parent window reflects notifications
/// The check is done by sending a WM_NOTIFY message with notification code
/// 0xFFFF (which surely isn't used by the control) to the parent class and
/// checking if the notification is reflected through an OCM_NOTIFY message to
/// us. If not, the user forgot to add the macro.
void CheckAddedReflectNotifications(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   static bool m_fInCheck = false;
   static bool m_fCheckPassed = false;

   if (!m_fCheckPassed)
   {
      if (m_fInCheck)
      {
         // did we get the proper notification message?
         if (uMsg == OCM_NOTIFY && wParam == (WPARAM)-1 && ((LPNMHDR)lParam)->code == (UINT)-1)
         {
            m_fInCheck = false;
            m_fCheckPassed = true;
         }
      }
      else
      {
         // start check
         m_fInCheck = true;

         // prepare notification
         NMHDR nmhdr;
         nmhdr.code = static_cast<UINT>(-1);
         nmhdr.hwndFrom = hWnd;
         nmhdr.idFrom = static_cast<UINT_PTR>(-1);
         ::SendMessage(::GetParent(hWnd), WM_NOTIFY, (WPARAM)-1, (LPARAM)&nmhdr);

         ATLASSERT(m_fCheckPassed == true); // Warning! forgot to add REFLECT_NOTIFICATIONS() to base class!
         m_fInCheck = false;
      }
   }
}
