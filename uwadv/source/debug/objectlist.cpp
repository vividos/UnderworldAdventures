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
   "item_id",  // 0
   "name",
   "link",     // 1
   "quality",  // 2
   "owner",    // 3
   "quantity",
   "x",
   "y",
   "height",
   "angle",
   "enchanted",
   "is_link",
//   "extra"
};

unsigned int ua_objectlist_widths[] =
{
   40,   // pos
   45,   // item_id
   120,  // name
   45,   // link
   45,   // quality
   45,   // owner
   50,   // quantity
   40,
   40,
   50,
   40,
   60,   // ench
   50,   // is_link
   60,
};


// ua_objectlist_frame event table

BEGIN_EVENT_TABLE(ua_objectlist_ctrl, wxListCtrl)
   EVT_LIST_BEGIN_LABEL_EDIT(-1, ua_objectlist_ctrl::OnBeginLabelEdit)
   EVT_LIST_END_LABEL_EDIT(-1, ua_objectlist_ctrl::OnEndLabelEdit)
   EVT_LEFT_DOWN(wxEditListCtrl::OnMouseLeftDown)
END_EVENT_TABLE()


// ua_objectlist_ctrl methods

ua_objectlist_ctrl::ua_objectlist_ctrl(wxWindow* parent, int mylevel)
:wxEditListCtrl(parent, -1, wxDefaultPosition, wxDefaultSize,
   wxLC_VIRTUAL | wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL)
{
   // determine level to show
   if (mylevel<0)
   {
      ua_debug_command_func cmd = wxGetApp().command;

      cmd(udc_lock,0,NULL,NULL);

      ua_debug_param param1;
      param1.set_int(8);
      cmd(udc_player_get,0,&param1,NULL);

      level = param1.val.i;
      cmd(udc_unlock,0,NULL,NULL);
   }
   else
      level = static_cast<unsigned int>(mylevel);

   // insert all columns
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

   UpdateData();
}

void ua_objectlist_ctrl::UpdateData()
{
   ua_debug_command_func cmd = wxGetApp().command;
   unsigned int ncol = SDL_TABLESIZE(ua_objectlist_captions);

   table.clear();
   table.resize(0x0400*ncol);

   wxString text;
   ua_debug_param param1,param2;

   cmd(udc_lock,0,NULL,NULL);

   for(unsigned int objpos=0; objpos<0x0400; objpos++)
   for(unsigned int col=0; col<ncol; col++)
   {
      param1.set_int(objpos);

      // check which column we have
      switch(col)
      {
      case 0: // pos
         text.Printf("%04x",objpos);
         break;

      case 1: // item_id
         param2.set_int(0);
         cmd(udc_objlist_get,level,&param1,&param2);
         text.Printf("%04x",param1.val.i);
         break;

      case 2: // name
         // get item_id
         param2.set_int(0);
         cmd(udc_objlist_get,level,&param1,&param2);

         // get string from block 4
         param2.set_int(param1.val.i);
         param1.set_int(4);
         cmd(udc_strings_get,0,&param1,&param2);
         text = param1.val.str;
         break;

      case 7: // x
      case 8: // y
         param2.set_int(col-2);
         cmd(udc_objlist_get,level,&param1,&param2);
         text.Printf("%.3f",param1.val.d);
         break;

      case 9: // height
         param2.set_int(col-2);
         cmd(udc_objlist_get,level,&param1,&param2);
         text.Printf("%2.1f",param1.val.d);
         break;

      case 10: // angle
         param2.set_int(col-2);
         cmd(udc_objlist_get,level,&param1,&param2);
         text.Printf("%u",param1.val.i);
         break;

      case 11: // enchanted
      case 12: // is_link
         param2.set_int(col-2);
         cmd(udc_objlist_get,level,&param1,&param2);
         text = param1.val.i == 1 ? "yes" : "no";
         break;

      case 3:
      case 4:
      case 5:
      case 6:
         param2.set_int(col-2);
         cmd(udc_objlist_get,level,&param1,&param2);
         text.Printf("%04x",param1.val.i);
         break;

      default: // all other fields
         text.Printf("-no text-");
         break;
      }

      // set string
      table[objpos*ncol+col].assign(text);
   }

   cmd(udc_unlock,0,NULL,NULL);
}

void ua_objectlist_ctrl::OnBeginLabelEdit(wxListEvent& event)
{
   int column = event.m_item.GetColumn();

   // do not allow editing column 0 and 2
   if (column==0 || column==2)
   {
      // select new item when clicking on column 0
      if (column==0 ||column==2)
         SetItemState(event.m_item.GetId(),wxLIST_STATE_SELECTED,wxLIST_STATE_SELECTED);

      event.Veto();
   }
}

void ua_objectlist_ctrl::OnEndLabelEdit(wxListEvent& event)
{
   // TODO: store value
}

wxString ua_objectlist_ctrl::OnGetItemText(long item, long column) const
{
   wxString text;
   text = table[item*SDL_TABLESIZE(ua_objectlist_captions)+column].c_str();
   return text;
}

int ua_objectlist_ctrl::OnGetItemImage(long item) const
{
   return 0;
}


// ua_objectlist_frame methods

ua_objectlist_frame::ua_objectlist_frame(/*wxDocManager* doc_manager, */
   int mylevel,
   wxMDIParentFrame* parent, wxWindowID id,
   const wxPoint& pos, const wxSize& size, long style)
:wxMDIChildFrame(parent, id, "Master Object List", pos, size, style,frame_name)
{
   listctrl = new ua_objectlist_ctrl(this,mylevel);
   listctrl->EnsureVisible(0x3ff);

   // set new title
   wxString title;
   title.Printf("Master Object List - Level %u",listctrl->get_level());

   SetTitle(title);
}
