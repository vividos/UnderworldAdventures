/*
   Underworld Adventures Debugger - a debugger tool for Underworld Adventures
   Copyright (c) 2004,2005 Michael Fink

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
/*! \file WindowBase.hpp

   \brief xxx

*/
//! \ingroup uadebug

//@{

// include guard
#pragma once

// includes

// forward references
class CDebugClientInterface;
class CMainFrame;


#define WM_UNDOCK_WINDOW (WM_USER+10)

enum enDockingWindowID
{
   idPlayerInfoWindow = 1,
   idObjectListWindow,
   idHotspotListWindow,
   idProjectInfoWindow,
};


// classes

//! base class for windows that want to access debugger and main frame
class CDebugWindowBase
{
public:
   //! initializes child window
   void InitDebugWindow(CDebugClientInterface* pDebugClient, CMainFrame* pMainFrame)
   {
      m_pMainFrame = pMainFrame;
      m_pDebugClient = pDebugClient;
   }
protected:
   CDebugClientInterface* m_pDebugClient;
   CMainFrame* m_pMainFrame;
};


//! base class for child windows with tabbing support
class CChildWindowBase:
   public CTabbedMDIChildWindowImpl<CChildWindowBase>,
   public CDebugWindowBase
{
   typedef CChildWindowBase thisClass;
   typedef CTabbedMDIChildWindowImpl<CChildWindowBase> baseClass;

   BEGIN_MSG_MAP(thisClass)
      CHAIN_MSG_MAP(baseClass)
   END_MSG_MAP()
};


//! base class for docking windows
class CDockingWindowBase :
   //public dockwins::CBoxedDockingWindowImpl<CDockingWindowBase,CWindow,dockwins::CVC7LikeExBoxedDockingWindowTraits>,
   public dockwins::CBoxedDockingWindowImpl<CDockingWindowBase, CWindow, dockwins::CVC7LikeExBoxedDockingWindowTraits>,
   public CDebugWindowBase
{
   typedef CDockingWindowBase thisClass;
   //typedef dockwins::CBoxedDockingWindowImpl<CDockingWindowBase,CWindow,dockwins::CVC7LikeExBoxedDockingWindowTraits> baseClass;
   typedef dockwins::CBoxedDockingWindowImpl<CDockingWindowBase, CWindow, dockwins::CVC7LikeExBoxedDockingWindowTraits> baseClass;
public:
   //! ctor
   CDockingWindowBase(enDockingWindowID windowID):m_windowID(windowID){}

   BEGIN_MSG_MAP(thisClass)   
      CHAIN_MSG_MAP(baseClass)      
   END_MSG_MAP()

   bool IsFloating()
   {
      return !m_rcUndock.IsRectEmpty();
   }

   void OnUndocked(HDOCKBAR hBar)
   {
      baseClass::OnUndocked(hBar);
      // notify main frame of undocking
//TODO      m_pMainFrame->OnUndockWindow(m_windowID);
      ::PostMessage(GetParent(),WM_UNDOCK_WINDOW, m_windowID, 0);
   }

protected:
   enDockingWindowID m_windowID;
};

//@}
