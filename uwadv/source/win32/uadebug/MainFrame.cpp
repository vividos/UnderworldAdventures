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

// constants

LPCTSTR g_pszLuaFileFilter = _T("Lua Source Files (*.lua)\0*.lua\0All Files (*.*)\0*.*\0\0");


// CMainFrame methods

bool CMainFrame::InitDebugClient(void* pDebugClient)
{
   // check debug interface version
   if (!m_debugClient.Init(reinterpret_cast<ua_debug_server_interface*>(pDebugClient)))
   {
      ::MessageBox(NULL,
         _T("This version of Underworld Adventures Debugger isn't compatible with the version of ")
         _T("Underworld Adventures you are currently using.\n")
         _T("Please use the appropriate version or update the Underworld Debugger. ")
         _T("The debugger is now exiting."),
         _T("Underworld Adventures Debugger"), MB_OK);

      return false;
   }

   m_bStopped = m_debugClient.IsGamePaused();
   UISetCheck(ID_UNDERWORLD_RUNNING, m_bStopped ? FALSE : TRUE);

   // load project
   CString cszGameCfgPath = m_debugClient.GetGameCfgPath();

   if (FALSE == cszGameCfgPath.IsEmpty())
   {
      cszGameCfgPath += _T("game.cfg");
      m_projectManager.LoadProject(cszGameCfgPath);
   }

   // get object imagelist
   m_ilObjects = m_debugClient.GetObjectImageList();

   return true;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   // set caption
   if (m_debugClient.IsStudioMode())
   {
      // TODO set uastudio mode
   }

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
   ATLVERIFY(TRUE == m_tabbedClient.SubclassWindow(m_hWndMDIClient));

   m_CmdBar.SetMDIClient(m_hWndMDIClient);

   UIAddToolBar(hWndToolBar);
   UISetCheck(ID_VIEW_TOOLBAR, 1);
   UISetCheck(ID_VIEW_STATUS_BAR, 1);

   // register object for message filtering and idle updates
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT(pLoop != NULL);
   pLoop->AddMessageFilter(this);
   pLoop->AddIdleHandler(this);

   // floating stuff
   InitializeDockingFrame();

   // dock some often-needed windows
   ShowHideDockingWindow(m_projectInfoWindow);

   UpdateLayout();

   return 0;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
   if (pMsg == NULL)
      return FALSE;

   bool bCalledBaseClass = false;

   if (pMsg->hwnd == m_hWnd || pMsg->hwnd == m_hWndMDIClient)
   {
      // Message is sent directly to main frame or
      // to the MDIClient window
      if (baseClass::PreTranslateMessage(pMsg))
         return TRUE;

      bCalledBaseClass = true;
   }

   HWND hWndFocus = ::GetFocus();
   HWND hWndMDIActive = MDIGetActive();

   if ((hWndMDIActive == hWndFocus) || (::IsChild(hWndMDIActive, hWndFocus)))
   {
      // Message is sent to Active MDI child frame
      // or a descendant
      // NOTE: IsChild checks if the window is a direct child or a descendant

      if (baseClass::PreTranslateMessage(pMsg))
         return TRUE;

      bCalledBaseClass = true;

      if (hWndMDIActive != NULL)
      {
         return (BOOL)::SendMessage(hWndMDIActive, WM_FORWARDMSG, 0, (LPARAM)pMsg);
      }
   }

   if (!bCalledBaseClass)
   {
      // If the base class hasn't already had a shot at doing
      // PreTranslateMessage (because the main frame or an
      // MDI child didn't have focus), call it now
      if (baseClass::PreTranslateMessage(pMsg))
         return TRUE;

      // Give active MDI child a chance.
      if (hWndMDIActive != NULL)
      {
         return (BOOL)::SendMessage(hWndMDIActive, WM_FORWARDMSG, 0, (LPARAM)pMsg);
      }
   }

   return FALSE;
}

BOOL CMainFrame::OnIdle()
{
   UIUpdateToolBar();

   ProcessServerMessages();

   return FALSE;
}

void CMainFrame::ProcessServerMessages()
{
   // check for new messages
   CDebugClientMessage msg;
   while (m_debugClient.GetMessage(msg))
   {
      switch(msg.m_nType)
      {
      case 0: // shutdown request
         PostMessage(WM_CLOSE);
         break;

      case 1: // debugger attach
         {
            unsigned int nCodeDebuggerID = msg.m_nArg1;
            ATLASSERT(false == m_debugClient.IsValidCodeDebuggerID(nCodeDebuggerID));
            m_debugClient.AddCodeDebugger(nCodeDebuggerID);

            // prepare debug info
            m_debugClient.GetCodeDebuggerInterface(nCodeDebuggerID).PrepareDebugInfo();

            // send notification about code debugger update
            CDebugWindowNotification notify;
            notify.code = ncCodeDebuggerUpdate;
            SendNotification(notify);
         }
         break;

      case 2: // debugger detach
         {
            ATLASSERT(true == m_debugClient.IsValidCodeDebuggerID(msg.m_nArg1));
            m_debugClient.RemoveCodeDebugger(msg.m_nArg1);

            CDebugWindowNotification notify;
            notify.code = ncCodeDebuggerUpdate;
            SendNotification(notify);
         }
         break;
      }
   }
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
   MDIMaximize(pChild->m_hWnd);
   AddLuaChildView(pChild);

   pChild->NewFile();

   return 0;
}

LRESULT CMainFrame::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   // ask for filename
   CFileDialog dlg(TRUE, _T(".lua"), NULL, OFN_FILEMUSTEXIST,
      g_pszLuaFileFilter, m_hWnd);

   if (IDOK == dlg.DoModal())
      OpenLuaSourceFile(dlg.m_ofn.lpstrFile);

   return 0;
}

LRESULT CMainFrame::OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   HWND hWnd = MDIGetActive();

   // search active Lua child frame
   int nMax = m_apLuaChildWindows.GetSize();
   for (int n=0; n<nMax; n++)
   {
      if (hWnd == m_apLuaChildWindows[n]->m_hWnd)
      {
         // found window; save file
         m_apLuaChildWindows[n]->SaveFile();
         break;
      }
   }

   return 0;
}

LRESULT CMainFrame::OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   HWND hWnd = MDIGetActive();

   // search active Lua child frame
   int nMax = m_apLuaChildWindows.GetSize();
   for (int n=0; n<nMax; n++)
   {
      if (hWnd == m_apLuaChildWindows[n]->m_hWnd)
      {
         CString cszFilename = m_apLuaChildWindows[n]->GetFilename();

         // found window; ask for filename
         CFileDialog dlg(FALSE, _T(".lua"), cszFilename, OFN_OVERWRITEPROMPT,
            g_pszLuaFileFilter, m_hWnd);

         if (IDOK == dlg.DoModal())
            m_apLuaChildWindows[n]->SaveAs(dlg.m_ofn.lpstrFile);

         break;
      }
   }

   return 0;
}

LRESULT CMainFrame::OnFileSaveAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   // go through all Lua child frames and save file when necessary
   int nMax = m_apLuaChildWindows.GetSize();
   for (int n=0; n<nMax; n++)
      if (m_apLuaChildWindows[n]->IsModified())
         m_apLuaChildWindows[n]->SaveFile();

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

   // send notification to all windows
   CDebugWindowNotification notify;
   notify.code = m_bStopped ? ncSetReadonly : ncSetReadWrite;
   SendNotification(notify);

   // when stopping, also update data
   if (m_bStopped)
   {
      notify.code = ncUpdateData;
      SendNotification(notify);
   }

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
   bool bVisible = ShowHideDockingWindow(m_playerInfoWindow);
   UISetCheck(ID_VIEW_PLAYERINFO, bVisible);
   return 0;
}

LRESULT CMainFrame::OnViewObjectList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   bool bVisible = ShowHideDockingWindow(m_objectListWindow);
   UISetCheck(ID_VIEW_OBJECTLIST, bVisible);
   return 0;
}

LRESULT CMainFrame::OnViewHotspotList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   bool bVisible = ShowHideDockingWindow(m_hotspotListWindow);
   UISetCheck(ID_VIEW_HOTSPOT, bVisible);
   return 0;
}

LRESULT CMainFrame::OnViewTileInfo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   bool bVisible = ShowHideDockingWindow(m_tileInfoWindow);
   UISetCheck(ID_VIEW_TILEINFO, bVisible);
   return 0;
}

LRESULT CMainFrame::OnViewProjectInfo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   bool bVisible = ShowHideDockingWindow(m_projectInfoWindow);
   UISetCheck(ID_VIEW_PROJECT, bVisible);
   return 0;
}

LRESULT CMainFrame::OnViewTilemap(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   BOOL bVisible = ::IsWindowVisible(m_tilemapChildFrame);

   if (!bVisible)
   {
      const CTileMapViewCtrl& tilemap = m_tilemapChildFrame.GetTilemapViewCtrl();

      int cx = tilemap.GetTileSizeX()*64 + GetSystemMetrics(SM_CXSIZEFRAME)*2;
      int cy = tilemap.GetTileSizeY()*64 + GetSystemMetrics(SM_CYSIZEFRAME)*2 + GetSystemMetrics(SM_CYCAPTION);

      CRect rect(0, 0, cx, cy);
      m_tilemapChildFrame.CreateEx(m_hWndClient, rect);
      AddDebugWindow(&m_tilemapChildFrame);

      // send notifications
      CDebugWindowNotification notify;
      notify.code = m_bStopped ? ncSetReadonly : ncSetReadWrite;
      notify.m_bRelayToDescendants = true;

      SendNotification(notify, &m_tilemapChildFrame);

      notify.code = ncUpdateData;
      SendNotification(notify, &m_tilemapChildFrame);
   }
   else
   {
      RemoveDebugWindow(&m_tilemapChildFrame);
      MDIDestroy(m_tilemapChildFrame.m_hWnd);
   }

   UISetCheck(ID_VIEW_TILEMAP, !bVisible);
   UpdateLayout();
   return 0;
}

LRESULT CMainFrame::OnViewGameStrings(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CGameStringsViewChildFrame* pChild = new CGameStringsViewChildFrame;
   pChild->CreateEx(m_hWndClient);

   AddDebugWindow(pChild);
   MDIMaximize(pChild->m_hWnd);

   // send read/write and update data notification
   CDebugWindowNotification notify;
   notify.code = m_bStopped ? ncSetReadonly : ncSetReadWrite;
   notify.m_bRelayToDescendants = true;

   SendNotification(notify, pChild);

   notify.code = ncUpdateData;
   SendNotification(notify, pChild);

   return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CAboutDlg dlg;
   dlg.DoModal();
   return 0;
}

bool CMainFrame::ShowHideDockingWindow(CDockingWindowBase& dockingWindow)
{
   // determine if docking window is visible
   bool bVisible = dockingWindow.IsWindow() && dockingWindow.IsWindowVisible() &&
       (dockingWindow.IsDocking() || dockingWindow.IsFloating());

   if (bVisible)
   {
      // when docking, undock window, else hide window
      if (dockingWindow.IsDocking())
         dockingWindow.Undock();
      else
         dockingWindow.Hide();
      SetFocus();

      RemoveDebugWindow(&dockingWindow);
   }
   else
   {
      AddDebugWindow(&dockingWindow);

      if (!dockingWindow.IsWindow())
      {
         //dockingWindow.CreateDockingWindow(m_hWnd);
         CRect rect(CPoint(0,0), dockingWindow.GetFloatingSize());
        
         DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
         dockingWindow.Create(m_hWnd, rect, dockingWindow.GetDockWindowCaption(), dwStyle);
      }

      CSize dockSize = dockingWindow.GetDockingSize();
      dockwins::CDockingSide dockSide = dockingWindow.GetPreferredDockingSide();

      // TODO search proper bar to dock to
      int nBar = 0; // TODO ::IsWindowVisible(m_playerInfoWindow) ? 1 : 0;

      DockWindow(dockingWindow, dockSide,
         nBar, float(0.0)/*fPctPos*/, dockSize.cx, dockSize.cy);

      if (!m_bStopped)
      {
         // update data in control
         CDebugWindowNotification notify;
         notify.code = ncUpdateData;
         notify.m_bRelayToDescendants = true;
         SendNotification(notify, &dockingWindow);

         // also set window to readonly / writable
         notify.code = m_bStopped ? ncSetReadonly : ncSetReadWrite;
         SendNotification(notify, &dockingWindow);
      }
   }

   return !bVisible;
}

LRESULT CMainFrame::OnUndockWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   T_enDockingWindowID id = static_cast<T_enDockingWindowID>(wParam);

   CDockingWindowBase* pWindowBase = NULL;
   UINT nViewId = 0;

   switch(id)
   {
   case idPlayerInfoWindow:
      pWindowBase = &m_playerInfoWindow;
      nViewId = ID_VIEW_PLAYERINFO;
      break;

   case idObjectListWindow:
      pWindowBase = &m_objectListWindow;
      nViewId = ID_VIEW_OBJECTLIST;
      break;

   case idHotspotListWindow:
      pWindowBase = &m_hotspotListWindow;
      nViewId = ID_VIEW_HOTSPOT;
      break;

   case idTileInfoWindow:
      pWindowBase = &m_tileInfoWindow;
      nViewId = ID_VIEW_TILEINFO;
      break;

   case idProjectInfoWindow:
      pWindowBase = &m_projectInfoWindow;
      nViewId = ID_VIEW_PROJECT;
      break;

   default:
      ATLASSERT(FALSE); // wrong id
      break;
   }

   // window ID must match
   ATLASSERT(id == pWindowBase->GetWindowId());

   if (nViewId != 0)
   {
      bool bFloating = pWindowBase->IsFloating();

      // TODO check if window is closed when floating
      if (!bFloating)
         UISetCheck(nViewId, FALSE);
   }

   UpdateLayout();
   return 0;
}

CDebugClientInterface& CMainFrame::GetDebugClientInterface()
{
   return m_debugClient;
}

void CMainFrame::SendNotification(CDebugWindowNotification& notify, CDebugWindowBase* pDebugWindow)
{
   pDebugWindow->ReceiveNotification(notify);
}

void CMainFrame::SendNotification(CDebugWindowNotification& notify,
   bool fExcludeSender, CDebugWindowBase* pSender)
{
   int nMax = m_apDebugWindows.GetSize();
   for (int n=0; n<nMax; n++)
   {
      if (fExcludeSender && pSender == m_apDebugWindows[n])
         continue;

      SendNotification(notify, m_apDebugWindows[n]);
   }
}

CImageList& CMainFrame::GetObjectImageList()
{
   return m_ilObjects;
}

void CMainFrame::UndockWindow(T_enDockingWindowID windowID)
{
   PostMessage(WM_UNDOCK_WINDOW, static_cast<WPARAM>(windowID));
}

void CMainFrame::AddDebugWindow(CDebugWindowBase* pDebugWindow)
{
   ATLASSERT(pDebugWindow != NULL);

   pDebugWindow->InitDebugWindow(this);
   m_apDebugWindows.Add(pDebugWindow);
}

void CMainFrame::RemoveDebugWindow(CDebugWindowBase* pDebugWindow)
{
   ATLASSERT(pDebugWindow != NULL);

   pDebugWindow->DoneDebugWindow();

   int nMax = m_apDebugWindows.GetSize();
   for (int n=0; n<nMax; n++)
      if (pDebugWindow == m_apDebugWindows[n])
      {
         m_apDebugWindows.RemoveAt(n);
         n--;
         nMax--;
      }
}

void CMainFrame::OpenLuaSourceFile(LPCTSTR pszFilename)
{
   ATLASSERT(pszFilename != NULL);

   // search if file is already open
   CString cszOpenFilename(pszFilename);
   cszOpenFilename.MakeLower();

   int nMax = m_apLuaChildWindows.GetSize();
   for (int n=0; n<nMax; n++)
   {
      CString cszWindowFilename = m_apLuaChildWindows[n]->GetFilename();
      cszWindowFilename.MakeLower();

      if (cszWindowFilename == cszOpenFilename)
      {
         MDIActivate(m_apLuaChildWindows[n]->m_hWnd);
         return;
      }
   }

   CLuaSourceView* pChild = new CLuaSourceView;
   pChild->CreateEx(m_hWndClient);
   MDIMaximize(pChild->m_hWnd);
   AddLuaChildView(pChild);

   pChild->OpenFile(pszFilename);
}

void CMainFrame::AddLuaChildView(CLuaSourceView* pChildView)
{
   ATLASSERT(pChildView != NULL);

   pChildView->InitDebugWindow(this);
   m_apLuaChildWindows.Add(pChildView);
}

void CMainFrame::RemoveLuaChildView(CLuaSourceView* pChildView)
{
   ATLASSERT(pChildView != NULL);

   pChildView->DoneDebugWindow();

   int nMax = m_apLuaChildWindows.GetSize();
   for (int n=0; n<nMax; n++)
      if (pChildView == m_apLuaChildWindows[n])
      {
         m_apLuaChildWindows.RemoveAt(n);
         n--;
         nMax--;
      }
}
