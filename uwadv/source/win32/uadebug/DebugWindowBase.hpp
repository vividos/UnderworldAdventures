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
/// \file DebugWindowBase.hpp
/// \brief debug window base classes
//
#pragma once

enum DockingWindowId
{
   idPlayerInfoWindow = 1,
   idObjectListWindow,
   idHotspotListWindow,
   idTileInfoWindow,
   idObjectInfoWindow,
   idBreakpointWindow,
   idWatchesWindow,
   idProjectInfoWindow,
};

/// notification code for debug app notifications
enum T_enNotifyCode
{
   notifyCodeSetReadonly,    ///< is sent when all controls should go into read only mode
   notifyCodeSetReadWrite,   ///< is sent when all controls should go to read/write mode again
   notifyCodeUpdateData,     ///< is sent when windows should be updated with new values
   notifyCodeSelectedObject, ///< is sent when user selected an object
   notifyCodeSelectedTile,   ///< is sent when user selected a tile
   notifyCodeUpdatedObject,  ///< is sent when object information were updated
   notifyCodeUpdatedTile,    ///< is sent when tile information were updated
   notifyCodeChangedLevel,   ///< is sent when user changed the current level
   notifyCodeCodeDebuggerUpdate,   ///< is sent when a code debugger action occured
   notifyCodeUnknown,
};

/// code debugger update type; stored in m_param1 field of DebugWindowNotification
enum CodeDebuggerUpdateType
{
   codeDebuggerAttach = 0, ///< code debugger attached; code debugger id is in m_param2
   codeDebuggerDetach,   ///< code debugger detached; code debugger id is in m_param2
   codeDebuggerUpdateState,   ///< code debugger updated its state; code debugger id is in m_param2
};

/// notification that can be sent to all debug app windows
struct DebugWindowNotification
{
   /// ctor
   DebugWindowNotification()
      :m_notifyCode(notifyCodeUnknown),
      m_param1(0),
      m_param2(0),
      m_relayToDescendants(false)
   {
   }

   /// notify code
   T_enNotifyCode m_notifyCode;
   /// first param value
   UINT m_param1;
   /// second param value
   UINT m_param2;

   /// indicates if a window that added a subwindow of itself to the window list should also
   /// get this message
   bool m_relayToDescendants;
};


enum CommonImageListImages
{
   imageFolder = 0,
   imageLua,
   imageLevelMap,
   imageBlankWindow,
   imageDebugger,
};

#include "IMainFrame.hpp"

/// base class for windows that want to access debugger and main frame
class DebugWindowBase
{
public:
   /// ctor
   DebugWindowBase()
      :m_mainFrame(NULL)
   {
   }
   /// dtor
   virtual ~DebugWindowBase() {}

   /// initializes debug window
   virtual void InitDebugWindow(IMainFrame* mainFrame) { m_mainFrame = mainFrame; }

   /// cleans up debug window
   virtual void DoneDebugWindow() { m_mainFrame = NULL; }

protected:
   /// receives notifications; only MainFrame may call this function
   virtual void ReceiveNotification(DebugWindowNotification& /*notify*/) {}

protected:
   /// pointer to main frame callback
   IMainFrame* m_mainFrame;

   friend class MainFrame;
};
