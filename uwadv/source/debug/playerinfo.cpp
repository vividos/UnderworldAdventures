/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Underworld Adventures Team

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


// ua_playerinfo_list misc. stuff

const char* ua_playerinfo_list::frame_name = "Player Info";

// event table
BEGIN_EVENT_TABLE(ua_playerinfo_list, wxListCtrl)
END_EVENT_TABLE()

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
:wxListCtrl(parent, id, pos, size, style | wxLC_REPORT | wxLC_HRULES | wxLC_VRULES)
{
   wxListItem column;
   column.m_format = wxLIST_FORMAT_LEFT;
   column.m_width = 110;
   column.m_text = "Player Attribute";
   column.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_WIDTH | wxLIST_MASK_FORMAT;

   InsertColumn(0,column);

   column.m_width = 200-column.m_width-20;
   column.m_text = "Value";
   InsertColumn(1,column);
}

void ua_playerinfo_list::UpdateData(ua_debug_interface* inter, bool to_underw)
{
   ua_uw_access_api* api = inter->get_access_api();

   DeleteAllItems();

   for(unsigned int i=0; i<SDL_TABLESIZE(ua_playerinfo_captions); i++)
   {
      // set item caption
      wxString caption(ua_playerinfo_captions[i]);
      long nr = InsertItem(i,caption);

      // set value
      wxString value;
      double d;

      if (i<4)
      {
         api->player_value(inter,false,i,d);
         value.Printf("%3.2f",d);
      }
      else
      {
         unsigned int val;
         if (i<4+ua_attr_max)
            api->player_attribute(inter,false,i-4,val);
         else
            api->player_skill(inter,false,i-4-ua_attr_max,val);

         value.Printf("%u",val);
      }

      SetItem(nr,1,value);
   }
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
