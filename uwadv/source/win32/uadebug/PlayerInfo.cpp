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
/*! \file PlayerInfo.cpp

   \brief player info docking window

*/

// includes
#include "stdatl.hpp"
#include "PlayerInfo.hpp"
#include "DebugClient.hpp"

// data

LPCTSTR g_aszAttrNames[] =
{
   _T("xpos"),
   _T("ypos"),
   _T("zpos"),
   _T("angle"),

   // attributes
   _T("gender"),
   _T("handedness"),
   _T("appearance"),
   _T("profession"),
   _T("maplevel"),
   _T("strength"),
   _T("dexterity"),
   _T("intelligence"),
   _T("life"),
   _T("max_life"),
   _T("mana"),
   _T("max_mana"),
   _T("weariness"),
   _T("hungriness"),
   _T("poisoned"),
   _T("mentalstate"),
   _T("nightvision"),
   _T("talks"),
   _T("kills"),
   _T("level"),
   _T("exp_points"),
   _T("difficulty"),

   // skills
   _T("attack"),
   _T("defense"),
   _T("unarmed"),
   _T("sword"),
   _T("axe"),
   _T("mace"),
   _T("missile"),
   _T("mana"),
   _T("lore"),
   _T("casting"),
   _T("traps"),
   _T("search"),
   _T("track"),
   _T("sneak"),
   _T("repair"),
   _T("charm"),
   _T("picklock"),
   _T("acrobat"),
   _T("appraise"),
   _T("swimming")
};


// CPlayerInfoWindow methods

void CPlayerInfoWindow::UpdateData()
{
   m_pDebugClient->Lock(true);

   _TCHAR szBuffer[32];
   double dVal = 0.0;
   for(unsigned int i=0; i<4; i++)
   {
      dVal = m_pDebugClient->GetPlayerPosInfo(i);
      _sntprintf(szBuffer, 32, _T("%3.2f"), dVal);
      m_listCtrl.SetItemText(i, 1, szBuffer);
   }

   unsigned int max = sizeof(g_aszAttrNames)/sizeof(g_aszAttrNames[0])-4;
   unsigned int iVal = 0;
   for(unsigned int n=0; n<max; n++)
   {
      iVal = m_pDebugClient->GetPlayerAttribute(n);
      _sntprintf(szBuffer, 32, _T("%u"), iVal);
      m_listCtrl.SetItemText(n+4, 1, szBuffer);
   }

   m_pDebugClient->Lock(false);
}


LRESULT CPlayerInfoWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   CRect rcDef;
   GetClientRect(rcDef);
   m_listCtrl.Create(m_hWnd, rcDef, NULL, WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS | LVS_SHOWSELALWAYS );

   m_listCtrl.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

   m_listCtrl.InsertColumn(0, (LPCTSTR)_T("Attribute"), LVCFMT_LEFT, 92, -1);
   m_listCtrl.InsertColumn(1, (LPCTSTR)_T("Value"), LVCFMT_LEFT, 100, -1);

   for(unsigned int i=0; i<sizeof(g_aszAttrNames)/sizeof(g_aszAttrNames[0]); i++)
      m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), g_aszAttrNames[i]);

/*
   m_font.CreateFont(13, 0,0,0,0,0,0,0,0,0,0,0,0, _T("Tahoma"));
   m_listCtrl.SetFont(m_font);
*/
   return 0;
}

LRESULT CPlayerInfoWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   return 0;
}

LRESULT CPlayerInfoWindow::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
   if(wParam != SIZE_MINIMIZED )
   {
      RECT rc;
      GetClientRect(&rc);
      m_listCtrl.SetWindowPos(NULL, &rc ,SWP_NOZORDER | SWP_NOACTIVATE );
   }      
   bHandled = FALSE;
   return 1;
}

LRESULT CPlayerInfoWindow::OnBeginLabelEdit(WPARAM /*wParam*/, NMHDR* pNMHDR, BOOL& /*bHandled*/)
{
   NMLVDISPINFO* pLvDispInfo = (NMLVDISPINFO*)pNMHDR;

   if (pLvDispInfo->item.iSubItem == 0)
   {
      return 1;
   }

   return 0;
}

LRESULT CPlayerInfoWindow::OnEndLabelEdit(WPARAM /*wParam*/, NMHDR* pNMHDR, BOOL& /*bHandled*/)
{
   NMLVDISPINFO* pLvDispInfo = (NMLVDISPINFO*)pNMHDR;
   if (pLvDispInfo->item.iItem == -1 || pLvDispInfo->item.iSubItem != 1)
      return 0;

   LPCTSTR pszText = pLvDispInfo->item.pszText;
   int item = pLvDispInfo->item.iItem;

   m_pDebugClient->Lock(true);

   if (item<4)
   {
      double d = _tcstod(pszText, NULL);
      m_pDebugClient->SetPlayerPosInfo(item, d);
   }
   else
   {
      unsigned long nVal = _tcstoul(pszText, NULL, 10);
      m_pDebugClient->SetPlayerAttribute(item-4, nVal);
   }

   m_pDebugClient->Lock(false);

   return 1;
}

void CPlayerInfoWindow::OnUndocked(HDOCKBAR hBar)
{
   baseClass::OnUndocked(hBar);
   ::PostMessage(GetParent(),WM_UNDOCK_WINDOW, idPlayerInfoWindow, 0);
}
