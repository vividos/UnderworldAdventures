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
/// \file GameStringsView.cpp
/// \brief game strings view
//
#include "stdatl.hpp"
#include "GameStringsView.hpp"
#include "DebugClient.hpp"

GameStringsView::~GameStringsView()
{
   m_listStrings.Detach();
   m_comboBlocks.Detach();
}

LRESULT GameStringsView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   m_listStrings.Attach(GetDlgItem(IDC_LIST_STRINGS));
   m_comboBlocks.Attach(GetDlgItem(IDC_COMBO_TEXTBLOCK));

   m_listStrings.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);

   CRect rect;
   m_listStrings.GetClientRect(&rect);

   int width = 60;
   int width2 = rect.Width() - width - 2 - ::GetSystemMetrics(SM_CXVSCROLL);

   m_listStrings.InsertColumn(0, _T("Nr."), LVCFMT_LEFT, width, 0);
   m_listStrings.InsertColumn(1, _T("Text"), LVCFMT_LEFT, width2, 0);

   return 0;
}

void GameStringsView::InitCombobox()
{
   DebugClient& debugClient = m_mainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   unsigned int index = 0, blockNumber = 0;
   CString blockName;
   while (debugClient.EnumGameStringsBlock(index, blockNumber))
   {
      // determine block name
      switch (blockNumber)
      {
      case 1: blockName = _T("Basic Game Strings,"); break;
      case 2: blockName = _T("Character creation strings, mantras,"); break;
      case 3: blockName = _T("Wall text/scroll/book/book title strings,"); break;
      case 4: blockName = _T("Object descriptions,"); break;
      case 5: blockName = _T("Object \"look\" descriptions, object quality states,"); break;
      case 6: blockName = _T("Spell names,"); break;
      case 7: blockName = _T("Conversation partner names,"); break;
      case 8: blockName = _T("Text on walls, signs,"); break;
      case 9: blockName = _T("Text trap messages,"); break;
      case 10: blockName = _T("Wall/floor description text,"); break;
      case 24: blockName = _T("Debugging strings,"); break;
      default:
         if (blockNumber >= 0x0c00 && blockNumber < 0x0e00)
            blockName = _T("Cutscenes");
         else
            if (blockNumber >= 0x0e00 && blockNumber < 0x0f00)
            {
               CString cszNpcName = debugClient.GetGameString(7, blockNumber - 0xe00 + 16);
               blockName.Format(_T("Conversation [%s]"), cszNpcName.GetString());
            }
            else
               if (blockNumber >= 0x0f00 && blockNumber < 0x1000)
                  blockName = _T("Conversation (generic)");
               else
                  blockName = _T("Unknown");
         break;
      }

      // add block number
      CString blockNumText;
      blockNumText.Format(_T(" Block %04x (%d)"), blockNumber, blockNumber);
      blockName += blockNumText;

      int item = m_comboBlocks.AddString(blockName);
      m_comboBlocks.SetItemData(item, blockNumber);

      ++index;
   }

   debugClient.Lock(false);

   m_comboBlocks.SetCurSel(0);
   BOOL dummy = TRUE;
   OnComboSelChange(0, 0, 0, dummy);
}

LRESULT GameStringsView::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
   CSize size = CSize(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
   m_listStrings.MoveWindow(0, 40, size.cx + 1, size.cy - 40, TRUE);

   return 0;
}

LRESULT GameStringsView::OnComboSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   int item = m_comboBlocks.GetCurSel();
   if (item == CB_ERR)
      return 0;

   int blockNumber = m_comboBlocks.GetItemData(item);

   DebugClient& debugClient = m_mainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   m_listStrings.SetRedraw(FALSE);

   m_listStrings.DeleteAllItems();

   CString number, text;
   unsigned int max = debugClient.GetGameStringBlockSize(blockNumber);
   for (unsigned int index = 0; index < max; index++)
   {
      number.Format(_T("%04x (%u)"), index, index);
      int item2 = m_listStrings.InsertItem(m_listStrings.GetItemCount(), number);
      text = debugClient.GetGameString(blockNumber, index);
      text.Replace(_T("\n"), _T("\\n"));
      m_listStrings.SetItemText(item2, 1, text);
   }

   m_listStrings.SetRedraw(TRUE);

   debugClient.Lock(false);

   return 0;
}
