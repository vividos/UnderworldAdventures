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
/*! \file MainFrame.cpp

   \brief main application frame

*/

// includes
#include "stdatl.hpp"
#include "aboutdlg.hpp"
#include "MainFrame.hpp"
#include "LuaSourceView.hpp"
#include "GameStringsView.hpp"

// methods

bool CMainFrame::InitDebugClient(void* pDebugClient)
{
   if (!m_debugClient.Init(reinterpret_cast<ua_debug_server_interface*>(pDebugClient)))
   {
      ::MessageBox(NULL,
         _T("This version of Underworld Adventures Debugger isn't compatible to the version of ")
         _T("Underworld Adventures you are currently using.\n")
         _T("Please use the appropriate version or update the Underworld Debugger. ")
         _T("The debugger is now exiting."),
         _T("Underworld Adventures Debugger"), MB_OK);

      return false;
   }

   m_bStopped = false;

   m_playerInfoWindow.SetClient(&m_debugClient);
   m_objectListWindow.SetClient(&m_debugClient);
   m_hotspotListWindow.SetClient(&m_debugClient);

   m_tilemapChildFrame.GetView().SetClient(&m_debugClient);

   // load project
   CString cszGameCfgPath = m_debugClient.GetGameCfgPath();

   if (FALSE == cszGameCfgPath.IsEmpty())
   {
      cszGameCfgPath += _T("game.cfg");
      m_projectManager.LoadProject(cszGameCfgPath);
   }

   UISetCheck(ID_UNDERWORLD_RUNNING, m_bStopped ? FALSE : TRUE);

   return true;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   // create command bar window
   HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
   // attach menu
   m_CmdBar.AttachMenu(GetMenu());
   // load command bar images
   m_CmdBar.LoadImages(IDR_MAINFRAME);
   // remove old menu
   SetMenu(NULL);

   HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

   CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
   AddSimpleReBarBand(hWndCmdBar);
   AddSimpleReBarBand(hWndToolBar, NULL, TRUE);
   CreateSimpleStatusBar();

   m_tabbedChildWindow.SetReflectNotifications(true);

   m_tabbedChildWindow.SetTabStyles(CTCS_TOOLTIPS|CTCS_SCROLL);
   m_tabbedChildWindow.Create(m_hWnd, rcDefault);
   m_tabbedChildWindow.ModifyStyleEx(WS_EX_CLIENTEDGE,0);

   CreateMDIClient();

   // subclass mdi client
   m_tabbedClient.SetTabOwnerParent(m_hWnd);
   BOOL bSubclass = m_tabbedClient.SubclassWindow(m_hWndMDIClient);

   m_CmdBar.SetMDIClient(m_hWndMDIClient);

   UIAddToolBar(hWndToolBar);
   UISetCheck(ID_VIEW_TOOLBAR, 1);
   UISetCheck(ID_VIEW_STATUS_BAR, 1);

   // register object for message filtering and idle updates
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT(pLoop != NULL);
   pLoop->AddMessageFilter(this);
   pLoop->AddIdleHandler(this);


   m_playerInfoWindow.InitWindow(this);
   m_objectListWindow.InitWindow(this);
   m_hotspotListWindow.InitWindow(this);
   m_tilemapChildFrame.GetView().InitWindow(this);

   // floating stuff
   InitializeDockingFrame();

   UpdateLayout();

   return 0;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
   if(pMsg == NULL)
   {
      return FALSE;
   }

   bool bCalledBaseClass = false;

   if(pMsg->hwnd == m_hWnd || pMsg->hwnd == m_hWndMDIClient)
   {
      // Message is sent directly to main frame or
      // to the MDIClient window
      if(baseClass::PreTranslateMessage(pMsg))
         return TRUE;

      bCalledBaseClass = true;
   }

   HWND hWndFocus = ::GetFocus();
   HWND hWndMDIActive = this->MDIGetActive();
   
   if((hWndMDIActive == hWndFocus) || (::IsChild(hWndMDIActive, hWndFocus)))
   {
      // Message is sent to Active MDI child frame
      // or a descendant
      // NOTE: IsChild checks if the window is a direct child or a descendant

      if(baseClass::PreTranslateMessage(pMsg))
         return TRUE;

      bCalledBaseClass = true;

      if(hWndMDIActive != NULL)
      {
         return (BOOL)::SendMessage(hWndMDIActive, WM_FORWARDMSG, 0, (LPARAM)pMsg);
      }
   }
   else
   {
      /*
      for(_PaneWindowIter iter=m_PaneWindows.begin(); iter!=m_PaneWindows.end(); iter++)
      {
         CTabbedAutoHideDockingWindow* pPaneWindow = *iter;
         if(pPaneWindow->IsChild(hWndFocus))
         {
            if(pPaneWindow->PreTranslateMessage(pMsg))
            {
               return TRUE;
            }
         }
      }*/
   }

   if(!bCalledBaseClass)
   {
      // If the base class hasn't already had a shot at doing
      // PreTranslateMessage (because the main frame or an
      // MDI child didn't have focus), call it now
      if(baseClass::PreTranslateMessage(pMsg))
         return TRUE;

      // Give active MDI child a chance.
      if (hWndMDIActive != NULL)
      {
         return (BOOL)::SendMessage(hWndMDIActive, WM_FORWARDMSG, 0, (LPARAM)pMsg);
      }
   }

   return FALSE;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   PostMessage(WM_CLOSE);
   return 0;
}

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CLuaSourceView* pChild = new CLuaSourceView;
   pChild->CreateEx(m_hWndClient);

   return 0;
}

LRESULT CMainFrame::OnButtonUnderworldRunning(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   m_bStopped = !m_bStopped;
   UISetCheck(ID_UNDERWORLD_RUNNING, m_bStopped ? FALSE : TRUE);

   // tell underworld to stop
   m_debugClient.Lock(true);
   m_debugClient.PauseGame(m_bStopped);
   m_debugClient.Lock(false);

   // TODO disable all windows when stopped
   return 0;
}

LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   static BOOL bVisible = TRUE;   // initially visible
   bVisible = !bVisible;
   CReBarCtrl rebar = m_hWndToolBar;
   int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);   // toolbar is 2nd added band
   rebar.ShowBand(nBandIndex, bVisible);
   UISetCheck(ID_VIEW_TOOLBAR, bVisible);
   UpdateLayout();
   return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
   ::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
   UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
   UpdateLayout();
   return 0;
}

LRESULT CMainFrame::OnViewPlayerInfo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   BOOL bVisible = ::IsWindowVisible(m_playerInfoWindow);

   if (!bVisible)
   {
      CRect rcDef(0,0,100,100);
      DWORD dwStyle=WS_OVERLAPPEDWINDOW | WS_POPUP| WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
      m_playerInfoWindow.Create(m_hWnd,rcDef,_T("Player Info"),dwStyle);

      DockWindow(m_playerInfoWindow,dockwins::CDockingSide(dockwins::CDockingSide::sRight),
                  0/*nBar*/,float(0.0)/*fPctPos*/,200/*nWidth*/,100/* nHeight*/);

      if (!m_bStopped)
         m_playerInfoWindow.UpdateData();
   }
   else
   {
      m_playerInfoWindow.Undock();
      m_playerInfoWindow.DestroyWindow();
   }

   UISetCheck(ID_VIEW_PLAYERINFO, !bVisible);
   UpdateLayout();
   return 0;
}

LRESULT CMainFrame::OnViewObjectList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   BOOL bVisible = ::IsWindowVisible(m_objectListWindow);

   if (!bVisible)
   {
      CRect rcDef(0,0,100,100);
      DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
      m_objectListWindow.Create(m_hWnd,rcDef,_T("Master Object List"),dwStyle);

      int nBar = ::IsWindowVisible(m_objectListWindow) ? 1 : 0;

      DockWindow(m_objectListWindow,dockwins::CDockingSide(dockwins::CDockingSide::sBottom),
                  nBar,float(0.0)/*fPctPos*/,200/*nWidth*/,100/* nHeight*/);

      if (!m_bStopped)
         m_objectListWindow.UpdateData();
   }
   else
   {
      m_objectListWindow.Undock();
      m_objectListWindow.DestroyWindow();
   }

   UISetCheck(ID_VIEW_OBJECTLIST, !bVisible);
   UpdateLayout();
   return 0;
}

LRESULT CMainFrame::OnViewHotspotList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   BOOL bVisible = ::IsWindowVisible(m_hotspotListWindow);

   if (!bVisible)
   {
      CRect rcDef(0,0,100,100);
      DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
      m_hotspotListWindow.Create(m_hWnd,rcDef,_T("Master Object List"),dwStyle);

      int nBar = ::IsWindowVisible(m_hotspotListWindow) ? 1 : 0;

      DockWindow(m_hotspotListWindow,dockwins::CDockingSide(dockwins::CDockingSide::sLeft),
                  nBar,float(0.0)/*fPctPos*/,200/*nWidth*/,100/* nHeight*/);
   }
   else
   {
      m_hotspotListWindow.Undock();
      m_hotspotListWindow.DestroyWindow();
   }

   UISetCheck(ID_VIEW_HOTSPOT, !bVisible);
   UpdateLayout();
   return 0;
}

LRESULT CMainFrame::OnViewTilemap(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   BOOL bVisible = ::IsWindowVisible(m_tilemapChildFrame);

   if (!bVisible)
   {
      m_tilemapChildFrame.CreateEx(m_hWndClient);
      if (!m_bStopped)
         m_tilemapChildFrame.UpdateData();
   }
   else
   {
      MDIDestroy(m_tilemapChildFrame.m_hWnd);
//      m_tilemapChildFrame.DestroyWindow();
   }

   UISetCheck(ID_VIEW_TILEMAP, !bVisible);
   UpdateLayout();
   return 0;
}

LRESULT CMainFrame::OnViewGameStrings(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CGameStringsViewChildFrame* pChild = new CGameStringsViewChildFrame;
   pChild->GetView().InitWindow(this);
   pChild->GetView().SetClient(&m_debugClient);

   pChild->CreateEx(m_hWndClient);
   return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CAboutDlg dlg;
   dlg.DoModal();
   return 0;
}

LRESULT CMainFrame::OnUndockWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
   CDockWindowID id = (CDockWindowID)wParam;

   switch(id)
   {
   case idPlayerInfoWindow:
      if (m_playerInfoWindow.m_hWnd != NULL)
         m_playerInfoWindow.DestroyWindow();
      UISetCheck(ID_VIEW_PLAYERINFO, FALSE);
      break;

   case idObjectListWindow:
      if (m_objectListWindow.m_hWnd != NULL)
         m_objectListWindow.DestroyWindow();
      UISetCheck(ID_VIEW_OBJECTLIST, FALSE);
      break;

   case idHotspotListWindow:
      if (m_hotspotListWindow.m_hWnd != NULL)
         m_hotspotListWindow.DestroyWindow();
      UISetCheck(ID_VIEW_HOTSPOT, FALSE);
      break;

   default:
      ATLASSERT(FALSE); // wrong id
      break;
   }

   UpdateLayout();
   return 0;
}
