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
/*! \file hotspotlist.cpp

   \brief underworld hotspot list

*/

// needed includes
#include "dbgcommon.hpp"
#include "hotspotlist.hpp"
#include "dbgapp.hpp"


// static variables

const char* ua_hotspotlist_frame::frame_name = "hotspot_list_frame";


// tables
struct ua_hotspot_items
{
   const char* name;
   unsigned int level;
   double xpos, ypos;
} hotspot_items[] =
{
   { "Level 1: Entrance of the Abyss", 0, 32.0, 2.0 },
   { "Level 1: Bragit (7)", 0, 17.0, 6.0 },
   { "Level 1: Human Enclave", 0, 22.5, 15.5 },
   { "Level 1: Spider Lair", 0, 26.5, 25.5 },
   { "Level 1: Hostile Green Goblins", 0, 5.5, 38.5 },
   { "Level 1: Green Goblin Lair", 0, 13.5, 53.5 },
   { "Level 1: Gray Goblin Lair", 0, 54.5, 55.5 },
   { "Level 1: Lever Tile Puzzle (22)", 0, 47.5, 46.5 },
   { "Level 1: 's Grave (near 22)", 0, 49.5, 41.5 },
   { "Level 1: Shrine (17)", 0, 46.5, 15.5 },

   { "Level 6: Wine of Compassion (16)", 5, 26.5, 51.5 },

   { "Level 7: Crystal Splinter passage", 6, 20.5, 44.5 },

   { "Ethereal Void: Starting point", 8, 29.5, 24.5 },
   { "Ethereal Void: Green Pathway End", 8, 52.5, 48.5 },
   { "Ethereal Void: Blue Pathway End", 8, 10.5, 16.5 },
   { "Ethereal Void: Red Pathway End", 8, 7.5, 51.5 },

   { NULL, 0, 0.0, 0.0 } // end of list marker
};


// ua_hostspotlist_frame event table

BEGIN_EVENT_TABLE(ua_hotspotlist_frame, wxMDIChildFrame)
   EVT_LIST_ITEM_ACTIVATED(-1, ua_hotspotlist_frame::OnListItemActivated)
END_EVENT_TABLE()


// ua_hostspotlist_frame methods

ua_hotspotlist_frame::ua_hotspotlist_frame(
   /*wxDocManager* doc_manager, */
   wxMDIParentFrame* parent, wxWindowID id,
   const wxPoint& pos, const wxSize& size, long style)
:wxMDIChildFrame(parent, id, "Hotspot List", pos, size, style,frame_name)
{
   hotspotlist = new wxListCtrl(this, -1, wxDefaultPosition, wxDefaultSize,
      wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL,
      wxDefaultValidator, "hotspotList");

   hotspotlist->InsertColumn(0,"Hotspot Name",wxLIST_FORMAT_LEFT,400);
   hotspotlist->InsertColumn(1,"Position",wxLIST_FORMAT_LEFT,200);

   // insert all items
   unsigned int i=0;
   while(hotspot_items[i].name != NULL)
   {
      long pos = hotspotlist->InsertItem(i,hotspot_items[i].name);

      wxString pos_str;
      pos_str.Printf("Level %u, Pos %2.1f/%2.1f",
         hotspot_items[i].level,
         hotspot_items[i].xpos,hotspot_items[i].ypos);

      wxListItem item;

      item.SetMask(wxLIST_MASK_TEXT);
      item.SetId(pos);
      item.SetColumn(1);
      item.SetText(pos_str);

      hotspotlist->SetItem(item);

      i++;
   }
}

void ua_hotspotlist_frame::OnListItemActivated(wxListEvent& event)
{
   unsigned int pos = event.m_itemIndex;

   ua_debug_command_func cmd = wxGetApp().command;

   // lock underworld
   cmd(udc_lock,0,NULL,NULL);

   // set new level
   ua_debug_param param1,param2;

   param1.set_int(8);
   param2.set_int(hotspot_items[pos].level);
   cmd(udc_player_set,0,&param1,&param2);

   // xpos
   param1.set_int(0);
   param2.set(hotspot_items[pos].xpos);
   cmd(udc_player_set,0,&param1,&param2);

   // ypos
   param1.set_int(1);
   param2.set(hotspot_items[pos].ypos);
   cmd(udc_player_set,0,&param1,&param2);

   // set proper height

   // set to 24.0 for now
   param1.set_int(2);
   param2.set(24.0);
   cmd(udc_player_set,0,&param1,&param2);

   // unlock again
   cmd(udc_unlock,0,NULL,NULL);
}
