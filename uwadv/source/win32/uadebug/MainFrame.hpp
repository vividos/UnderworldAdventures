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
/*! \file MainFrame.hpp

   \brief main application frame

*/
//! \ingroup uadebug

//@{

// include guard
#pragma once

// includes
#include "DebugClient.hpp"
#include "ProjectManager.hpp"
#include "ProjectInfo.hpp"
#include "PlayerInfo.hpp"
#include "ObjectList.hpp"
#include "HotspotList.hpp"
#include "TileMapView.hpp"
#include "Resource.h"

// classes

class CMainFrame :
   public dockwins::CMDIDockingFrameImpl<CMainFrame>,
   public CUpdateUI<CMainFrame>,
   public CMessageFilter,
   public CIdleHandler
{
   typedef dockwins::CMDIDockingFrameImpl<CMainFrame> baseClass;
public:
   DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

   CTabbedMDIClient<CDotNetTabCtrl<CTabViewTabItem> > m_tabbedClient;

   //CMDICommandBarCtrlXP m_CmdBar;
//   CTabbedMDICommandBarCtrl m_CmdBar;
   CTabbedMDICommandBarCtrlXP m_CmdBar;

   CTabbedChildWindow< CDotNetTabCtrl<CTabViewTabItem> > m_tabbedChildWindow;

   CPlayerInfoWindow m_playerInfoWindow;
   CObjectListWindow m_objectListWindow;
   CHotspotListWindow m_hotspotListWindow;
   CProjectInfoWindow m_projectInfoWindow;
 
   CTileMapViewChildFrame m_tilemapChildFrame;

public:
   bool InitDebugClient(void* pDebugClient);

   virtual BOOL PreTranslateMessage(MSG* pMsg);

   virtual BOOL OnIdle()
   {
      UIUpdateToolBar();
      return FALSE;
   }

   BEGIN_MSG_MAP(CMainFrame)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
      COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
      COMMAND_ID_HANDLER(ID_UNDERWORLD_RUNNING, OnButtonUnderworldRunning)
      COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
      COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
      COMMAND_ID_HANDLER(ID_VIEW_PLAYERINFO, OnViewPlayerInfo)
      COMMAND_ID_HANDLER(ID_VIEW_OBJECTLIST, OnViewObjectList)
      COMMAND_ID_HANDLER(ID_VIEW_HOTSPOT, OnViewHotspotList)
      COMMAND_ID_HANDLER(ID_VIEW_PROJECT, OnViewProjectInfo)
      COMMAND_ID_HANDLER(ID_VIEW_TILEMAP, OnViewTilemap)
      COMMAND_ID_HANDLER(ID_VIEW_GAMESTRINGS, OnViewGameStrings)
      COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
      COMMAND_ID_HANDLER(ID_WINDOW_CASCADE, OnWindowCascade)
      COMMAND_ID_HANDLER(ID_WINDOW_TILE_HORZ, OnWindowTileHorizontal)
      COMMAND_ID_HANDLER(ID_WINDOW_TILE_VERT, OnWindowTileVertical)
      COMMAND_ID_HANDLER(ID_WINDOW_ARRANGE, OnWindowArrangeIcons)
      MESSAGE_HANDLER(WM_UNDOCK_WINDOW, OnUndockWindow)
      CHAIN_MDI_CHILD_COMMANDS()
      CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
      CHAIN_MSG_MAP(baseClass)
   END_MSG_MAP()

   BEGIN_UPDATE_UI_MAP(CMainFrame)
      UPDATE_ELEMENT(ID_UNDERWORLD_RUNNING, UPDUI_TOOLBAR)
      UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
      UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
      UPDATE_ELEMENT(ID_VIEW_PLAYERINFO, UPDUI_MENUPOPUP|UPDUI_TOOLBAR)
      UPDATE_ELEMENT(ID_VIEW_OBJECTLIST, UPDUI_MENUPOPUP|UPDUI_TOOLBAR)
      UPDATE_ELEMENT(ID_VIEW_HOTSPOT, UPDUI_MENUPOPUP|UPDUI_TOOLBAR)
      UPDATE_ELEMENT(ID_VIEW_TILEMAP, UPDUI_MENUPOPUP|UPDUI_TOOLBAR)
      UPDATE_ELEMENT(ID_VIEW_GAMESTRINGS, UPDUI_MENUPOPUP|UPDUI_TOOLBAR)
   END_UPDATE_UI_MAP()

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnButtonUnderworldRunning(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnViewPlayerInfo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnViewObjectList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnViewHotspotList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnViewProjectInfo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnViewTilemap(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnViewGameStrings(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

   LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

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

   LRESULT OnUndockWindow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
   CDebugClientInterface m_debugClient;

   bool m_bStopped;

   CProjectManager m_projectManager;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//@}
