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
/// \file MainFrame.cpp
/// \brief main application frame
//
#include "stdatl.hpp"
#include "aboutdlg.hpp"
#include "MainFrame.hpp"
#include "LuaSourceView.hpp"
#include "GameStringsView.hpp"

LPCTSTR g_luaFileFilter = _T("Lua Source Files (*.lua)\0*.lua\0All Files (*.*)\0*.*\0\0");

bool MainFrame::InitDebugClient(void* debugClient)
{
   // check debug interface version
   if (!m_debugClient.Init(reinterpret_cast<IDebugServer*>(debugClient)))
   {
      ::MessageBox(NULL,
         _T("This version of Underworld Adventures Debugger isn't compatible with the version of ")
         _T("Underworld Adventures you are currently using.\n")
         _T("Please use the appropriate version or update the Underworld Debugger. ")
         _T("The debugger is now exiting."),
         _T("Underworld Adventures Debugger"), MB_OK);

      return false;
   }

   m_isStopped = m_debugClient.IsGamePaused();
   UIEnable(ID_UNDERWORLD_RUN, m_isStopped ? TRUE : FALSE, TRUE);
   UIEnable(ID_UNDERWORLD_PAUSE, m_isStopped ? FALSE : TRUE, TRUE);

   // load project
   CString gameConfigPath = m_debugClient.GetGameConfigPath();

   if (FALSE == gameConfigPath.IsEmpty())
   {
      gameConfigPath += _T("game.cfg");
      m_projectManager.LoadProject(gameConfigPath);
   }

   // get object imagelist
   m_objectImageList = m_debugClient.GetObjectImageList();

   return true;
}

LRESULT MainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   // set caption
   if (m_debugClient.IsStudioMode())
   {
      // TODO set uastudio mode
   }

   // create command bar window
   HWND hWndCmdBar = m_commandBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
   // attach menu
   m_commandBar.AttachMenu(GetMenu());
   // load command bar images
   m_commandBar.LoadImages(IDR_MAINFRAME);
   m_commandBar.LoadImages(IDR_TOOLBAR_STANDARD);
   m_commandBar.LoadImages(IDR_TOOLBAR_DEBUG);
   // remove old menu
   SetMenu(NULL);

   // set up toolbars and status bar
   {
      HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

      CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
      AddSimpleReBarBand(hWndCmdBar);
      AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

      // misc command bar
      HWND hWndMainToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_TOOLBAR_STANDARD, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE, ATL_IDW_TOOLBAR);
      AddSimpleReBarBand(hWndMainToolBar, NULL, FALSE);

      // debug command bar
      HWND hWndDebugToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_TOOLBAR_DEBUG, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE, ATL_IDW_TOOLBAR);
      AddSimpleReBarBand(hWndDebugToolBar, NULL, FALSE);

      CreateSimpleStatusBar();

      UIAddToolBar(hWndToolBar);
      UIAddToolBar(hWndMainToolBar);
      UIAddToolBar(hWndDebugToolBar);
      UISetCheck(ID_VIEW_TOOLBAR, 1);
      UISetCheck(ID_VIEW_TOOLBAR_STANDARD, 1);
      UISetCheck(ID_VIEW_TOOLBAR_DEBUG, 1);
      UISetCheck(ID_VIEW_STATUS_BAR, 1);
   }

   m_tabbedChildWindow.SetReflectNotifications(true);

   m_tabbedChildWindow.SetTabStyles(CTCS_TOOLTIPS | CTCS_SCROLL);
   m_tabbedChildWindow.Create(m_hWnd, rcDefault);
   m_tabbedChildWindow.ModifyStyleEx(WS_EX_CLIENTEDGE, 0);

   CreateMDIClient();

   // subclass mdi client
   m_tabbedClient.SetTabOwnerParent(m_hWnd);
   ATLVERIFY(TRUE == m_tabbedClient.SubclassWindow(m_hWndMDIClient));

   m_commandBar.SetMDIClient(m_hWndMDIClient);

   // register object for message filtering and idle updates
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT(pLoop != NULL);
   pLoop->AddMessageFilter(this);
   pLoop->AddIdleHandler(this);

   // floating stuff
   InitializeDockingFrame();

   // dock some often-needed windows
   ShowHideDockingWindow(m_projectInfoWindow);
   UISetCheck(ID_VIEW_PROJECT, TRUE);

   UpdateLayout();

   return 0;
}

BOOL MainFrame::PreTranslateMessage(MSG* msg)
{
   if (msg == NULL)
      return FALSE;

   bool calledBaseClass = false;

   if (msg->hwnd == m_hWnd || msg->hwnd == m_hWndMDIClient)
   {
      // Message is sent directly to main frame or
      // to the MDIClient window
      if (baseClass::PreTranslateMessage(msg))
         return TRUE;

      calledBaseClass = true;
   }

   HWND hWndFocus = ::GetFocus();
   HWND hWndMDIActive = MDIGetActive();

   if ((hWndMDIActive == hWndFocus) || (::IsChild(hWndMDIActive, hWndFocus)))
   {
      // Message is sent to Active MDI child frame
      // or a descendant
      // NOTE: IsChild checks if the window is a direct child or a descendant

      if (baseClass::PreTranslateMessage(msg))
         return TRUE;

      calledBaseClass = true;

      if (hWndMDIActive != NULL)
      {
         return (BOOL)::SendMessage(hWndMDIActive, WM_FORWARDMSG, 0, (LPARAM)msg);
      }
   }

   if (!calledBaseClass)
   {
      // If the base class hasn't already had a shot at doing
      // PreTranslateMessage (because the main frame or an
      // MDI child didn't have focus), call it now
      if (baseClass::PreTranslateMessage(msg))
         return TRUE;

      // Give active MDI child a chance.
      if (hWndMDIActive != NULL)
      {
         return (BOOL)::SendMessage(hWndMDIActive, WM_FORWARDMSG, 0, (LPARAM)msg);
      }
   }

   return FALSE;
}

BOOL MainFrame::OnIdle()
{
   UIUpdateToolBar();

   ProcessServerMessages();

   return FALSE;
}

void MainFrame::ProcessServerMessages()
{
   // check for new messages
   DebugClientMessage message;
   while (m_debugClient.GetMessage(message))
   {
      ATLTRACE(_T("server message: type=%s\n"),
         message.m_messageType == 0 ? _T("shutdown") :
         message.m_messageType == 1 ? _T("attach") :
         message.m_messageType == 2 ? _T("detach") : _T("unknown"));

      switch (message.m_messageType)
      {
      case 0: // shutdown request
         ATLTRACE(_T("server message: type=shutdown\n"));
         PostMessage(WM_CLOSE);
         break;

      case 1: // debugger attach
      {
         ATLTRACE(_T("server message: type=code debugger attach, id=%u\n"), message.m_messageArg1);
         unsigned int codeDebuggerId = message.m_messageArg1;
         ATLASSERT(FALSE == m_debugClient.IsValidCodeDebuggerId(codeDebuggerId));

         m_debugClient.AddCodeDebugger(codeDebuggerId);

         // prepare debug info
         m_debugClient.GetCodeDebuggerInterface(codeDebuggerId).PrepareDebugInfo();

         // send notification about code debugger update
         DebugWindowNotification notify;
         notify.m_notifyCode = notifyCodeCodeDebuggerUpdate;
         notify.m_param1 = codeDebuggerAttach;
         notify.m_param2 = message.m_messageArg1;
         SendNotification(notify);
      }
      break;

      case 2: // debugger detach
      {
         ATLTRACE(_T("server message: type=code debugger detach, id=%u\n"), message.m_messageArg1);

         ATLASSERT(true == m_debugClient.IsValidCodeDebuggerId(message.m_messageArg1));
         m_debugClient.RemoveCodeDebugger(message.m_messageArg1);

         DebugWindowNotification notify;
         notify.m_notifyCode = notifyCodeCodeDebuggerUpdate;
         notify.m_param1 = codeDebuggerDetach;
         notify.m_param2 = message.m_messageArg1;
         SendNotification(notify);
      }
      break;

      case 3: // code debugger update
      {
         ATLTRACE(_T("server message: type=code debugger update, id=%u\n"), message.m_messageArg1);

         // send notification about code debugger update
         DebugWindowNotification notify;
         notify.m_notifyCode = notifyCodeCodeDebuggerUpdate;
         notify.m_param1 = codeDebuggerUpdateState;
         notify.m_param2 = message.m_messageArg1;
         SendNotification(notify);
      }
      break;

      case 4:
         ATLASSERT(false);
         break;
      }
   }
}

LRESULT MainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   PostMessage(WM_CLOSE);
   return 0;
}

LRESULT MainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   LuaSourceView* child = new LuaSourceView;
   child->CreateEx(m_hWndClient);
   MDIMaximize(child->m_hWnd);
   AddLuaChildView(child);

   child->NewFile();

   return 0;
}

LRESULT MainFrame::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   // ask for filename
   CFileDialog dlg(TRUE, _T(".lua"), NULL, OFN_FILEMUSTEXIST,
      g_luaFileFilter, m_hWnd);

   if (IDOK == dlg.DoModal())
      OpenLuaSourceFile(dlg.m_ofn.lpstrFile);

   return 0;
}

LRESULT MainFrame::OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   HWND hWnd = MDIGetActive();

   // search active Lua child frame
   int maxIndex = m_luaChildWindows.GetSize();
   for (int index = 0; index < maxIndex; index++)
   {
      if (hWnd == m_luaChildWindows[index]->m_hWnd)
      {
         // found window; save file
         m_luaChildWindows[index]->SaveFile();
         break;
      }
   }

   return 0;
}

LRESULT MainFrame::OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   HWND hWnd = MDIGetActive();

   // search active Lua child frame
   int maxIndex = m_luaChildWindows.GetSize();
   for (int index = 0; index < maxIndex; index++)
   {
      if (hWnd == m_luaChildWindows[index]->m_hWnd)
      {
         CString filename = m_luaChildWindows[index]->GetFilename();

         // found window; ask for filename
         CFileDialog dlg(FALSE, _T(".lua"), filename, OFN_OVERWRITEPROMPT,
            g_luaFileFilter, m_hWnd);

         if (IDOK == dlg.DoModal())
            m_luaChildWindows[index]->SaveAs(dlg.m_ofn.lpstrFile);

         break;
      }
   }

   return 0;
}

LRESULT MainFrame::OnFileSaveAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   // go through all Lua child frames and save file when necessary
   int maxIndex = m_luaChildWindows.GetSize();
   for (int index = 0; index < maxIndex; index++)
      if (m_luaChildWindows[index]->IsModified())
         m_luaChildWindows[index]->SaveFile();

   return 0;
}

LRESULT MainFrame::OnGameNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   return 0;
}

LRESULT MainFrame::OnGameOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   return 0;
}

LRESULT MainFrame::OnButtonUnderworldRunPause(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   ATLASSERT(wID == ID_UNDERWORLD_RUN || wID == ID_UNDERWORLD_PAUSE);

   m_isStopped = wID == ID_UNDERWORLD_RUN ? false : true;
   UIEnable(ID_UNDERWORLD_RUN, m_isStopped ? TRUE : FALSE, TRUE);
   UIEnable(ID_UNDERWORLD_PAUSE, m_isStopped ? FALSE : TRUE, TRUE);

   // tell underworld to stop
   m_debugClient.Lock(true);
   m_debugClient.PauseGame(m_isStopped);
   m_debugClient.Lock(false);

   // send notification to all windows
   DebugWindowNotification notify;
   notify.m_notifyCode = m_isStopped ? notifyCodeSetReadonly : notifyCodeSetReadWrite;
   SendNotification(notify);

   // when stopping, also update data
   if (m_isStopped)
   {
      notify.m_notifyCode = notifyCodeUpdateData;
      SendNotification(notify);
   }

   return 0;
}

LRESULT MainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   static BOOL isVisible = TRUE;   // initially visible
   isVisible = !isVisible;

   CReBarCtrl rebar = m_hWndToolBar;
   int bandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);   // toolbar is 2nd added band
   rebar.ShowBand(bandIndex, isVisible);

   UISetCheck(ID_VIEW_TOOLBAR, isVisible);
   UpdateLayout();
   return 0;
}

LRESULT MainFrame::OnViewToolBarStandard(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   static BOOL isVisible = TRUE;   // initially visible
   isVisible = !isVisible;

   CReBarCtrl rebar = m_hWndToolBar;
   int bandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 2);   // toolbar is 3rd added band
   rebar.ShowBand(bandIndex, isVisible);

   UISetCheck(ID_VIEW_TOOLBAR_STANDARD, isVisible);
   UpdateLayout();
   return 0;
}

LRESULT MainFrame::OnViewToolBarDebug(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   static BOOL isVisible = TRUE;   // initially visible
   isVisible = !isVisible;

   CReBarCtrl rebar = m_hWndToolBar;
   int bandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 3);   // toolbar is 4th added band
   rebar.ShowBand(bandIndex, isVisible);

   UISetCheck(ID_VIEW_TOOLBAR_DEBUG, isVisible);
   UpdateLayout();
   return 0;
}

LRESULT MainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   BOOL isVisible = !::IsWindowVisible(m_hWndStatusBar);
   ::ShowWindow(m_hWndStatusBar, isVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
   UISetCheck(ID_VIEW_STATUS_BAR, isVisible);
   UpdateLayout();
   return 0;
}

LRESULT MainFrame::OnViewPlayerInfo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   bool isVisible = ShowHideDockingWindow(m_playerInfoWindow);
   UISetCheck(ID_VIEW_PLAYERINFO, isVisible);
   return 0;
}

LRESULT MainFrame::OnViewObjectList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   bool isVisible = ShowHideDockingWindow(m_objectListWindow);
   UISetCheck(ID_VIEW_OBJECTLIST, isVisible);
   return 0;
}

LRESULT MainFrame::OnViewHotspotList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   bool isVisible = ShowHideDockingWindow(m_hotspotListWindow);
   UISetCheck(ID_VIEW_HOTSPOT, isVisible);
   return 0;
}

LRESULT MainFrame::OnViewTileInfo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   bool isVisible = ShowHideDockingWindow(m_tileInfoWindow);
   UISetCheck(ID_VIEW_TILEINFO, isVisible);
   return 0;
}

LRESULT MainFrame::OnViewProjectInfo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   bool isVisible = ShowHideDockingWindow(m_projectInfoWindow);
   UISetCheck(ID_VIEW_PROJECT, isVisible);
   return 0;
}

LRESULT MainFrame::OnViewTilemap(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   BOOL isVisible = ::IsWindowVisible(m_tilemapChildFrame);

   if (!isVisible)
   {
      const TileMapViewCtrl& tilemap = m_tilemapChildFrame.GetTilemapViewCtrl();

      int cx = tilemap.GetTileSizeX() * 64 + GetSystemMetrics(SM_CXSIZEFRAME) * 2;
      int cy = tilemap.GetTileSizeY() * 64 + GetSystemMetrics(SM_CYSIZEFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION);

      CRect rect(0, 0, cx, cy);
      m_tilemapChildFrame.CreateEx(m_hWndClient, rect);
      AddDebugWindow(&m_tilemapChildFrame);

      // send notifications
      DebugWindowNotification notify;
      notify.m_notifyCode = m_isStopped ? notifyCodeSetReadonly : notifyCodeSetReadWrite;
      notify.m_relayToDescendants = true;

      SendNotification(notify, &m_tilemapChildFrame);

      notify.m_notifyCode = notifyCodeUpdateData;
      SendNotification(notify, &m_tilemapChildFrame);
   }
   else
   {
      RemoveDebugWindow(&m_tilemapChildFrame);
      MDIDestroy(m_tilemapChildFrame.m_hWnd);
   }

   UISetCheck(ID_VIEW_TILEMAP, !isVisible);
   UpdateLayout();
   return 0;
}

LRESULT MainFrame::OnViewGameStrings(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   GameStringsViewChildFrame* child = new GameStringsViewChildFrame;
   child->CreateEx(m_hWndClient);

   AddDebugWindow(child);
   MDIMaximize(child->m_hWnd);

   // send read/write and update data notification
   DebugWindowNotification notify;
   notify.m_notifyCode = m_isStopped ? notifyCodeSetReadonly : notifyCodeSetReadWrite;
   notify.m_relayToDescendants = true;

   SendNotification(notify, child);

   notify.m_notifyCode = notifyCodeUpdateData;
   SendNotification(notify, child);

   return 0;
}

LRESULT MainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   AboutDlg dlg;
   dlg.DoModal();
   return 0;
}

bool MainFrame::ShowHideDockingWindow(DockingWindowBase& dockingWindow)
{
   // determine if docking window is visible
   bool isVisible = dockingWindow.IsWindow() && dockingWindow.IsWindowVisible() &&
      (dockingWindow.IsDocking() || dockingWindow.IsFloating());

   if (isVisible)
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
         CRect rect(CPoint(0, 0), dockingWindow.GetFloatingSize());

         DWORD style = WS_OVERLAPPEDWINDOW | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
         dockingWindow.Create(m_hWnd, rect, dockingWindow.GetDockWindowCaption(), style);
      }

      DockDebugWindow(dockingWindow);

      // update data in control
      DebugWindowNotification notify;
      notify.m_notifyCode = notifyCodeUpdateData;
      notify.m_relayToDescendants = true;
      SendNotification(notify, &dockingWindow);

      // also set window to readonly / writable
      notify.m_notifyCode = m_isStopped ? notifyCodeSetReadonly : notifyCodeSetReadWrite;
      SendNotification(notify, &dockingWindow);
   }

   return !isVisible;
}

LRESULT MainFrame::OnUndockWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
   DockingWindowId id = static_cast<DockingWindowId>(wParam);
   DockingWindowBase& dockingWindow = *reinterpret_cast<DockingWindowBase*>(lParam);

   DockingWindowBase* windowBase = NULL;
   UINT viewId = 0;

   switch (id)
   {
   case idPlayerInfoWindow:
      windowBase = &m_playerInfoWindow;
      viewId = ID_VIEW_PLAYERINFO;
      break;

   case idObjectListWindow:
      windowBase = &m_objectListWindow;
      viewId = ID_VIEW_OBJECTLIST;
      break;

   case idHotspotListWindow:
      windowBase = &m_hotspotListWindow;
      viewId = ID_VIEW_HOTSPOT;
      break;

   case idTileInfoWindow:
      windowBase = &m_tileInfoWindow;
      viewId = ID_VIEW_TILEINFO;
      break;

   case idProjectInfoWindow:
      windowBase = &m_projectInfoWindow;
      viewId = ID_VIEW_PROJECT;
      break;

   case idBreakpointWindow:
      // breakpoint windows are created from ProjectInfoWindow, so we don't
      // know exactly the window base pointer
      windowBase = &dockingWindow;
      viewId = 0;
      break;

   default:
      ATLASSERT(FALSE); // wrong id
      break;
   }

   // window ID must match
   ATLASSERT(id == windowBase->GetWindowId());

   if (viewId != 0)
   {
      bool isFloating = windowBase->IsFloating();

      // TODO check if window is closed when floating
      if (!isFloating)
         UISetCheck(viewId, FALSE);
   }

   UpdateLayout();
   return 0;
}

DebugClient& MainFrame::GetDebugClientInterface()
{
   return m_debugClient;
}

void MainFrame::SendNotification(DebugWindowNotification& notify, DebugWindowBase* debugWindow)
{
   debugWindow->ReceiveNotification(notify);
}

void MainFrame::SendNotification(DebugWindowNotification& notify,
   bool excludeSender, DebugWindowBase* sender)
{
   int maxIndex = m_debugWindowList.GetSize();
   for (int index = 0; index < maxIndex; index++)
   {
      if (excludeSender && sender == m_debugWindowList[index])
         continue;

      SendNotification(notify, m_debugWindowList[index]);
   }
}

CImageList& MainFrame::GetObjectImageList()
{
   return m_objectImageList;
}

void MainFrame::DockDebugWindow(DockingWindowBase& dockingWindow)
{
   CSize dockSize = dockingWindow.GetDockingSize();
   dockwins::CDockingSide dockSide = dockingWindow.GetPreferredDockingSide();

   // TODO search proper bar to dock to
   int bar = 0;

   DockWindow(dockingWindow, dockSide,
      bar, float(0.0)/*fPctPos*/, dockSize.cx, dockSize.cy);
}

void MainFrame::UndockWindow(DockingWindowId windowId, DockingWindowBase* dockingWindow)
{
   PostMessage(WM_UNDOCK_WINDOW, static_cast<WPARAM>(windowId), reinterpret_cast<LPARAM>(dockingWindow));
}

void MainFrame::AddDebugWindow(DebugWindowBase* debugWindow)
{
   ATLASSERT(debugWindow != NULL);

   debugWindow->InitDebugWindow(this);
   m_debugWindowList.Add(debugWindow);
}

void MainFrame::RemoveDebugWindow(DebugWindowBase* debugWindow)
{
   ATLASSERT(debugWindow != NULL);

   debugWindow->DoneDebugWindow();

   int maxIndex = m_debugWindowList.GetSize();
   for (int index = 0; index < maxIndex; index++)
      if (debugWindow == m_debugWindowList[index])
      {
         m_debugWindowList.RemoveAt(index);
         index--;
         maxIndex--;
      }
}

void MainFrame::OpenLuaSourceFile(LPCTSTR filename)
{
   ATLASSERT(filename != NULL);

   // search if file is already open
   CString openFilename(filename);
   openFilename.MakeLower();

   int maxIndex = m_luaChildWindows.GetSize();
   for (int index = 0; index < maxIndex; index++)
   {
      CString windowFilename = m_luaChildWindows[index]->GetFilename();
      windowFilename.MakeLower();

      if (windowFilename == openFilename)
      {
         MDIActivate(m_luaChildWindows[index]->m_hWnd);
         return;
      }
   }

   LuaSourceView* child = new LuaSourceView;
   child->CreateEx(m_hWndClient);
   MDIMaximize(child->m_hWnd);
   AddLuaChildView(child);

   child->OpenFile(filename);
}

void MainFrame::AddLuaChildView(LuaSourceView* childView)
{
   ATLASSERT(childView != NULL);

   childView->InitDebugWindow(this);
   m_luaChildWindows.Add(childView);
}

void MainFrame::RemoveLuaChildView(LuaSourceView* childView)
{
   ATLASSERT(childView != NULL);

   childView->DoneDebugWindow();

   int maxIndex = m_luaChildWindows.GetSize();
   for (int index = 0; index < maxIndex; index++)
      if (childView == m_luaChildWindows[index])
      {
         m_luaChildWindows.RemoveAt(index);
         index--;
         maxIndex--;
      }
}
