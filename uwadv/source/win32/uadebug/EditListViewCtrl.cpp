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
/*! \file EditListViewCtrl.cpp

   \brief editable list view control

*/

// includes
#include "stdatl.hpp"
#include "EditListViewCtrl.hpp"

// CEditListInplaceEditCtrl methods

bool CEditListInplaceEditCtrl::AcceptChanges()
{
   _TCHAR szBuffer[256];
   GetWindowText(szBuffer, 256);

   NMLVDISPINFO dispinfo;
   dispinfo.hdr.hwndFrom = m_hWnd;
   dispinfo.hdr.idFrom = 0;
   dispinfo.hdr.code = LVN_ENDLABELEDIT;

   dispinfo.item.mask = LVIF_TEXT;
   dispinfo.item.iItem = m_nItem;
   dispinfo.item.iSubItem = m_nColumn;
   dispinfo.item.pszText = szBuffer;

   HWND hWnd = ::GetParent(GetParent());
   int nRet = ::SendMessage(hWnd, WM_NOTIFY, 0, (LPARAM)&dispinfo);

   if (nRet == 1)
      ListView_SetItemText(GetParent(), m_nItem, m_nColumn, szBuffer);

   return nRet != 0;
}

void CEditListInplaceEditCtrl::Finish()
{
   if (!m_bFinished)
   {
      m_bFinished = true;
      ::SetFocus(GetParent());
      DestroyWindow();
   }
}

LRESULT CEditListInplaceEditCtrl::OnChar(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
   switch(wParam)
   {
   case VK_RETURN:
      if (!AcceptChanges())
      {
         bHandled = true;
         break;
      }

   case VK_ESCAPE:
      Finish();
      break;

   default:
      bHandled = false;
      break;
   }

   return 0;
}

LRESULT CEditListInplaceEditCtrl::OnKillFocus(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
   if (!m_bFinished)
   {
      AcceptChanges();
      Finish();
   }
   return 0;
}

LRESULT CEditListInplaceEditCtrl::OnNcDestroy(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
//   delete this;
   // TODO delete control
   return 0;
}


// CEditListViewCtrl methods

LRESULT CEditListViewCtrl::OnLeftButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   SetFocus();

   POINT pt;
   pt.x = GET_X_LPARAM(lParam);
   pt.y = GET_Y_LPARAM(lParam);

   UINT flags = 0;
   int item = HitTest(pt, &flags);

   if ((flags & LVHT_ONITEMLABEL) == 0)
      return 0;

   unsigned int xpos = 0;

   int xlparam = GET_X_LPARAM(lParam);
   int column = 0;

   while(xpos < (unsigned)xlparam)
      xpos += GetColumnWidth(column++);
   column--;

   NMLVDISPINFO dispInfo;
   dispInfo.hdr.hwndFrom = m_hWnd;
   dispInfo.hdr.idFrom = -1;
   dispInfo.hdr.code = LVN_BEGINLABELEDIT;
   dispInfo.item.mask = 0;
   dispInfo.item.iItem = item;
   dispInfo.item.iSubItem = column;

   HWND hWnd = GetParent();
   int nRet = SendMessage(hWnd, WM_NOTIFY, 0, (LPARAM)&dispInfo);

   if (nRet != 0)
      return 0;

   // create edit-control
   CEditListInplaceEditCtrl* pEdit = new CEditListInplaceEditCtrl(item,column);

   RECT rect;
   GetItemRect(item, &rect, LVIR_BOUNDS);

   unsigned int startx = 0;
   for(int n=0; n<column; n++)
      startx += GetColumnWidth(n);

   rect.left = startx+6;
   rect.right = startx + GetColumnWidth(column);
   rect.top;
   rect.bottom--;

   _TCHAR szBuffer[256];
   GetItemText(item,column,szBuffer,256);

   pEdit->Create(m_hWnd, rect, _T(""));
   pEdit->SetWindowText(szBuffer);

   pEdit->SetFocus();
   pEdit->SetSel(0,-1);

   HFONT hFont = GetFont();
   pEdit->SetFont(hFont);

   return 0;
}

/*
void CEditListViewCtrl::TraceMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   if (uMsg == WM_NOTIFY)
   {
      LPNMHDR pNMHDR = (LPNMHDR)lParam;
      LPCTSTR pszType = _T("unknown");
      signed long code = (signed long)pNMHDR->code;

      switch(code)
      {
      case NM_OUTOFMEMORY          : pszType = _T("NM_OUTOFMEMORY    "); break;
      case NM_CLICK                : pszType = _T("NM_CLICK          "); break;
      case NM_DBLCLK               : pszType = _T("NM_DBLCLK         "); break;
      case NM_RETURN               : pszType = _T("NM_RETURN         "); break;
      case NM_RCLICK               : pszType = _T("NM_RCLICK         "); break;
      case NM_RDBLCLK              : pszType = _T("NM_RDBLCLK        "); break;
      case NM_SETFOCUS             : pszType = _T("NM_SETFOCUS       "); break;
      case NM_KILLFOCUS            : pszType = _T("NM_KILLFOCUS      "); break;
      case NM_CUSTOMDRAW           : pszType = _T("NM_CUSTOMDRAW     "); break;
      case NM_HOVER                : pszType = _T("NM_HOVER          "); break;
      case NM_NCHITTEST            : pszType = _T("NM_NCHITTEST      "); break;
      case NM_KEYDOWN              : pszType = _T("NM_KEYDOWN        "); break;
      case NM_RELEASEDCAPTURE      : pszType = _T("NM_RELEASEDCAPTURE"); break;
      case NM_SETCURSOR            : pszType = _T("NM_SETCURSOR      "); break;
      case NM_CHAR                 : pszType = _T("NM_CHAR           "); break;

      case LVN_ITEMCHANGING        : pszType = _T("LVN_ITEMCHANGING   "); break;
      case LVN_ITEMCHANGED         : pszType = _T("LVN_ITEMCHANGED    "); break;
      case LVN_INSERTITEM          : pszType = _T("LVN_INSERTITEM     "); break;
      case LVN_DELETEITEM          : pszType = _T("LVN_DELETEITEM     "); break;
      case LVN_DELETEALLITEMS      : pszType = _T("LVN_DELETEALLITEMS "); break;
      case LVN_BEGINLABELEDITA     : pszType = _T("LVN_BEGINLABELEDITA"); break;
      case LVN_BEGINLABELEDITW     : pszType = _T("LVN_BEGINLABELEDITW"); break;
      case LVN_ENDLABELEDITA       : pszType = _T("LVN_ENDLABELEDITA  "); break;
      case LVN_ENDLABELEDITW       : pszType = _T("LVN_ENDLABELEDITW  "); break;
      case LVN_COLUMNCLICK         : pszType = _T("LVN_COLUMNCLICK    "); break;
      case LVN_BEGINDRAG           : pszType = _T("LVN_BEGINDRAG      "); break;
      case LVN_BEGINRDRAG          : pszType = _T("LVN_BEGINRDRAG     "); break;
      }

      ATLTRACE("NOTIFY: %d, %s\n", code, pszType);
   }
   else
   {
      if (uMsg >= WM_USER && uMsg < OCM__BASE)
      {
         ATLTRACE("MESSAGE: WM_USER+0x%04x\n", uMsg-WM_USER);
      }
      else
      if (uMsg == OCM_NOTIFY)
      {
         LPNMHDR pNMHDR = (LPNMHDR)lParam;
         LPCTSTR pszType = _T("unknown");
         signed long code = (signed long)pNMHDR->code;
         switch(code)
         {
         case NM_OUTOFMEMORY          : pszType = _T("NM_OUTOFMEMORY    "); break;
         case NM_CLICK                : pszType = _T("NM_CLICK          "); break;
         case NM_DBLCLK               : pszType = _T("NM_DBLCLK         "); break;
         case NM_RETURN               : pszType = _T("NM_RETURN         "); break;
         case NM_RCLICK               : pszType = _T("NM_RCLICK         "); break;
         case NM_RDBLCLK              : pszType = _T("NM_RDBLCLK        "); break;
         case NM_SETFOCUS             : pszType = _T("NM_SETFOCUS       "); break;
         case NM_KILLFOCUS            : pszType = _T("NM_KILLFOCUS      "); break;
         case NM_CUSTOMDRAW           : pszType = _T("NM_CUSTOMDRAW     "); break;
         case NM_HOVER                : pszType = _T("NM_HOVER          "); break;
         case NM_NCHITTEST            : pszType = _T("NM_NCHITTEST      "); break;
         case NM_KEYDOWN              : pszType = _T("NM_KEYDOWN        "); break;
         case NM_RELEASEDCAPTURE      : pszType = _T("NM_RELEASEDCAPTURE"); break;
         case NM_SETCURSOR            : pszType = _T("NM_SETCURSOR      "); break;
         case NM_CHAR                 : pszType = _T("NM_CHAR           "); break;

         case LVN_ITEMCHANGING        : pszType = _T("LVN_ITEMCHANGING   "); break;
         case LVN_ITEMCHANGED         : pszType = _T("LVN_ITEMCHANGED    "); break;
         case LVN_INSERTITEM          : pszType = _T("LVN_INSERTITEM     "); break;
         case LVN_DELETEITEM          : pszType = _T("LVN_DELETEITEM     "); break;
         case LVN_DELETEALLITEMS      : pszType = _T("LVN_DELETEALLITEMS "); break;
         case LVN_BEGINLABELEDITA     : pszType = _T("LVN_BEGINLABELEDITA"); break;
         case LVN_BEGINLABELEDITW     : pszType = _T("LVN_BEGINLABELEDITW"); break;
         case LVN_ENDLABELEDITA       : pszType = _T("LVN_ENDLABELEDITA  "); break;
         case LVN_ENDLABELEDITW       : pszType = _T("LVN_ENDLABELEDITW  "); break;
         case LVN_COLUMNCLICK         : pszType = _T("LVN_COLUMNCLICK    "); break;
         case LVN_BEGINDRAG           : pszType = _T("LVN_BEGINDRAG      "); break;
         case LVN_BEGINRDRAG          : pszType = _T("LVN_BEGINRDRAG     "); break;
         }

         ATLTRACE("NOTIFY_REFLECT: %d, %s\n", code, pszType);
      }
      else
      if (uMsg >= OCM__BASE && uMsg < OCM__BASE+0x200)
      {
         ATLTRACE("MESSAGE: OCM__BASE+0x%04x\n", uMsg-OCM__BASE);
      }
      else
         ATLTRACE("MESSAGE: %04x\n", uMsg);
   }
}
*/
