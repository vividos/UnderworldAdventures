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
/*! \file GameStringsView.cpp

   \brief game strings view

*/

// includes
#include "stdatl.hpp"
#include "GameStringsView.hpp"
#include "DebugClient.hpp"

// methods

CGameStringsView::~CGameStringsView()
{
   m_listStrings.Detach();
   m_comboBlocks.Detach();
}

LRESULT CGameStringsView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   m_listStrings.Attach(GetDlgItem(IDC_LIST_STRINGS));
   m_comboBlocks.Attach(GetDlgItem(IDC_COMBO_TEXTBLOCK));

   m_listStrings.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);

   CRect rect;
   m_listStrings.GetClientRect(&rect);

   int nWidth = 60;
   int nWidth2 = rect.Width()-nWidth-2-::GetSystemMetrics(SM_CXVSCROLL);

   m_listStrings.InsertColumn(0, _T("Nr."), LVCFMT_LEFT, nWidth, 0);
   m_listStrings.InsertColumn(1, _T("Text"), LVCFMT_LEFT, nWidth2, 0);

   return 0;
}

void CGameStringsView::InitCombobox()
{
   CDebugClientInterface& debugClient = m_pMainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   unsigned int nIndex = 0, nBlock = 0;
   CString cszBlockName;
   while(debugClient.EnumGameStringsBlock(nIndex, nBlock))
   {
      // determine block name
      switch(nBlock)
      {
      case 1: cszBlockName = _T("Basic Game Strings,"); break;
      case 2: cszBlockName = _T("Character creation strings, mantras,"); break;
      case 3: cszBlockName = _T("Wall text/scroll/book/book title strings,"); break;
      case 4: cszBlockName = _T("Object descriptions,"); break;
      case 5: cszBlockName = _T("Object \"look\" descriptions, object quality states,"); break;
      case 6: cszBlockName = _T("Spell names,"); break;
      case 7: cszBlockName = _T("Conversation partner names,"); break;
      case 8: cszBlockName = _T("Text on walls, signs,"); break;
      case 9: cszBlockName = _T("Text trap messages,"); break;
      case 10: cszBlockName = _T("Wall/floor description text,"); break;
      case 24: cszBlockName = _T("Debugging strings,"); break;
      default:
         if (nBlock >= 0x0c00 && nBlock < 0x0e00)
            cszBlockName = _T("Cutscenes");
         else
         if (nBlock >= 0x0e00 && nBlock < 0x0f00)
         {
            CString cszNpcName = debugClient.GetGameString(7, nBlock-0xe00+16);
            cszBlockName.Format(_T("Conversation [%s]"), cszNpcName);
         }
         else
         if (nBlock >= 0x0f00 && nBlock < 0x1000)
            cszBlockName = _T("Conversation (generic)");
         else
            cszBlockName = _T("Unknown");
         break;
      }

      // add block number
      CString cszBlockNr;
      cszBlockNr.Format(_T(" Block %04x (%d)"), nBlock, nBlock);
      cszBlockName += cszBlockNr;

      int nItem = m_comboBlocks.AddString(cszBlockName);
      m_comboBlocks.SetItemData(nItem, nBlock);

      ++nIndex;
   }

   debugClient.Lock(false);

   m_comboBlocks.SetCurSel(0);
   BOOL bDummy = TRUE;
   OnComboSelChange(0,0,0, bDummy);
}

LRESULT CGameStringsView::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
   CSize size = CSize(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
   m_listStrings.MoveWindow(0,40,size.cx+1, size.cy-40, TRUE);

   return 0;
}

LRESULT CGameStringsView::OnComboSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   int nItem = m_comboBlocks.GetCurSel();
   if (nItem == CB_ERR)
      return 0;

   int nBlock = m_comboBlocks.GetItemData(nItem);

   CDebugClientInterface& debugClient = m_pMainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   m_listStrings.LockWindowUpdate();

   m_listStrings.DeleteAllItems();

   CString cszNr, cszText;
   unsigned int max = debugClient.GetGameStringBlockSize(nBlock);
   for(unsigned int n=0; n<max; n++)
   {
      cszNr.Format(_T("%04x (%u)"), n, n);
      int nItem = m_listStrings.InsertItem(m_listStrings.GetItemCount(), cszNr);
      cszText = debugClient.GetGameString(nBlock, n);
      cszText.Replace(_T("\n"), _T("\\n"));
      m_listStrings.SetItemText(nItem, 1, cszText);
   }

   m_listStrings.LockWindowUpdate(FALSE);

   debugClient.Lock(false);

   return 0;
}
