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
/*! \file editlist.cpp

   \brief list control with editable cells

*/

// needed includes
#include "dbgcommon.hpp"
#include "editlist.hpp"


// wxEditListCtrl event table

BEGIN_EVENT_TABLE(wxEditListCtrl, wxListCtrl)
   EVT_LEFT_DOWN(wxEditListCtrl::OnMouseLeftDown)
END_EVENT_TABLE()


// wxEditListCtrl methods

void wxEditListCtrl::OnMouseLeftDown(wxMouseEvent& event)
{
   // set focus on us to cancel previous edits
   SetFocus();

   // check which cell we have
   int item=0, column=0;
   {
      // check for item
      int flags = 0;
      item = HitTest(wxPoint(event.m_x,event.m_y),flags);

      // no hit?
      if ((flags&wxLIST_HITTEST_ONITEM) == 0)
         return;

      // find out column
      unsigned int xpos = 0;

      while(xpos < (unsigned)event.m_x)
         xpos += GetColumnWidth(column++);
      column--;
   }

   // check if we can start editing the label
   {
      // do list event
      wxListEvent le(wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT, GetId());
      le.SetEventObject(this);
      le.m_itemIndex = item;
      le.m_col = column;

      // fill out item struct
      le.m_item.SetId(item);
      le.m_item.SetColumn(column);

      // handle event
      if (!GetEventHandler()->ProcessEvent(le) || !le.IsAllowed())
      {
         // vetoed by user
         return;
      }
   }

   // create edit control
   wxEditListTextCtrl* text = new wxEditListTextCtrl(this,item,column);
   text->SetFocus();
}


// wxEditListTextCtrl event table

BEGIN_EVENT_TABLE(wxEditListTextCtrl,wxTextCtrl)
   EVT_CHAR      (wxEditListTextCtrl::OnChar)
   EVT_KEY_UP    (wxEditListTextCtrl::OnKeyUp)
   EVT_KILL_FOCUS(wxEditListTextCtrl::OnKillFocus)
END_EVENT_TABLE()


// wxEditListTextCtrl methods

wxEditListTextCtrl::wxEditListTextCtrl(wxListCtrl* theowner,
   size_t theitem, size_t thecolumn)
:owner(theowner), item(theitem), column(thecolumn), finished(false)
{
   // calculate cell rect
   wxRect rect;
   owner->GetItemRect(item, rect, wxLIST_RECT_BOUNDS);

   unsigned int startx = 0;
   for(unsigned int n=0; n<column; n++)
      startx += owner->GetColumnWidth(n);

   rect.SetX(startx);
   rect.SetWidth(owner->GetColumnWidth(column));

   // set start value
   wxListItem info;
   info.SetId(item);
   info.SetColumn(column);
   info.SetMask(wxLIST_MASK_TEXT);
   owner->GetItem(info);
   startvalue = info.GetText();

   // create control
   (void)Create(owner, wxID_ANY, startvalue,
      wxPoint(rect.x+2,rect.y-3),
      wxSize(rect.width-4,rect.height+4),
      wxSIMPLE_BORDER);

   // select all
   SetSelection(-1,-1);
}

void wxEditListTextCtrl::Finish()
{
   if (!finished)
   {
      wxPendingDelete.Append(this);

      finished = true;

      owner->SetFocus();
   }
}

bool wxEditListTextCtrl::AcceptChanges()
{
   const wxString value = GetValue();

   if (value == startvalue)
   {
      // nothing changed, always accept
      return true;
   }

   // send "end label edit" event
   {
      // do list event
      wxListEvent le(wxEVT_COMMAND_LIST_END_LABEL_EDIT, owner->GetId());
      le.SetEventObject(owner);
      le.m_itemIndex = item;
      le.m_col = column;

      // fill out item struct
      le.m_item.SetId(item);
      le.m_item.SetColumn(column);
      le.m_item.SetText(value);

      // handle event
      if (!GetParent()->GetEventHandler()->ProcessEvent(le) || !le.IsAllowed())
      {
         // vetoed by user
         return FALSE;
      }
   }

   // accepted, rename the item
   wxListItem info;
   info.SetId(item);
   info.SetColumn(column);
   info.SetText(value);
   info.SetMask(wxLIST_MASK_TEXT);
   owner->SetItem(info);

   return TRUE;
}

void wxEditListTextCtrl::OnChar(wxKeyEvent& event)
{
   switch ( event.m_keyCode )
   {
   case WXK_RETURN:
      if (!AcceptChanges())
      {
         // vetoed by user
         break;
      }
      // else: fall through

   case WXK_ESCAPE:
      Finish();
      break;

   default:
      event.Skip();
   }
}

void wxEditListTextCtrl::OnKeyUp(wxKeyEvent& event)
{
   if (finished)
   {
      event.Skip();
      return;
   }

   // auto-grow the textctrl:
   wxSize parentSize = owner->GetSize();
   wxPoint myPos = GetPosition();
   wxSize mySize = GetSize();

   int sx, sy;
   GetTextExtent(GetValue() + _T("MM"), &sx, &sy);
   if (myPos.x + sx > parentSize.x)
      sx = parentSize.x - myPos.x;
   if (mySize.x > sx)
      sx = mySize.x;
   SetSize(sx, -1);

   event.Skip();
}

void wxEditListTextCtrl::OnKillFocus( wxFocusEvent &event )
{
   if (!finished)
   {
      (void)AcceptChanges();
      Finish();
   }

   event.Skip();
}
