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

   \brief window base classes

*/
//! \ingroup uadebug

//@{

// include guard
#pragma once

// includes

// forward references
class CDebugClientInterface;
class CMainFrame;
class CProjectManager;
class CDockingWindowBase;


// types

enum T_enDockingWindowID
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

//! notification code for debug app notifications
enum T_enNotifyCode
{
   ncSetReadonly,    //!< is sent when all controls should go into read only mode
   ncSetReadWrite,   //!< is sent when all controls should go to read/write mode again
   ncUpdateData,     //!< is sent when windows should be updated with new values
   ncSelectedObject, //!< is sent when user selected an object
   ncSelectedTile,   //!< is sent when user selected a tile
   ncUpdatedObject,  //!< is sent when object information were updated
   ncUpdatedTile,    //!< is sent when tile information were updated
   ncChangedLevel,   //!< is sent when user changed the current level
   ncCodeDebuggerUpdate,   //!< is sent when a code debugger action occured
   ncUnknown,
};

//! code debugger update type; stored in m_nParam1 field of CDebugWindowNotification
enum T_enCodeDebuggerUpdateType
{
   utAttach=0, //!< code debugger attached; code debugger id is in m_nParam2
   utDetach,   //!< code debugger detached; code debugger id is in m_nParam2
   utUpdateState,   //!< code debugger updated its state; code debugger id is in m_nParam2
};


// structs

//! notification that can be sent to all debug app windows
struct CDebugWindowNotification
{
   //! ctor
   CDebugWindowNotification()
      :m_enCode(ncUnknown), m_nParam1(0), m_nParam2(0), m_bRelayToDescendants(false){}

   //! notify code
   T_enNotifyCode m_enCode;
   //! first param value
   UINT m_nParam1;
   //! second param value
   UINT m_nParam2;

   //! indicates if a window that added a subwindow of itself to the window list should also
   //! get this message
   bool m_bRelayToDescendants;
};


enum T_enCommonImageListImages
{
   enImageFolder = 0,
   enImageLua,
   enImageLevelMap,
   enImageBlankWindow,
   enImageDebugger,
};


// classes and interfaces

//! interface to main frame
class IMainFrame
{
public:
   //! dtor
   virtual ~IMainFrame(){}

   //! returns debug client interface
   virtual CDebugClientInterface& GetDebugClientInterface()=0;

   //! returns project manager object
   virtual CProjectManager& GetProjectManager()=0;

   //! returns common image list
   virtual CImageList GetCommonImageList()=0;

   //! returns if game is currently stopped or running
   virtual bool IsGameStopped() const=0;

   //! sends notification message to a specific debug application window
   virtual void SendNotification(CDebugWindowNotification& notify, class CDebugWindowBase* pDebugWindow)=0;

   //! sends notification message to all registered debug application windows
   virtual void SendNotification(CDebugWindowNotification& notify,
      bool fExcludeSender=false, class CDebugWindowBase* pSender=NULL)=0;

   //! returns image list with all game object images
   virtual CImageList& GetObjectImageList()=0;

   //! docks window at initial side
   virtual void DockDebugWindow(CDockingWindowBase& dockingWindow)=0;

   //! notifies main frame that window with given id was undocked (hidden)
   virtual void UndockWindow(T_enDockingWindowID windowID, CDockingWindowBase* pDockingWindow)=0;

   //! adds debug window to main frame processing
   virtual void AddDebugWindow(class CDebugWindowBase* pDebugWindow)=0;

   //! removes debug windows from main frame processing
   virtual void RemoveDebugWindow(class CDebugWindowBase* pWindow)=0;

   //! opens Lua source file
   virtual void OpenLuaSourceFile(LPCTSTR pszFilename)=0;

   //! adds Lua child view to main frame processing
   virtual void AddLuaChildView(class CLuaSourceView* pChildView)=0;

   //! removes Lua child view from main frame processing
   virtual void RemoveLuaChildView(class CLuaSourceView* pChildView)=0;
};


//! base class for windows that want to access debugger and main frame
class CDebugWindowBase
{
public:
   //! ctor
   CDebugWindowBase():m_pMainFrame(NULL){}
   //! dtor
   virtual ~CDebugWindowBase(){}

   //! initializes debug window
   virtual void InitDebugWindow(IMainFrame* pMainFrame){ m_pMainFrame = pMainFrame; }

   //! cleans up debug window
   virtual void DoneDebugWindow(){ m_pMainFrame = NULL; }

protected:
   //! receives notifications; only CMainFrame may call this function
   virtual void ReceiveNotification(CDebugWindowNotification& /*notify*/){}

protected:
   //! pointer to main frame callback
   IMainFrame* m_pMainFrame;

   friend class CMainFrame;
};


//! base class for child windows with tabbing support
template <unsigned IDR_WINDOW>
class CChildWindowBase:
   public CTabbedMDIChildWindowImpl<CChildWindowBase>,
   public CDebugWindowBase
{
   typedef CChildWindowBase thisClass;
   typedef CTabbedMDIChildWindowImpl<CChildWindowBase> baseClass;

public:
   CChildWindowBase():m_bDynamicWindow(false){}

   DECLARE_FRAME_WND_CLASS(NULL, IDR_WINDOW)

   virtual void OnFinalMessage(HWND /*hWnd*/)
   {
      if (m_bDynamicWindow)
         delete this;
   }

protected:
   bool m_bDynamicWindow;
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


//! base class for docking windows
class CDockingWindowBase :
   public dockwins::CBoxedDockingWindowImpl<CDockingWindowBase, CWindow, dockwins::CVC7LikeExBoxedDockingWindowTraits>,
   public CDebugWindowBase
{
   typedef CDockingWindowBase thisClass;
   typedef dockwins::CBoxedDockingWindowImpl<CDockingWindowBase, CWindow, dockwins::CVC7LikeExBoxedDockingWindowTraits> baseClass;

public:
   //! ctor
   CDockingWindowBase(T_enDockingWindowID windowID): m_windowID(windowID){}
   //! dtor
   virtual ~CDockingWindowBase(){}

   DECLARE_WND_CLASS_EX(_T("DockingWindow"), CS_DBLCLKS, COLOR_WINDOW)

   //! returns window ID
   T_enDockingWindowID GetWindowId() const { return m_windowID; }

   //! returns if window is floating
   bool IsFloating(){ return !m_rcUndock.IsRectEmpty(); }

   //! called when docking window is hidden
   void OnUndocked(HDOCKBAR hBar)
   {
      ATLASSERT(m_pMainFrame != NULL);
      baseClass::OnUndocked(hBar);
      m_pMainFrame->UndockWindow(m_windowID, this);
   }

   // pre virtual methods; implement by using DECLARE_DOCKING_WINDOW or DECLARE_DOCKING_WINDOW_ID macro
   virtual dockwins::CDockingSide GetPreferredDockingSide() const=0;
   virtual CSize GetDockingSize() const=0;
   virtual CSize GetFloatingSize() const=0;
   virtual CString GetDockWindowCaption() const=0;

protected:
   //! docking window id
   T_enDockingWindowID m_windowID;
};

//@}
