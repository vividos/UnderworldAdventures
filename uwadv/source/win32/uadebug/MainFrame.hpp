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
/// \file MainFrame.hpp
/// \brief main application frame
//
#pragma once

#include "DebugClient.hpp"
#include "ProjectManager.hpp"
#include "ProjectInfo.hpp"
#include "PlayerInfo.hpp"
#include "ObjectList.hpp"
#include "HotspotList.hpp"
#include "TileMapView.hpp"
#include "TileInfo.hpp"
#include "Resource.h"

#define WM_UNDOCK_WINDOW (WM_APP + 10)

/// debugger app main frame
class CMainFrame :
   public dockwins::CMDIDockingFrameImpl<CMainFrame>,
   public CUpdateUI<CMainFrame>,
   public CMessageFilter,
   public CIdleHandler,
   public IMainFrame
{
   typedef dockwins::CMDIDockingFrameImpl<CMainFrame> baseClass;
public:
   DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

   /// MDI command bar for tabbing, with XP look
   CTabbedMDICommandBarCtrlXP m_CmdBar;

   /// tabbed MDI client window
   CTabbedMDIClient<CDotNetTabCtrl<CTabViewTabItem> > m_tabbedClient;

   /// tabbed child window
   CTabbedChildWindow< CDotNetTabCtrl<CTabViewTabItem> > m_tabbedChildWindow;

   // docking windows

   CPlayerInfoWindow m_playerInfoWindow;
   CObjectListWindow m_objectListWindow;
   CHotspotListWindow m_hotspotListWindow;
   CTileInfoWindow m_tileInfoWindow;
   CProjectInfoWindow m_projectInfoWindow;

   // statically allocated child windows

   CTileMapViewChildFrame m_tilemapChildFrame;

   /// image list with all icons for underworld objects; 16x16
   CImageList m_ilObjects;

public:
   /// initializes debug client of main frame
   bool InitDebugClient(void* pDebugClient);

   virtual BOOL PreTranslateMessage(MSG* pMsg);

   /// called for idle processing
   virtual BOOL OnIdle();

   /// processes all waiting debug server messages
   void ProcessServerMessages();

   // message map
   BEGIN_MSG_MAP(CMainFrame)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
      COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
      COMMAND_ID_HANDLER(ID_FILE_OPEN, OnFileOpen)
      COMMAND_ID_HANDLER(ID_FILE_SAVE, OnFileSave)
      COMMAND_ID_HANDLER(ID_FILE_SAVE_AS, OnFileSaveAs)
      COMMAND_ID_HANDLER(ID_FILE_SAVE_ALL, OnFileSaveAll)
      COMMAND_ID_HANDLER(ID_GAME_NEW, OnGameNew)
      //COMMAND_ID_HANDLER(ID_GAME_OPEN, OnGameOpen)
      COMMAND_ID_HANDLER(ID_UNDERWORLD_RUN, OnButtonUnderworldRunPause)
      COMMAND_ID_HANDLER(ID_UNDERWORLD_PAUSE, OnButtonUnderworldRunPause)
      COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
      COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR_STANDARD, OnViewToolBarStandard)
      COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR_DEBUG, OnViewToolBarDebug)
      COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
      COMMAND_ID_HANDLER(ID_VIEW_PLAYERINFO, OnViewPlayerInfo)
      COMMAND_ID_HANDLER(ID_VIEW_OBJECTLIST, OnViewObjectList)
      COMMAND_ID_HANDLER(ID_VIEW_HOTSPOT, OnViewHotspotList)
      COMMAND_ID_HANDLER(ID_VIEW_TILEINFO, OnViewTileInfo)
      COMMAND_ID_HANDLER(ID_VIEW_PROJECT, OnViewProjectInfo)
      COMMAND_ID_HANDLER(ID_VIEW_TILEMAP, OnViewTilemap)
      COMMAND_ID_HANDLER(ID_VIEW_GAMESTRINGS, OnViewGameStrings)
      COMMAND_ID_HANDLER(ID_WINDOW_CASCADE, OnWindowCascade)
      COMMAND_ID_HANDLER(ID_WINDOW_TILE_HORZ, OnWindowTileHorizontal)
      COMMAND_ID_HANDLER(ID_WINDOW_TILE_VERT, OnWindowTileVertical)
      COMMAND_ID_HANDLER(ID_WINDOW_ARRANGE, OnWindowArrangeIcons)
      COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
      // ID_HELP_INDEX
      MESSAGE_HANDLER(WM_UNDOCK_WINDOW, OnUndockWindow)
      CHAIN_MDI_CHILD_COMMANDS()
      CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
      CHAIN_MSG_MAP(baseClass)
      REFLECT_NOTIFICATIONS()
   END_MSG_MAP()

   // update map for menus and toolbars
   BEGIN_UPDATE_UI_MAP(CMainFrame)
      UPDATE_ELEMENT(ID_UNDERWORLD_RUN, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
      UPDATE_ELEMENT(ID_UNDERWORLD_PAUSE, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
      UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
      UPDATE_ELEMENT(ID_VIEW_TOOLBAR_STANDARD, UPDUI_MENUPOPUP)
      UPDATE_ELEMENT(ID_VIEW_TOOLBAR_DEBUG, UPDUI_MENUPOPUP)
      UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
      UPDATE_ELEMENT(ID_VIEW_PLAYERINFO, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
      UPDATE_ELEMENT(ID_VIEW_OBJECTLIST, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
      UPDATE_ELEMENT(ID_VIEW_HOTSPOT, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
      UPDATE_ELEMENT(ID_VIEW_TILEINFO, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
      UPDATE_ELEMENT(ID_VIEW_PROJECT, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
      UPDATE_ELEMENT(ID_VIEW_TILEMAP, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
      UPDATE_ELEMENT(ID_VIEW_GAMESTRINGS, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
   END_UPDATE_UI_MAP()

   // Handler prototypes (uncomment arguments if needed):
   //   LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   //   LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   //   LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

      // message handler

   LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnFileExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnFileNew(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnFileOpen(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnFileSave(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnFileSaveAs(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnFileSaveAll(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnGameNew(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnGameOpen(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnButtonUnderworldRunPause(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnViewToolBar(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnViewToolBarStandard(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnViewToolBarDebug(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnViewStatusBar(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnViewPlayerInfo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnViewObjectList(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnViewHotspotList(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnViewTileInfo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnViewProjectInfo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnViewTilemap(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnViewGameStrings(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
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
   bool ShowHideDockingWindow(CDockingWindowBase& dockingWindow);

   /// called per PostMessage from UndockWindow() to really undock a docking window
   LRESULT OnUndockWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   // virtual methods from IMainFrame

   virtual CDebugClientInterface& GetDebugClientInterface();
   virtual CProjectManager& GetProjectManager() { return m_projectManager; }
   virtual CImageList GetCommonImageList() { return m_ilCommonImages; }
   virtual bool IsGameStopped() const { return m_bStopped; }
   virtual void SendNotification(CDebugWindowNotification& notify, CDebugWindowBase* pDebugWindow);
   virtual void SendNotification(CDebugWindowNotification& notify,
      bool fExcludeSender = false, CDebugWindowBase* pSender = NULL);
   virtual CImageList& GetObjectImageList();
   virtual void DockDebugWindow(CDockingWindowBase& dockingWindow);
   virtual void UndockWindow(T_enDockingWindowID windowID, CDockingWindowBase* pDockingWindow);
   virtual void AddDebugWindow(CDebugWindowBase* pDebugWindow);
   virtual void RemoveDebugWindow(CDebugWindowBase* pDebugWindow);
   virtual void OpenLuaSourceFile(LPCTSTR pszFilename);
   virtual void AddLuaChildView(CLuaSourceView* pChildView);
   virtual void RemoveLuaChildView(CLuaSourceView* pChildView);

private:
   /// debug client interface
   CDebugClientInterface m_debugClient;

   /// indicates if game is stopped
   bool m_bStopped;

   /// project manager
   CProjectManager m_projectManager;

   /// array with pointer to all debug windows
   CSimpleArray<CDebugWindowBase*> m_apDebugWindows;

   /// array with pointer to all lua child windows
   CSimpleArray<CLuaSourceView*> m_apLuaChildWindows;

   /// common image list
   CImageList m_ilCommonImages;
};
