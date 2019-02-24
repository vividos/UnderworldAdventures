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
/// \file TileMapView.hpp
/// \brief tilemap view class
//
#pragma once

#include "Resource.h"
#include "TileMapViewCtrl.hpp"

class CMainFrame;
class CDebugClientInterface;

/// child frame for tilemap control
class CTileMapViewChildFrame : public CChildWindowBase<IDR_TILEMAP_VIEW>
{
   typedef CTileMapViewChildFrame thisClass;
   typedef CChildWindowBase<IDR_TILEMAP_VIEW> baseClass;
public:
   /// ctor
   CTileMapViewChildFrame() {}

   /// returns tilemap view control
   const CTileMapViewCtrl& GetTilemapViewCtrl() const { return m_view; }

protected:
   BEGIN_MSG_MAP(thisClass)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
      COMMAND_ROUTE_TO_MEMBER(ID_TILEMAP_ZOOMIN, m_view);
   COMMAND_ROUTE_TO_MEMBER(ID_TILEMAP_ZOOMOUT, m_view);
   CHAIN_MSG_MAP(baseClass)
   END_MSG_MAP()

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL);

      bHandled = FALSE;
      return 1;
   }

   LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      LPMSG pMsg = (LPMSG)lParam;

      if (baseClass::PreTranslateMessage(pMsg))
         return TRUE;

      return FALSE;
   }

   // virtual methods from CChildWindowBase

   virtual void InitDebugWindow(IMainFrame* pMainFrame) override
   {
      baseClass::InitDebugWindow(pMainFrame);
      pMainFrame->AddDebugWindow(&m_view);
   }

   virtual void DoneDebugWindow() override
   {
      m_pMainFrame->RemoveDebugWindow(&m_view);
      baseClass::DoneDebugWindow();
   }

   virtual void ReceiveNotification(CDebugWindowNotification& notify) override
   {
      // relay notification to descendant window, if needed
      if (notify.m_bRelayToDescendants)
         m_pMainFrame->SendNotification(notify, &m_view);
   }

protected:
   /// tilemap control
   CTileMapViewCtrl m_view;
};
