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
/// \file IMainFrame.hpp
/// \brief main frame interface
//
#pragma once

class DebugClient;
class ProjectManager;
class LuaSourceView;
struct DebugWindowNotification;
class DockingWindowBase;

/// interface to main frame
class IMainFrame
{
public:
   /// dtor
   virtual ~IMainFrame() {}

   /// returns debug client interface
   virtual DebugClient& GetDebugClientInterface() = 0;

   /// returns project manager object
   virtual ProjectManager& GetProjectManager() = 0;

   /// returns common image list
   virtual CImageList GetCommonImageList() = 0;

   /// returns if game is currently stopped or running
   virtual bool IsGameStopped() const = 0;

   /// sends notification message to a specific debug application window
   virtual void SendNotification(DebugWindowNotification& notify, class DebugWindowBase* debugWindow) = 0;

   /// sends notification message to all registered debug application windows
   virtual void SendNotification(DebugWindowNotification& notify,
      bool excludeSender = false, class DebugWindowBase* sender = NULL) = 0;

   /// returns image list with all game object images
   virtual CImageList& GetObjectImageList() = 0;

   /// docks window at initial side
   virtual void DockDebugWindow(DockingWindowBase& dockingWindow) = 0;

   /// notifies main frame that window with given id was undocked (hidden)
   virtual void UndockWindow(DockingWindowId windowId, DockingWindowBase* dockingWindow) = 0;

   /// adds debug window to main frame processing
   virtual void AddDebugWindow(class DebugWindowBase* debugWindow) = 0;

   /// removes debug windows from main frame processing
   virtual void RemoveDebugWindow(class DebugWindowBase* window) = 0;

   /// opens Lua source file
   virtual void OpenLuaSourceFile(LPCTSTR filename) = 0;

   /// adds Lua child view to main frame processing
   virtual void AddLuaChildView(LuaSourceView* childView) = 0;

   /// removes Lua child view from main frame processing
   virtual void RemoveLuaChildView(class LuaSourceView* childView) = 0;
};
