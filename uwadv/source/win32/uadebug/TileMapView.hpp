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
/*! \file TileMapView.hpp

   \brief tilemap view class

*/
//! \ingroup uadebug

//@{

// include guard
#pragma once

// includes
#include "Resource.h"
#include "TileMapViewCtrl.hpp"
#include "EditListViewCtrl.hpp"

// forward references
class CMainFrame;
class CDebugClientInterface;

// classes

class CTileMapViewWindow: public CDialogImpl<CTileMapViewWindow>
{
public:
   CTileMapViewWindow();
   virtual ~CTileMapViewWindow();

   void InitWindow(CMainFrame* pMainFrame){ m_pMainFrame = pMainFrame; }
   void SetClient(CDebugClientInterface* pDebugClient){ m_pDebugClient = pDebugClient; }

   void UpdateData();

   void SelectTile(unsigned int x, unsigned y, bool select=true);

   void UpdateTileInfo();
   void UpdateObjectInfo();

   enum { IDD = IDD_TILEMAPVIEW_FORM };

   BOOL PreTranslateMessage(MSG* pMsg)
   {
      return IsDialogMessage(pMsg);
   }

protected:
   BEGIN_MSG_MAP(CTileMapViewWindow)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
      COMMAND_HANDLER(IDC_BUTTON_BEAM, BN_CLICKED, OnButtonBeam)
   END_MSG_MAP()

   LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   LRESULT OnButtonBeam(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

protected:
   CTileMapViewCtrl m_tileMapView;
   CEditListViewCtrl m_tileInfoList;
   CEditListViewCtrl m_objectList;

   CMainFrame* m_pMainFrame;
   CDebugClientInterface* m_pDebugClient;

   unsigned int m_nTileX, m_nTileY;
};


class CTileMapViewChildFrame:
//   public CMDIChildWindowImpl<CTileMapViewChildFrame>
   public CTabbedMDIChildWindowImpl<CTileMapViewChildFrame>
{
   typedef CTileMapViewChildFrame thisClass;
//   typedef CMDIChildWindowImpl<CTileMapViewChildFrame> baseClass;
   typedef CTabbedMDIChildWindowImpl<CTileMapViewChildFrame> baseClass;
public:
   DECLARE_FRAME_WND_CLASS(NULL, IDR_TILEMAP_VIEW)

   CTileMapViewWindow& GetView(){ return m_view; }

   void UpdateData(){ m_view.UpdateData(); }

protected:
   virtual void OnFinalMessage(HWND /*hWnd*/)
   {
//      delete this;
   }

   BEGIN_MSG_MAP(thisClass)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
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

      if(baseClass::PreTranslateMessage(pMsg))
         return TRUE;

      return m_view.PreTranslateMessage(pMsg);
   }

protected:
   CTileMapViewWindow m_view;
};

//@}
