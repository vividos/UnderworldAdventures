/*
   Underworld Adventures Debugger - a debugger tool for Underworld Adventures
   Copyright (c) 2005 Michael Fink

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file WatchesList.cpp

   \brief code debugger watches window

*/

// includes
#include "stdatl.hpp"
#include "WatchesList.hpp"

CWatchesListWindow::CWatchesListWindow(unsigned int nCodeDebuggerID)
:CDockingWindowBase(idWatchesWindow),
 m_nCodeDebuggerID(nCodeDebuggerID)
{
}

void CWatchesListWindow::ReceiveNotification(CDebugWindowNotification& notify)
{
   switch (notify.m_enCode)
   {
   case ncUnknown:
   default:
      break;
   }
}


//virtual void CWatchesListWindow::OnUpdatedValue(unsigned int nItem, unsigned int nSubItem, LPCTSTR pszValue);
