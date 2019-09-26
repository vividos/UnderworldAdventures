//
// Underworld Adventures Debugger - a debugger tool for Underworld Adventures
// Copyright (c) 2005,2019 Underworld Adventures Team
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
/// \file WatchesListWindow.cpp
/// \brief code debugger watches window
//
#include "pch.hpp"
#include "WatchesListWindow.hpp"

WatchesListWindow::WatchesListWindow(unsigned int codeDebuggerId)
:DockingWindowBase(idWatchesWindow),
 m_codeDebuggerId(codeDebuggerId)
{
}

void WatchesListWindow::ReceiveNotification(DebugWindowNotification& notify)
{
   switch (notify.m_notifyCode)
   {
   case notifyCodeUnknown:
   default:
      break;
   }
}

//void WatchesListWindow::OnUpdatedValue(unsigned int nItem, unsigned int nSubItem, LPCTSTR pszValue);
