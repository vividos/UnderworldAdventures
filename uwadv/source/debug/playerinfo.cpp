/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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
/*! \file playerinfo.cpp

   \brief player info list control

*/

// needed includes
#include "dbgcommon.hpp"
#include "playerinfo.hpp"
#include "dbgapp.hpp"


// ua_playerinfo_list event table

BEGIN_EVENT_TABLE(ua_playerinfo_list, wxListCtrl)
   EVT_LIST_BEGIN_LABEL_EDIT(-1, ua_playerinfo_list::OnBeginLabelEdit)
   EVT_LIST_END_LABEL_EDIT(-1, ua_playerinfo_list::OnEndLabelEdit)
   EVT_LEFT_DOWN(wxEditListCtrl::OnMouseLeftDown)
END_EVENT_TABLE()


// static members

const char* ua_playerinfo_list::frame_name = "Player Info";


// global variables

const char* ua_playerinfo_captions[] =
{
   "xpos",
   "ypos",
   "height",
   "angle",

   // attributes
   "gender",
   "handedness",
   "appearance",
   "profession",
   "maplevel",
   "strength",
   "dexterity",
   "intelligence",
   "life",
   "max_life",
   "mana",
   "max_mana",
   "weariness",
   "hungriness",
   "poisoned",
   "mentalstate",
   "nightvision",
   "talks",
   "kills",
   "level",
   "exp_points",
   "difficulty",

   // skills
   "attack",
   "defense",
   "unarmed",
   "sword",
   "axe",
   "mace",
   "missile",
   "mana",
   "lore",
   "casting",
   "traps",
   "search",
   "track",
   "sneak",
   "repair",
   "charm",
   "picklock",
   "acrobat",
   "appraise",
   "swimming",
};


// ua_playerinfo_list methods

ua_playerinfo_list::ua_playerinfo_list(wxWindow *parent,
   const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
:wxEditListCtrl(parent, id, pos, size, style | wxLC_HRULES | wxLC_VRULES)
{
   // add columns
   wxListItem column;
   column.m_format = wxLIST_FORMAT_LEFT;
   column.m_width = 110;
   column.m_text = "Player Attribute";
   column.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_WIDTH | wxLIST_MASK_FORMAT;

   InsertColumn(0,column);

   column.m_width = 200-column.m_width-30;
   column.m_text = "Value";
   InsertColumn(1,column);

   // add all items
   for(unsigned int i=0; i<SDL_TABLESIZE(ua_playerinfo_captions); i++)
   {
      wxString caption(ua_playerinfo_captions[i]);
      InsertItem(i,caption);
   }
}

void ua_playerinfo_list::UpdateData()
{
/*
   ua_debug_command_func cmd = wxGetApp().command;

   // lock underworld
   cmd(udc_lock,0,NULL,NULL);

   // update all items
   for(unsigned int i=0; i<SDL_TABLESIZE(ua_playerinfo_captions); i++)
   {
      // set value
      wxString value;
      ua_debug_param param1;

      param1.set_int(i);
      cmd(udc_player_get,1,&param1,NULL);

      if (param1.type == ua_param_int)
         value.Printf("%u",param1.val.i);
      else
      if (param1.type == ua_param_double)
         value.Printf("%3.2f",param1.val.d);

      SetItem(i,1,value);
   }

   // unlock again
   cmd(udc_unlock,0,NULL,NULL);
*/
}

void ua_playerinfo_list::AddBar(wxFrameLayout* pLayout)
{
   // add bar to frame layout

   cbDimInfo sizes( 600,200, // when docked horizontally
      200,800, // when docked vertically
      170,400, // when floated
      FALSE,   // the bar is not fixed-size
      4,       // vertical gap (bar border)
      4        // horizontal gap (bar border)
   );

   pLayout->AddBar(this,    // bar window
      sizes, FL_ALIGN_RIGHT, // alignment ( 0-top,1-bottom, etc)
      0,                    // insert into 0th row (vert. position)
      0,                    // offset from the start of row (in pixels)
      frame_name,           // name to refere in customization pop-ups
      TRUE
   );
}

void ua_playerinfo_list::OnBeginLabelEdit(wxListEvent& event)
{
   if (event.m_item.GetColumn()==0)
      event.Veto();
}

void ua_playerinfo_list::OnEndLabelEdit(wxListEvent& event)
{
   if (event.m_col != 1)
      return;

   unsigned int item = event.m_itemIndex;;
   wxString text = event.m_item.GetText();
/*
   ua_debug_command_func cmd = wxGetApp().command;

   ua_debug_param param1,param2;
   param1.set_int(item);

   if (item<4)
   {
      double d;
      text.ToDouble(&d);
      param2.set(d);
   }
   else
   {
      unsigned long ul;
      text.ToULong(&ul);
      param2.set_int(ul);
   }

   cmd(udc_lock,0,NULL,NULL);
   cmd(udc_player_set,0,&param1,&param2);
   cmd(udc_unlock,0,NULL,NULL);
*/
}
