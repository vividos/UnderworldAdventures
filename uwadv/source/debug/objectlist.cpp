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
/*! \file objectlist.cpp

   \brief master object list control

*/

// needed includes
#include "dbgcommon.hpp"
#include "objectlist.hpp"
#include "dbgapp.hpp"


// static variables

const char* ua_objectlist_frame::frame_name = "master_object_list";


// global variables

const char* ua_objectlist_captions[] =
{
   "pos",
   "item_id",
   "name",
   "x",
   "y",
   "height",
   "angle",
   "link",
};

unsigned int ua_objectlist_widths[] =
{
   40,
   50,
   120,
   50,
   50,
   50,
   60,
   60,
   60,
   60,
};


// ua_objectlist_frame event table

BEGIN_EVENT_TABLE(ua_objectlist_ctrl, wxListCtrl)
   EVT_LIST_BEGIN_LABEL_EDIT(-1, ua_objectlist_ctrl::OnBeginLabelEdit)
   EVT_LIST_END_LABEL_EDIT(-1, ua_objectlist_ctrl::OnEndLabelEdit)
   EVT_LEFT_DOWN(wxEditListCtrl::OnMouseLeftDown)
END_EVENT_TABLE()


// ua_objectlist_ctrl methods

ua_objectlist_ctrl::ua_objectlist_ctrl(wxWindow* parent)
:wxEditListCtrl(parent, -1, wxDefaultPosition, wxDefaultSize,
   wxLC_VIRTUAL | wxLC_HRULES | wxLC_VRULES)
{
   for(unsigned int i=0; i<SDL_TABLESIZE(ua_objectlist_captions); i++)
   {
      wxListItem column;
      column.m_format = wxLIST_FORMAT_LEFT;
      column.m_width = ua_objectlist_widths[i];
      column.m_text = ua_objectlist_captions[i];
      column.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_WIDTH | wxLIST_MASK_FORMAT;

      InsertColumn(i,column);
   }

   SetItemCount(0x400);
}

void ua_objectlist_ctrl::OnBeginLabelEdit(wxListEvent& event)
{
   // do not allow editing column 0 and 2
   if (event.m_item.GetColumn()==0 || event.m_item.GetColumn()==2)
      event.Veto();
}

void ua_objectlist_ctrl::OnEndLabelEdit(wxListEvent& event)
{
   // TODO: store value
}

wxString ua_objectlist_ctrl::OnGetItemText(long item, long column) const
{
   ua_debug_command_func cmd = wxGetApp().command;

   if (column == 0)
   {
      // position column
      wxString text;
      text.Printf("%04x",item);
      return text;
   }

   cmd(udc_lock,0,NULL,NULL);

   // prepare parameters
   unsigned int level = 0;
   ua_debug_param param1,param2;
   wxString text;

   param1.set(static_cast<unsigned int>(item));

   // get string
   switch(column)
   {
   case 1:
      param2.set(static_cast<unsigned int>(0));
      cmd(udc_objlist_get,level,&param1,&param2);
      text.Printf("%04x",param1.val.i);
      break;

   case 2:
      // get item_id
      param2.set(static_cast<unsigned int>(0));
      cmd(udc_objlist_get,level,&param1,&param2);

      // get string from block 4
      param2.set(param1.val.i);
      param1.set(static_cast<unsigned int>(4));
      cmd(udc_strings_get,0,&param1,&param2);
      text = param1.val.str;
      break;

   default:
      param2.set(static_cast<unsigned int>(column-2));
      cmd(udc_objlist_get,level,&param1,&param2);
      text.Printf("%04x",param1.val.i);
      break;
   }

   param2.set(static_cast<unsigned int>(column-1));

   cmd(udc_unlock,0,NULL,NULL);

   return text;
}

int ua_objectlist_ctrl::OnGetItemImage(long item) const
{
   return 0;
}


// ua_objectlist_frame methods

ua_objectlist_frame::ua_objectlist_frame(/*wxDocManager* doc_manager, */
   wxMDIParentFrame* parent, wxWindowID id,
   const wxPoint& pos, const wxSize& size, long style)
:wxMDIChildFrame(parent, id, "Master Object List", pos, size, style,frame_name)
{
   listctrl = new ua_objectlist_ctrl(this);
   listctrl->EnsureVisible(0x3ff);
}

void ua_objectlist_frame::UpdateData()
{
}
