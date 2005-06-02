
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
/*! \file EditListViewCtrl.hpp

   \brief editable list view control

*/
//! \ingroup uadebug

//@{

// include guard
#pragma once

// includes

#define WM_DELETEME (WM_USER+11)

// classes

//! in-place edit control for editable list view control
class CEditListInplaceEditCtrl: public CWindowImpl<CEditListInplaceEditCtrl, CEdit>
{
public:
   //! ctor
   CEditListInplaceEditCtrl(int nItem, int nColumn)
      :m_bFinished(false), m_nItem(nItem), m_nColumn(nColumn){}

   // message map
   BEGIN_MSG_MAP(CEditListViewCtrl)
      ATLASSERT_ADDED_REFLECT_NOTIFICATIONS()
      MESSAGE_HANDLER(WM_CHAR, OnChar)
      MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
      MESSAGE_HANDLER(WM_NCDESTROY, OnNcDestroy)
   END_MSG_MAP()

protected:
   bool AcceptChanges();
   void Finish();

   // message handler

   LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnNcDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
   bool m_bFinished;
   int m_nItem, m_nColumn;
};

//! callback class for subitem editing
class IEditListViewCallback
{
public:
   //! dtor
   virtual ~IEditListViewCallback(){}

   //! called when a value was edited
   virtual void OnUpdatedValue(unsigned int nItem, unsigned int nSubItem, LPCTSTR pszValue)=0;
};


//! editable list view control
class CEditListViewCtrl: public CWindowImpl<CEditListViewCtrl, CListViewCtrl>
{
public:
   //! ctor
   CEditListViewCtrl():m_pCallback(NULL), m_bReadonly(false){}

   BEGIN_MSG_MAP(CEditListViewCtrl)
      ATLASSERT_ADDED_REFLECT_NOTIFICATIONS() // checks for forgotten REFLECT_NOTIFICATIONS() macro in base class
      MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLeftButtonDown)
      MESSAGE_HANDLER(WM_DELETEME, OnDeleteMe)
      REFLECTED_NOTIFY_CODE_HANDLER(LVN_BEGINLABELEDIT, OnBeginLabelEdit) // item editing
      REFLECTED_NOTIFY_CODE_HANDLER(LVN_ENDLABELEDIT, OnEndLabelEdit)
   END_MSG_MAP()

   //! initializes edit list view
   void Init(IEditListViewCallback* pCallback = NULL);

   //! sets a columns "editable" flag
   void SetColumnEditable(unsigned int nColumn, bool bEditable = true);

   void SetReadonly(bool bReadonly){ m_bReadonly = bReadonly; }

   LRESULT OnLeftButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnDeleteMe(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnBeginLabelEdit(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
   LRESULT OnEndLabelEdit(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

private:
   //! pointer to callback interface, if any
   IEditListViewCallback* m_pCallback;

   //! array with flags if a column is editable
   CSimpleArray<bool> m_abEditableColumns;

   bool m_bReadonly;
};

//@}
