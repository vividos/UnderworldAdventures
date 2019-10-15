//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2019 Underworld Adventures Team
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
/// \file TooltipCtrl.hpp
/// \brief Tooltip control
//
#pragma once

/// tooltip control
class TooltipCtrl
{
public:
   /// Adds tooltips for all controls
   void Init(HWND parent)
   {
      m_hwnd = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, 0,
         0, 0, 0, 0, parent, NULL, _Module.GetResourceInstance(), NULL);

      HWND child = ::GetWindow(parent, GW_CHILD);

      char tooltipText[256];

      while (child != NULL)
      {
         int id = ::GetDlgCtrlID(child);

         if (id != -1 && id != 0)
         {
            // try to load text from string table
            tooltipText[0] = 0;
            ::LoadString(NULL, id, tooltipText, 256);

            // when successful, add text as tooltip text
            if (strlen(tooltipText) != 0)
               AddTooltip(child, tooltipText);
         }

         child = GetWindow(child, GW_HWNDNEXT);
      }

      ::SendMessage(m_hwnd, TTM_ACTIVATE, TRUE, 0L);

      // set autopop delay time to 10s
      ::SendMessage(m_hwnd, TTM_SETDELAYTIME, TTDT_AUTOPOP, MAKELPARAM(10 * 1000, 0));
   }

   /// adds tooltip for given control
   void AddTooltip(HWND ctrl, char* text)
   {
      // set up toolinfo struct
      TOOLINFO toolinfo;
      memset(&toolinfo, 0, sizeof(toolinfo));

      toolinfo.cbSize = sizeof(toolinfo);
      toolinfo.uFlags = TTF_IDISHWND;
      toolinfo.hwnd = ::GetParent(ctrl);
      toolinfo.uId = (UINT_PTR)ctrl;
      toolinfo.hinst = _Module.GetResourceInstance();
      toolinfo.lpszText = text;

      ::SendMessageA(m_hwnd, TTM_ADDTOOL, 0, (LPARAM)&toolinfo);
   }

   /// relay mouse move event to tooltip control
   void RelayEvent(MSG* msg)
   {
      ::SendMessage(m_hwnd, TTM_RELAYEVENT, 0, (LPARAM)msg);
   }

   /// cleans up tooltip window
   void Done()
   {
      DestroyWindow(m_hwnd);
      m_hwnd = NULL;
   }

private:
   /// tooltip window handle
   HWND m_hwnd;
};
