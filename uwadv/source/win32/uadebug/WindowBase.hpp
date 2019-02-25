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
/// \file WindowBase.hpp
/// \brief window base classes
//
#pragma once

class DebugClient;
class MainFrame;
class ProjectManager;
class DockingWindowBase;

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
   virtual void AddLuaChildView(class LuaSourceView* childView) = 0;

   /// removes Lua child view from main frame processing
   virtual void RemoveLuaChildView(class LuaSourceView* childView) = 0;
};


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


/// base class for child windows with tabbing support
template <unsigned IDR_WINDOW>
class ChildWindowBase :
   public CTabbedMDIChildWindowImpl<ChildWindowBase<IDR_WINDOW> >,
   public DebugWindowBase
{
   typedef ChildWindowBase thisClass;
   typedef CTabbedMDIChildWindowImpl<ChildWindowBase> baseClass;

public:
   ChildWindowBase()
      :m_isDynamicWindow(false)
   {
   }

   DECLARE_FRAME_WND_CLASS(NULL, IDR_WINDOW)

   virtual void OnFinalMessage(HWND /*hWnd*/)
   {
      if (m_isDynamicWindow)
         delete this;
   }

protected:
   bool m_isDynamicWindow;
};


// macro to define docking window properties
#define DECLARE_DOCKING_WINDOW(dockcaption, docksize, floatsize, dockside) \
   public: \
      virtual dockwins::CDockingSide GetPreferredDockingSide() const { return dockside; } \
      virtual CSize GetDockingSize() const { return docksize; } \
      virtual CSize GetFloatingSize() const { return floatsize; } \
      virtual CString GetDockWindowCaption() const { return dockcaption; }

// macro to define docking window properties; same as above but uses string resource id for docking caption
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

   // pre virtual methods; implement by using DECLARE_DOCKING_WINDOW or DECLARE_DOCKING_WINDOW_ID macro
   virtual dockwins::CDockingSide GetPreferredDockingSide() const = 0;
   virtual CSize GetDockingSize() const = 0;
   virtual CSize GetFloatingSize() const = 0;
   virtual CString GetDockWindowCaption() const = 0;

protected:
   /// docking window id
   DockingWindowId m_windowId;
};
