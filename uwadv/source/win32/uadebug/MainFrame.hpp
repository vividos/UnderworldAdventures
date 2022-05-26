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
/// \file MainFrame.hpp
/// \brief main application frame
//
#pragma once

#include "DebugClient.hpp"
#include "ProjectManager.hpp"
#include "ProjectInfoWindow.hpp"
#include "PlayerInfoWindow.hpp"
#include "ObjectListWindow.hpp"
#include "HotspotListWindow.hpp"
#include "TileMapViewChildFrame.hpp"
#include "TileMap3DEditorView.hpp"
#include "TileInfoWindow.hpp"
#include "resource.h"

#define WM_UNDOCK_WINDOW (WM_APP + 10)

class IDebugServer;

/// main frame base class that supports MDI windows, docking windows and ribbon
template <class T,
   class TBase = CMDIWindow,
   class TWinTraits = dockwins::CDockingFrameTraits>
class ATL_NO_VTABLE CRibbonMDIDockingFrameWindowImpl :
   public dockwins::CDockingFrameImplBase<T, CRibbonMDIFrameWindowImpl<T, TBase, TWinTraits>, TWinTraits>
{
   typedef dockwins::CDockingFrameImplBase<T, CRibbonMDIFrameWindowImpl<T, TBase, TWinTraits>, TWinTraits> baseClass;

public:
   DECLARE_WND_CLASS(_T("CRibbonMDIDockingFrameWindowImpl"))

   /// Returns client rect, with ribbon taken in consideration.
   /// This is done since CDockingFrameImplBase tries to adjust the docking
   /// windows in UpdateLayout() and calls GetClientRect(), but the ribbon is
   /// not part of the non-client area and would get obscured.
   BOOL GetClientRect(LPRECT rect)
   {
      BOOL ret = baseClass::GetClientRect(rect);

      if (this->IsRibbonUI() && !this->IsRibbonHidden())
      {
         rect->top += this->GetRibbonHeight();
      }

      return ret;
   }
};

/// debugger app main frame
class MainFrame :
   public CRibbonMDIDockingFrameWindowImpl<MainFrame>,
   public CMessageFilter,
   public CIdleHandler,
   public IMainFrame
{
   typedef CRibbonMDIDockingFrameWindowImpl<MainFrame> baseClass;

public:
   DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

   /// initializes debug client of main frame
   bool InitDebugClient(IDebugServer* debugServer);

private:
   virtual BOOL PreTranslateMessage(MSG* pMsg);

   /// called for idle processing
   virtual BOOL OnIdle();

   /// processes all waiting debug server messages
   void ProcessServerMessages();

   // message map
   BEGIN_MSG_MAP(MainFrame)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_TIMER, OnTimer)
      COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
      COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
      COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
      COMMAND_ID_HANDLER(ID_FILE_OPEN, OnFileOpen)
      COMMAND_ID_HANDLER(ID_FILE_SAVE, OnFileSave)
      COMMAND_ID_HANDLER(ID_FILE_SAVE_AS, OnFileSaveAs)
      COMMAND_ID_HANDLER(ID_FILE_SAVE_ALL, OnFileSaveAll)
      COMMAND_ID_HANDLER(ID_FILE_NEW_PROJECT, OnFileNewProject)
      COMMAND_ID_HANDLER(ID_FILE_OPEN_PROJECT, OnFileOpenProject)
      COMMAND_ID_HANDLER(ID_UNDERWORLD_RUN, OnButtonUnderworldRunPause)
      COMMAND_ID_HANDLER(ID_UNDERWORLD_PAUSE, OnButtonUnderworldRunPause)
      COMMAND_ID_HANDLER(ID_VIEW_PLAYERINFO, OnViewPlayerInfo)
      COMMAND_ID_HANDLER(ID_VIEW_OBJECTLIST, OnViewObjectList)
      COMMAND_ID_HANDLER(ID_VIEW_HOTSPOT, OnViewHotspotList)
      COMMAND_ID_HANDLER(ID_VIEW_TILEINFO, OnViewTileInfo)
      COMMAND_ID_HANDLER(ID_VIEW_PROJECT, OnViewProjectInfo)
      COMMAND_ID_HANDLER(ID_VIEW_TILEMAP, OnViewTilemap)
      COMMAND_ID_HANDLER(ID_VIEW_TILEMAP_EDITOR, OnViewTilemapEditor)
      COMMAND_ID_HANDLER(ID_VIEW_GAMESTRINGS, OnViewGameStrings)
      COMMAND_ID_HANDLER(ID_DEBUG_PAUSE, OnDebugPause)
      COMMAND_ID_HANDLER(ID_DEBUG_RUN, OnDebugRun)
      COMMAND_ID_HANDLER(ID_DEBUG_STEP_INTO, OnDebugStepInto)
      COMMAND_ID_HANDLER(ID_DEBUG_STEP_OVER, OnDebugStepOver)
      COMMAND_ID_HANDLER(ID_DEBUG_STEP_OUT, OnDebugStepOut)
      COMMAND_ID_HANDLER(ID_WINDOW_CASCADE, OnWindowCascade)
      COMMAND_ID_HANDLER(ID_WINDOW_TILE_HORZ, OnWindowTileHorizontal)
      COMMAND_ID_HANDLER(ID_WINDOW_TILE_VERT, OnWindowTileVertical)
      COMMAND_ID_HANDLER(ID_WINDOW_ARRANGE, OnWindowArrangeIcons)
      MESSAGE_HANDLER(WM_UNDOCK_WINDOW, OnUndockWindow)
      CHAIN_MDI_CHILD_COMMANDS()
      CHAIN_MSG_MAP(baseClass)
      REFLECT_NOTIFICATIONS()
   END_MSG_MAP()

   // update map for menus and toolbars
   BEGIN_UPDATE_UI_MAP(MainFrame)
      UPDATE_ELEMENT(ID_FILE_OPEN_PROJECT, UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_UNDERWORLD_RUN, UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_UNDERWORLD_PAUSE, UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEW_PROJECT, UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEW_PLAYERINFO, UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEW_TILEMAP, UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEW_TILEMAP_EDITOR, UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEW_OBJECTLIST, UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEW_GAMESTRINGS, UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEW_HOTSPOT, UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEW_TILEINFO, UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEW_OUTPUT, UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_DEBUG_PAUSE, UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_DEBUG_RUN, UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_DEBUG_STEP_INTO, UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_DEBUG_STEP_OVER, UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_DEBUG_STEP_OUT, UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_TILEMAP_ZOOMIN, UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_TILEMAP_ZOOMOUT, UPDUI_RIBBON)
   END_UPDATE_UI_MAP()

   LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnFileExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnFileNew(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnFileOpen(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnFileSave(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnFileSaveAs(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnFileSaveAll(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnFileNewProject(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnFileOpenProject(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnButtonUnderworldRunPause(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnViewPlayerInfo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnViewObjectList(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnViewHotspotList(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnViewTileInfo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnViewProjectInfo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnViewTilemap(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnViewTilemapEditor(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnViewGameStrings(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnDebugBreak(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnDebugPause(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnDebugRun(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnDebugStepInto(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnDebugStepOver(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnDebugStepOut(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnAppAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   LRESULT OnWindowCascade(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      MDICascade();
      return 0;
   }

   LRESULT OnWindowTileHorizontal(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      MDITile(MDITILE_HORIZONTAL);
      return 0;
   }

   LRESULT OnWindowTileVertical(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      MDITile(MDITILE_VERTICAL);
      return 0;
   }

   LRESULT OnWindowArrangeIcons(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      MDIIconArrange();
      return 0;
   }

   /// shows or hides docking windows
   bool ShowHideDockingWindow(DockingWindowBase& dockingWindow);

   /// called per PostMessage from UndockWindow() to really undock a docking window
   LRESULT OnUndockWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// starts uastudio.exe
   void StartStudioApp();

   // virtual methods from IMainFrame

   virtual DebugClient& GetDebugClientInterface() override;
   virtual ProjectManager& GetProjectManager() override { return m_projectManager; }
   virtual CImageList GetCommonImageList() override { return m_commonImageList; }
   virtual bool IsGameStopped() const override { return m_isStopped; }
   virtual void SendNotification(DebugWindowNotification& notify, DebugWindowBase* debugWindow) override;
   virtual void SendNotification(DebugWindowNotification& notify,
      bool excludeSender = false, DebugWindowBase* sender = NULL) override;
   virtual CImageList& GetObjectImageList() override;
   virtual void DockDebugWindow(DockingWindowBase& dockingWindow) override;
   virtual void UndockWindow(DockingWindowId windowId, DockingWindowBase* dockingWindow) override;
   virtual void AddDebugWindow(DebugWindowBase* debugWindow) override;
   virtual void RemoveDebugWindow(DebugWindowBase* debugWindow) override;
   virtual LuaSourceWindow& OpenLuaSourceFile(LPCTSTR filename) override;
   virtual void AddLuaChildView(LuaSourceWindow* childView) override;
   virtual void RemoveLuaChildView(LuaSourceWindow* childView) override;

   virtual LuaSourceWindow& OpenConvCodeSourceFile(LPCTSTR filename) override;

   /// called when code debugger state has been updated
   void OnUpdateCodeDebuggerState(DebugWindowNotification& notify);

private:
   /// MDI command bar for tabbing
   CTabbedMDICommandBarCtrl m_commandBar;

   /// tabbed MDI client window
   CTabbedMDIClient<CDotNetTabCtrl<CTabViewTabItem>> m_tabbedClient;

   // docking windows

   PlayerInfoWindow m_playerInfoWindow;
   ObjectListWindow m_objectListWindow;
   HotspotListWindow m_hotspotListWindow;
   TileInfoWindow m_tileInfoWindow;
   ProjectInfoWindow m_projectInfoWindow;

   // statically allocated child windows

   /// 2D tilemap view
   TileMapViewChildFrame m_tilemapChildFrame;

   /// 3D tilemap editor
   Tilemap3DEditorView m_tilemapEditorChildFrame;

   /// debug client interface
   DebugClient m_debugClient;

   /// indicates if game is stopped
   bool m_isStopped;

   /// project manager
   ProjectManager m_projectManager;

   /// array with pointer to all debug windows
   CSimpleArray<DebugWindowBase*> m_debugWindowList;

   /// array with pointer to all lua child windows
   CSimpleArray<LuaSourceWindow*> m_luaChildWindows;

   /// common image list
   CImageList m_commonImageList;

   /// image list with all icons for underworld objects; 16x16
   CImageList m_objectImageList;
};
