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

LRESULT CGameStringsView::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

   m_pDebugClient->Lock(true);

   unsigned int nIndex = 0, nBlock = 0;
   CString cszBlockName;
   while(m_pDebugClient->EnumGameStringsBlock(nIndex, nBlock))
   {
      switch(nBlock)
      {
      case 1: cszBlockName = _T("Basic Game Strings, Block 0001 (1)");
         break;
      case 2: cszBlockName = _T("Character creation strings, mantras, Block 0002 (2)");
         break;
      case 3: cszBlockName = _T("Wall text/scroll/book/book title strings, Block 0003 (3)");
         break;
      case 4: cszBlockName = _T("Object descriptions, Block 0004 (4)");
         break;
      case 5: cszBlockName = _T("Object \"look\" descriptions, object quality states, Block 0005 (5)");
         break;
      case 6: cszBlockName = _T("Spell names, Block 0006 (6)");
         break;
      case 7: cszBlockName = _T("Conversation partner names, Block 0007 (7)");
         break;
      case 8: cszBlockName = _T("Text on walls, signs, Block 0008 (8)");
         break;
      case 9: cszBlockName = _T("Text trap messages, Block 0009 (9)");
         break;
      case 10: cszBlockName = _T("Wall/floor description text, Block 000a (10)");
         break;
      case 18: cszBlockName = _T("Debugging strings, Block 000a (10)");
         break;
      default:
         if (nBlock >= 0x0c00 && nBlock < 0x0e00)
            cszBlockName.Format(_T("Cutscenes Block %04x (%d)"), nBlock, nBlock);
         else
         if (nBlock >= 0x0e00 && nBlock < 0x0f00)
         {
            CString cszNpcName = m_pDebugClient->GetGameString(7, nBlock-0xe00+16);
            cszBlockName.Format(_T("Conversation [%s] Block %04x (%d)"),
               cszNpcName, nBlock, nBlock);
         }
         else
         if (nBlock >= 0x0f00 && nBlock < 0x1000)
         {
            cszBlockName.Format(_T("Conversation (generic) Block %04x (%d)"),
               nBlock, nBlock);
         }
         else
            cszBlockName.Format(_T("Unknown Block %04x (%d)"), nBlock, nBlock);
         break;
      }

      int nItem = m_comboBlocks.AddString(cszBlockName);
      m_comboBlocks.SetItemData(nItem, nBlock);

      ++nIndex;
   }   

   m_pDebugClient->Lock(false);

   m_comboBlocks.SetCurSel(0);

   BOOL fDummy = false;
   OnComboSelChange(0,0,0,fDummy);

   return 0;
}

LRESULT CGameStringsView::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

   m_pDebugClient->Lock(true);

   m_listStrings.LockWindowUpdate();

   m_listStrings.DeleteAllItems();

   CString cszNr, cszText;
   unsigned int max = m_pDebugClient->GetGameStringBlockSize(nBlock);
   for(unsigned int n=0; n<max; n++)
   {
      cszNr.Format(_T("%04x (%u)"), n, n);
      int nItem = m_listStrings.InsertItem(m_listStrings.GetItemCount(), cszNr);
      cszText = m_pDebugClient->GetGameString(nBlock, n);
      cszText.Replace(_T("\n"), _T("\\n"));
      m_listStrings.SetItemText(nItem, 1, cszText);
   }

   m_listStrings.LockWindowUpdate(FALSE);

   m_pDebugClient->Lock(false); 

   return 0;
}
