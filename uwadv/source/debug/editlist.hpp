/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

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
/*! \file editlist.hpp

   \brief list control with editable cells

   to use the wxEditListCtrl, you have to do the following steps:

   1. derive your class from wxEditListCtrl, if you want to handle some events
      for yourself
   2. add an event handler for left mouse-down events:
      EVT_LEFT_DOWN(wxEditListCtrl::OnMouseLeftDown)
   3. to check if a cell may be edited or editing may be ended, add event
      handler for these events:
      EVT_LIST_BEGIN_LABEL_EDIT
      EVT_LIST_END_LABEL_EDIT

*/

// include guard
#ifndef uadebug_editlist_hpp_
#define uadebug_editlist_hpp_

// needed includes
#include "wx/listctrl.h"
#include "wx/textctrl.h"


// classes

//! list control in report mode with editable cells
class wxEditListCtrl: public wxListCtrl
{
public:
   //! ctor
   wxEditListCtrl(wxWindow* parent, wxWindowID id,
      const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
      long style = wxLC_REPORT, const wxValidator& validator = wxDefaultValidator,
      const wxString& name = "editListCtrl")
         :wxListCtrl(parent,id,pos,size,style|wxLC_REPORT,validator,name)
   {
   }

protected:
   // event handler

   //! called when beginning item edit
   void OnBeginLabelEdit(wxListEvent& event);

   //! called on left mouse click
   void OnMouseLeftDown(wxMouseEvent& event);

   //! called on ending item edit
   void OnEndLabelEdit(wxListEvent& event);

   // event table
   DECLARE_EVENT_TABLE()
};


//! text control to edit list cell contents
class wxEditListTextCtrl: public wxTextCtrl
{
public:
   //! ctor
   wxEditListTextCtrl(wxListCtrl *owner, size_t item, size_t column);

protected:
   // event handler
   void OnChar(wxKeyEvent& event);
   void OnKeyUp(wxKeyEvent& event);
   void OnKillFocus(wxFocusEvent& event);

   bool AcceptChanges();
   void Finish();

protected:
   //! owner list control
   wxListCtrl* owner;

   //! start value of text control
   wxString startvalue;

   //! item and column of cell to edit
   size_t item;
   size_t column;

   //! indicates if editing is finished
   bool finished;

   // event table
   DECLARE_EVENT_TABLE()
};

#endif
