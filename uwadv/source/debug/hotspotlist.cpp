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
/*! \file hotspotlist.cpp

   \brief underworld hotspot list

*/

// needed includes
#include "dbgcommon.hpp"
#include "hotspotlist.hpp"
#include "dbgapp.hpp"
#include "player.hpp"


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

   { "Level 2: Brawnclan (3)", 1, 13.5, 28.5 },

   { "Level 3: Sseetharee (2)", 2, 24.5, 22.5 },

   { "Level 4: Sethar Strongarm (11)", 3, 50.5, 22.5 },

   { "Level 5: Shanclick and Eyesnack (1)", 4, 52.5, 23.5 },

   { "Level 6: Wine of Compassion (16)", 5, 26.5, 51.5 },

   { "Level 7: Crystal Splinter passage", 6, 20.5, 44.5 },

   { "Level 8: Before the Chamber of Virtue", 7, 32.0, 25.5 },

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

   ua_debug_client_interface& client = wxGetApp().get_client_interface();

   // lock underworld
   client.lock(true);

   // set new level
   client.set_player_attr(ua_attr_maplevel,hotspot_items[pos].level);

   // set position
   client.set_player_pos_info(0,hotspot_items[pos].xpos);
   client.set_player_pos_info(1,hotspot_items[pos].ypos);

   double height = client.get_tile_height(hotspot_items[pos].level,
      hotspot_items[pos].xpos, hotspot_items[pos].ypos);
   client.set_player_pos_info(2,height);

   // unlock again
   client.lock(false);
}
