//
// Underworld Adventures Debugger - a debugger tool for Underworld Adventures
// Copyright (c) 2004,2005,2006,2019 Michael Fink
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
/// \file EditListViewCtrl.hpp
/// \brief editable list view control
//
#pragma once

#define WM_DELETEME (WM_USER+11)

typedef CWinTraits<WS_VISIBLE | WS_CHILD, 0> EditListInplaceEditCtrlTraits;

/// in-place edit control for editable list view control
class EditListInplaceEditCtrl : public CWindowImpl<EditListInplaceEditCtrl, CEdit, EditListInplaceEditCtrlTraits>
{
public:
   /// ctor
   EditListInplaceEditCtrl(int item, int columnIndex)
      :m_isFinished(false),
      m_item(item),
      m_columnIndex(columnIndex)
   {
   }

   BEGIN_MSG_MAP(EditListInplaceEditCtrl)
      ATLASSERT_ADDED_REFLECT_NOTIFICATIONS()
      MESSAGE_HANDLER(WM_CHAR, OnChar)
      MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
      MESSAGE_HANDLER(WM_NCDESTROY, OnNcDestroy)
   END_MSG_MAP()

protected:
   bool AcceptChanges();
   void Finish();

   LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnNcDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
   bool m_isFinished;
   int m_item, m_columnIndex;
};

/// callback class for subitem editing
class IEditListViewCallback
{
public:
   /// dtor
   virtual ~IEditListViewCallback() {}

   /// called when a value was edited
   virtual void OnUpdatedValue(unsigned int item, unsigned int subItem, LPCTSTR value) = 0;
};


/// editable list view control
class EditListViewCtrl : public CWindowImpl<EditListViewCtrl, CListViewCtrl>
{
public:
   /// ctor
   EditListViewCtrl()
      :m_callback(NULL), m_isReadOnly(false)
   {
   }

   BEGIN_MSG_MAP(EditListViewCtrl)
      ATLASSERT_ADDED_REFLECT_NOTIFICATIONS() // checks for forgotten REFLECT_NOTIFICATIONS() macro in base class
      MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLeftButtonDown)
      MESSAGE_HANDLER(WM_DELETEME, OnDeleteMe)
      REFLECTED_NOTIFY_CODE_HANDLER(LVN_BEGINLABELEDIT, OnBeginLabelEdit) // item editing
      REFLECTED_NOTIFY_CODE_HANDLER(LVN_ENDLABELEDIT, OnEndLabelEdit)
   END_MSG_MAP()

   /// initializes edit list view
   void Init(IEditListViewCallback* callback = NULL);

   /// sets a columns "editable" flag
   void SetColumnEditable(unsigned int columnIndex, bool isEditable = true);

   void SetReadOnly(bool readOnly) { m_isReadOnly = readOnly; }

   LRESULT OnLeftButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnDeleteMe(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnBeginLabelEdit(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
   LRESULT OnEndLabelEdit(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

private:
   /// pointer to callback interface, if any
   IEditListViewCallback* m_callback;

   /// array with flags if a column is editable
   CSimpleArray<bool> m_editableColumnList;

   bool m_isReadOnly;
};
