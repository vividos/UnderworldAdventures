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
/*! \file objectlist.cpp

   \brief master object list control

*/

// needed includes
#include "dbgcommon.hpp"
#include "objectlist.hpp"
#include "dbgapp.hpp"
#include "mainframe.hpp"
#include <sstream>


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
   "xpos",
   "ypos",
   "zpos",
   "heading",
   "flags",
   "ench",
   "is_quant",
   "hidden"
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
   35,   // xpos
   35,   // ypos
   40,   // zpos
   45,   // heading
   40,   // flags
   40,   // ench
   40,   // is_quant
   40,   // is_hidden
};


// ua_objectlist_ctrl event table

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
      ua_debug_client_interface& client = wxGetApp().get_client_interface();
      client.lock(true);

      level = client.get_player_attr(4);
      client.lock(false);
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
   ua_debug_client_interface& client = wxGetApp().get_client_interface();
   client.lock(true);

/*
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

      case 7: // xpos
      case 8: // ypos
      case 10: // heading
      case 11: // flags
         param2.set_int(col-2);
         cmd(udc_objlist_get,level,&param1,&param2);
         text.Printf("%x",param1.val.i);
         break;

      case 9: // zpos
         param2.set_int(col-2);
         cmd(udc_objlist_get,level,&param1,&param2);
         text.Printf("%02x",param1.val.i);
         break;

      case 12: // enchanted
      case 13: // is_quant
      case 14: // is_hidden
         param2.set_int(col-2);
         cmd(udc_objlist_get,level,&param1,&param2);
         text = param1.val.i == 1 ? "yes" : "no";
         break;

      case 3: // link
      case 4: // quality
      case 5: // owner
      case 6: // quantity
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
*/
   client.lock(false);

   // refresh view
   RefreshItems(0,0x03ff);
}

void ua_objectlist_ctrl::OnBeginLabelEdit(wxListEvent& event)
{
   int column = event.m_item.GetColumn();

   // do not allow editing some columns
   if (column==0 || column==2 || column==3)
   {
      // select new item when clicking on some columns
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
//   text = table[item*SDL_TABLESIZE(ua_objectlist_captions)+column].c_str();
   return text;
}

int ua_objectlist_ctrl::OnGetItemImage(long item) const
{
   return 0;
}


// ua_objectlist_frame event table

BEGIN_EVENT_TABLE(ua_objectlist_frame, wxMDIChildFrame)
   EVT_MENU(MENU_OBJLIST_CHANGELEVEL, ua_objectlist_frame::OnMenuObjlistChangeLevel)
   EVT_MENU(MENU_OBJLIST_SAVE, ua_objectlist_frame::OnMenuObjlistSave)
END_EVENT_TABLE()


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

   // menu bar
   menubar = new wxMenuBar();
   reinterpret_cast<ua_debugger_main_frame*>(parent)->AddFrameMenus(menubar);

   // object list menu
   objlistmenu = new wxMenu;
   objlistmenu->Append(MENU_OBJLIST_CHANGELEVEL, "&Change level ...",
      "changes underworld level to show master object list");
   objlistmenu->Append(MENU_OBJLIST_SAVE, "&Save to text file ...",
      "saves master object list as text file");

   menubar->Append(objlistmenu, "&Master Object List");

   SetMenuBar(menubar);
}

void ua_objectlist_frame::OnMenuObjlistChangeLevel(wxCommandEvent& event)
{
   unsigned int level = listctrl->get_level();

   // choose level to change to
   {
      // TODO use string names of levels from (custom) game strings
      wxString levelnames_uw1[] =
      {
         "Level 0: Goblin Home/Human Enclave",
         "Level 1: Mountain-Folk",
         "Level 2: Lair of the Lizardmen",
         "Level 3: Knights of the Crux",
         "Level 4: Lair of the Lizards",
         "Level 5: The Tombs",
         "Level 6: Seers of the Moonstone",
         "Level 7: Meeting Halls",
         "Level 8: Ethereal Void",
      };

      wxString levelnames_uw2[] =
      {
         "Britannia 1 (castle)",
         "Britannia 2 (cellars)",
         "Britannia 3 (sewers)",
         "Britannia 4",
         "Britannia 5 (gem)",
         "Britannia 5? (draft?)",
         "Britannia 4? (draft?)",
         "(empty)",
         "Prisons 1 (cellar)",
         "Prisons 2",
         "Prisons 3",
         "Prisons 4",
         "Prisons 5",
         "Prisons 6",
         "Prisons 7",
         "Prisons 8",
         "Killorn Keep 1",
         "Killorn Keep 2",
         "(empty)",
         "(empty)",
         "Unknown icy place",
         "(empty)",
         "(empty)",
         "(empty)",
         "Ice Caverns 1",
         "Ice Caverns 2",
         "Ice Caverns?",
         "(empty)",
         "(empty)",
         "(empty)",
         "(empty)",
         "(empty)",
         "Talorus 1",
         "Talorus 2",
         "(empty)",
         "(empty)",
         "(empty)",
         "(empty)",
         "(empty)",
         "(empty)",
         "Academy 1",
         "Academy 2",
         "Academy 3",
         "Academy 4",
         "Academy 5",
         "Academy 6",
         "Academy 7",
         "Academy 8",
         "Loth's Tomb 1",
         "Loth's Tomb 2",
         "Loth's Tomb 3",
         "Loth's Tomb 4",
         "(empty)",
         "(empty)",
         "Unknown (maze)",
         "Loth's Tomb?",
         "Pits of Carnage 1",
         "Pits of Carnage 2",
         "Pits of Carnage 3",
         "(empty)",
         "(empty)",
         "(empty)",
         "(empty)",
         "(empty)",
         "Ethereal Void 1",
         "Ethereal Void 2",
         "Ethereal Void 3",
         "Ethereal Void (yellow)",
         "Ethereal Void (main)",
         "Can't Get Here",
         "Ethereal Void",
         "Ethereal Void?",
         "Loth's Tomb?",
      };

      wxSingleChoiceDialog dlg(this,"select a new underworld level ...",
         "Underworld Adventures Debugger",
         SDL_TABLESIZE(levelnames_uw1),levelnames_uw1,NULL);

      dlg.SetSelection(level);

      if (dlg.ShowModal() != wxID_OK)
         return;

      level = dlg.GetSelection();
   }

   // change level
   listctrl->set_level(level);
   listctrl->UpdateData();

   // set new title
   wxString title;
   title.Printf("Master Object List - Level %u",listctrl->get_level());

   SetTitle(title);
}

void ua_objectlist_frame::OnMenuObjlistSave(wxCommandEvent& event)
{
   // update object list
   UpdateData();

   // construct base filename
   std::string filename;
   {
      std::ostringstream buffer;
      buffer << "objlist-level" << listctrl->get_level() <<
         ".csv" << std::ends;

      filename.assign(buffer.str());
   }

   // ask for file to save to
   {
      wxFileDialog dlg(this,"select an output filename","./",
         filename.c_str(),"*.csv",wxSAVE|wxOVERWRITE_PROMPT);

      if (dlg.ShowModal()==wxID_OK)
         filename.assign(dlg.GetPath());
      else
         return;
   }

   // write comma separated values file
   {
      std::vector<std::string>& table = listctrl->get_table();
      unsigned int ncol = SDL_TABLESIZE(ua_objectlist_captions);

      FILE* fd = fopen(filename.c_str(),"wt");

      // write column names
      for(unsigned int col=0; col<ncol; col++)
         fprintf(fd,"\"%s\";",ua_objectlist_captions[col]);

      fputs("\n",fd);

      // write table
      for(unsigned int objpos=0; objpos<0x400; objpos++)
      {
         for(unsigned int col=0; col<ncol; col++)
            fprintf(fd,"\"%s\";",table[objpos*ncol+col].c_str());

         fputs("\n",fd);
      }

      fclose(fd);
   }
}
