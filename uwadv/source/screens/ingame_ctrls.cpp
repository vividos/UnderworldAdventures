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
/*! \file ingame_ctrls.cpp

   \brief ingame screen controls

*/

// needed includes
#include "common.hpp"
#include "ingame_ctrls.hpp"
#include "underworld.hpp"
#include "ingame_orig.hpp"


// ua_ingame_compass methods

void ua_ingame_compass::init(ua_game_interface& game, unsigned int xpos,
   unsigned int ypos)
{
   // init image
   get_image().create(52,26);

   ua_image_quad::init(game,xpos,ypos);

   // load compass images
   {
      std::vector<ua_image> temp_compass;
      game.get_image_manager().load_list(temp_compass,"compass");

      ua_palette256_ptr pal0 = temp_compass[0].get_palette();

      img_compass.resize(16);

      static unsigned int compass_tip_coords[16*2] =
      {
         24, 0, 16, 2,  8, 3,  4, 6,
          0,10,  0,14,  4,16, 12,19,
         24,21, 32,19, 44,16, 48,14,
         48,10, 44, 6, 40, 3, 32, 2,
      };

      // create compass images and add needle tips and right/bottom borders
      for(unsigned int n=0; n<16; n++)
      {
         ua_image& img = img_compass[n];
         img.create(52,26);
         img.get_palette() = pal0;

         img.paste_image(temp_compass[n&3],0,0);

         // compass needle
         img.paste_image(temp_compass[n+4],
            compass_tip_coords[n*2],compass_tip_coords[n*2+1]);
      }
   }

   // remember player object
   player = &game.get_underworld().get_player();

   compass_curimg = 16;
}

void ua_ingame_compass::draw()
{
   // check if we have to reupload the image

   // calculate current angle and images
   double angle = fmod(-player->get_angle_rot()+90.0+360.0,360.0);
   unsigned int compassimg = unsigned((angle+11.25)/22.5)&15;

   // prepare texture
   if (compass_curimg != compassimg)
   {
      // reupload compass texture
      compass_curimg = compassimg;

      unsigned int dest = ua_image_quad::has_border ? 1 : 0;
      get_image().paste_image(img_compass[compassimg],dest,dest);

      update();
   }

   ua_image_quad::draw();
}


// ua_ingame_runeshelf methods

void ua_ingame_runeshelf::init(ua_game_interface& game, unsigned int xpos,
   unsigned int ypos)
{
   get_image().create(46,16);

   // load images 232..255; A-Z without X and Z
   game.get_image_manager().load_list(img_runestones, "objects", 232,256);

   ua_image_quad::init(game,xpos,ypos);
}

/*! Updates the runeshelf image from runeshelf content.
    \todo actually get runes on the self
*/
void ua_ingame_runeshelf::update_runeshelf()
{
   ua_image& img_shelf = get_image();
   img_shelf.clear(0);

   unsigned int rune[3] = { 0, 0, 0 };

   for(unsigned int i=0; i<3; i++)
   {
      if (rune[i] == 0)
         continue;

      // paste appropriate rune image
      ua_image& img_rune = img_runestones[(rune[i]-1)%24];

      unsigned int dest = ua_image_quad::has_border ? 1 : 0;
      img_shelf.paste_rect(img_rune, 0,0, 14,14,
         i*15+dest, dest, true);
   }

   update();
}


// ua_ingame_spell_area methods

void ua_ingame_spell_area::init(ua_game_interface& game, unsigned int xpos,
   unsigned int ypos)
{
   get_image().create(51,18);

   // load images 232..255; A-Z without X and Z
   game.get_image_manager().load_list(img_spells, "spells");

   ua_image_quad::init(game,xpos,ypos);
}

/*! Updates the active spell area image.
    \todo actually get spells from ua_underworld
*/
void ua_ingame_spell_area::update_spell_area()
{
   ua_image& img_area = get_image();
   img_area.clear(0);

   unsigned int spell[3] = { 0, 0, 0 };

   for(unsigned int i=0; i<3; i++)
   {
      if (spell[i] == 0)
         continue;

      // paste appropriate spell image
      ua_image& img_spell = img_spells[(spell[i]-1)%21];

      unsigned int dest = ua_image_quad::has_border ? 1 : 0;
      img_area.paste_rect(img_spell, 0,0, 16,18,
         i*17+dest, dest, true);
   }

   update();
}


// ua_ingame_flask methods

void ua_ingame_flask::init(ua_game_interface& game, unsigned int xpos,
   unsigned int ypos)
{
   get_image().create(24,33);

   // load flask images
   {
      std::vector<ua_image> temp_flasks;
      game.get_image_manager().load_list(temp_flasks, "flasks");

      unsigned int maximg = vitality_flask ? 28 : 14;
      img_flask.resize(maximg);

      // paste together all flask fill heights
      for(unsigned int i=0; i<maximg; i+=14)
      {
         ua_image base_img = temp_flasks[75];

         static unsigned int flask_pos[13] =
         { 26, 24, 22, 20, 18, 16, 15, 14, 13, 11, 9, 7, 5 };

         unsigned int offset = vitality_flask ? (i==0 ? 0 : 50) : 25;

         // image 0 is the empty flask
         img_flask[i] = base_img;

         // generate all images
         for(unsigned int j=0; j<13; j++)
         {
            base_img.paste_image(temp_flasks[offset+j], 0, flask_pos[j]);
            img_flask[i+j+1] = base_img;
         }
      }
   }

   ua_image_quad::init(game,xpos,ypos);

   // remember player object
   player = &game.get_underworld().get_player();

   last_image = 14*2;
   is_poisoned = false;
}

void ua_ingame_flask::draw()
{
   is_poisoned = player->get_attr(ua_attr_poisoned) != 0;

   unsigned int curval = player->get_attr(
      vitality_flask ? ua_attr_life : ua_attr_mana);
   unsigned int maxval = player->get_attr(
      vitality_flask ? ua_attr_max_life : ua_attr_max_mana);
   unsigned int curimg = unsigned((curval*13.0)/maxval);

   // check if flask image has to be update
   unsigned int new_image = vitality_flask && is_poisoned ? curimg+14 : curimg;

   if (last_image != new_image)
   {
      last_image = new_image;
      update_flask();
   }

   ua_image_quad::draw();
}

void ua_ingame_flask::update_flask()
{
   unsigned int dest = ua_image_quad::has_border ? 1 : 0;
   get_image().paste_image(img_flask[last_image], dest,dest);
   update();
}


// ua_ingame_gargoyle_eyes methods

void ua_ingame_gargoyle_eyes::init(ua_game_interface& game, unsigned int xpos,
   unsigned int ypos)
{
   get_image().create(20,3);

   game.get_image_manager().load_list(img_eyes, "eyes");

   ua_image_quad::init(game,xpos,ypos);
}

/*! \todo update eyes from data in ua_underworld */
void ua_ingame_gargoyle_eyes::update_eyes()
{
   unsigned int new_image = 0;

   unsigned int dest = ua_image_quad::has_border ? 1 : 0;
   get_image().paste_image(img_eyes[new_image], dest,dest);
   update();
}


// ua_ingame_dragon methods

ua_ingame_dragon::ua_ingame_dragon(bool drg_left)
:dragon_left(drg_left)
{
}

void ua_ingame_dragon::init(ua_game_interface& game, unsigned int xpos,
   unsigned int ypos)
{
   // load images
   game.get_image_manager().load_list(img_dragon,"dragons",
      dragon_left? 0 : 18, dragon_left ? 18 : 36);

   // prepare image
   ua_image& img = get_image();

   img.create(37,104);
   img.get_palette() = img_dragon[0].get_palette();
   img.clear(0);


   ua_image_quad::init(game,xpos,ypos);


   // dragon part that's never moving
   img.paste_image(img_dragon[0], dragon_left ? 0 : 24, 69);

   update_dragon();
}

void ua_ingame_dragon::update_dragon()
{
   ua_image& img = get_image();

   unsigned int head_frame = 1;
   img.paste_image(img_dragon[head_frame], 0, 80);

   unsigned int tail_frame = 14;
   img.paste_image(img_dragon[tail_frame], dragon_left ? 4 : 20, 0);

   update();
}



// tables

struct ua_ingame_command_menu_info
{
   int base_image;
   unsigned int images[7];
   bool selectable[7];
   int opt_index[7];

} ua_ingame_command_menu_table[8] =
{
   { // ua_cmd_menu_actions; special case: images taken from lfti.gr
      0,
      { 0,2,4,6,8,10,0 },
      { true,true,true,true,true,true,true },
      { -1, -1, -1, -1, -1, -1, -1 }
   },
   { // ua_cmd_menu_options
      1,
      { 6,8,10,12,14,16,18 },
      { true,true,true,true,true,true,true },
      // this line is used for showing music, sound, detail status directly in
      // the options menu, but unfortunately there are no images (see optb.gr)
      //{ -1, -1, 0, 1, 2, -1, -1 } 
      { -1, -1, -1, -1, -1, -1, -1 }
   },
   { // ua_cmd_menu_quit
      3,
      { 0,0, 57,59, 0,0,0 },
      { false,false,true,true,false,false,false },
      { -1, -1, -1, -1, -1, -1, -1 }
   },
   { // ua_cmd_menu_music
      4,
      { 47,51,20,22,26, 0,0 },
      { false,false,true,true,true,false,false },
      { 0, -1, -1, -1, -1, -1, -1 }
   },
   { // ua_cmd_menu_sound
      4,
      { 49,52,20,22,26, 0,0 },
      { false,false,true,true,true,false,false },
      { 1, -1, -1, -1, -1, -1, -1 }
   },
   { // ua_cmd_menu_details
      5,
      { 53, 0, 38,40,42,44, 28 },
      { false,false,true,true,true,true,true },
      { 2, -1, -1, -1, -1, -1, -1 }
   },
   { // ua_cmd_menu_save
      2,
      { 0, 30,32,34,36, 24, 0 },
      { false,true,true,true,true,true,false },
      { -1, -1, -1, -1, -1, -1, -1 }
   },
   { // ua_cmd_menu_restore
      2,
      { 46,30,32,34,36, 24, 0 },
      { false,true,true,true,true,true,false },
      { -1, -1, -1, -1, -1, -1, -1 }
   },
};


// ua_ingame_command_buttons methods

void ua_ingame_command_buttons::init(ua_game_interface& game, unsigned int xpos,
   unsigned int ypos)
{
   // load images
   game.get_image_manager().load_list(img_buttons,"optbtns");

   game.get_image_manager().load_list(img_actions,"lfti");

   // create image
   ua_image& img = get_image();
   img.get_palette() = img_buttons[0].get_palette();

   ua_image_quad::init(game,xpos,ypos);

   options[0] = options[1] = options[2] = 0;
   button_selected = -1;
   menu = ua_cmd_menu_actions;

   update_menu();
}

void ua_ingame_command_buttons::select_button(int button)
{
   button_selected = button;

   update_menu();
}

void ua_ingame_command_buttons::select_previous_button(bool jump_to_start)
{
   if (button_selected == -1 || menu == ua_cmd_menu_actions)
      return;

   ua_ingame_command_menu_info& menuinfo = ua_ingame_command_menu_table[menu];

   if (jump_to_start)
   {
      while (button_selected>0 && menuinfo.selectable[button_selected-1])
         button_selected--;
   }
   else
   {
      if (button_selected>0 && menuinfo.selectable[button_selected-1])
         button_selected--;
   }

   update_menu();
}

void ua_ingame_command_buttons::select_next_button(bool jump_to_end)
{
   if (button_selected == -1 || menu == ua_cmd_menu_actions)
      return;

   ua_ingame_command_menu_info& menuinfo = ua_ingame_command_menu_table[menu];

   if (jump_to_end)
   {
      while (button_selected<7 && menuinfo.selectable[button_selected+1])
         button_selected++;
   }
   else
   {
      if (button_selected<7 && menuinfo.selectable[button_selected+1])
         button_selected++;
   }

   update_menu();
}


void ua_ingame_command_buttons::update_menu()
{
   // check if in range
   if (menu >= ua_cmd_menu_max ||
       menu > SDL_TABLESIZE(ua_ingame_command_menu_table))
      return;

   ua_image& img = get_image();
   ua_ingame_command_menu_info& menuinfo = ua_ingame_command_menu_table[menu];

   // re-set window position
   static unsigned int window_pos[6*2] = { 4,10, 4,10, 4,10, 4,10, 3,10, 3,9 };

   wnd_xpos = window_pos[menuinfo.base_image*2];
   wnd_ypos = window_pos[menuinfo.base_image*2+1];

   wnd_width = 35;
   wnd_height = 108;
   img.create(wnd_width,wnd_height);

   // base image
   img.paste_image(img_buttons[menuinfo.base_image], 0,0);

   static unsigned int button_pos[7] = { 2, 17, 32, 47, 62, 77, 92 };
   static unsigned int action_pos[6*2] = { 4,0, 3,17, 2,37, 2,55, 4,70, 4,89 };

   // paste button images
   if (menu == ua_cmd_menu_actions)
   {
      // special: actions menu
      for(unsigned int i=0; i<6; i++)
      {
         unsigned int btn = menuinfo.images[i];

         // selected? draw selected image then
         if (button_selected >= 0 && i == unsigned(button_selected))
            btn++;

         img.paste_image(img_actions[btn], action_pos[i*2], action_pos[i*2+1]);
      }
   }
   else
   {
      // normal menu images
      for(unsigned int i=0; i<7; i++)
      {
         unsigned int btn = menuinfo.images[i];
         if (btn==0)
            continue;

         if (menuinfo.opt_index[i] >= 0)
            btn += options[menuinfo.opt_index[i]];

         // selected? draw selected image then
         if (button_selected > 0 && i == unsigned(button_selected))
            btn++;

         unsigned int xpos = menuinfo.selectable[i] ? 1 : 2;

         // special case: "details" menu, first image
         if (menuinfo.base_image == 5 && i == 0)
            xpos = 0;

         img.paste_image(img_buttons[btn], xpos, button_pos[i]);
      }
   }

   // add border
   add_border(parent->img_background.get_image());

   update();
}

void ua_ingame_command_buttons::mouse_event(bool button_clicked,
   bool left_button, bool button_down, unsigned int mousex,
   unsigned int mousey)
{
   // check if a mouse button is down
   if (!button_clicked && (SDL_GetMouseState(NULL, NULL) & (SDL_BUTTON_LMASK|SDL_BUTTON_RMASK)) == 0)
      return; // no button was pressed

   unsigned int ypos = mousey-wnd_ypos;

   static unsigned int button_pos[8] = { 2, 17, 32, 47, 62, 77, 92, 107 };
   static unsigned int action_pos[8] = { 0, 17, 35, 54, 70, 90, 107, 107 };

   int last_button_selected = button_selected;

   // find selected button
   button_selected = -1;
   for(unsigned int i=0; i<7; i++)
   {
      if ((menu == ua_cmd_menu_actions && ypos >= (action_pos[i]) && ypos < (action_pos[i+1])) ||
          (menu != ua_cmd_menu_actions && ypos >= (button_pos[i]) && ypos < (button_pos[i+1])))
      {
         button_selected = i;
         break;
      }
   }

   if (last_button_selected != button_selected)
      update_menu();

   // do button press when user clicked button
   if (button_clicked && !button_down && button_selected != -1)
   {
      // press key
      do_button_action();
   }
}

void ua_ingame_command_buttons::do_button_action()
{
   ua_ingame_command_menu old_menu = menu;
   bool option_changed = false;

   switch(menu)
   {
      // action menu
   case ua_cmd_menu_actions:
      switch(button_selected)
      {
         // button "options"
      case 0:
         menu = ua_cmd_menu_options;
         parent->set_gamemode(ua_mode_options);
         break;

         // all other buttons just toggle the game mode
      default:
         // update game mode; note: the code assumes that ua_ingame_game_mode
         // values are contiguous
         parent->set_gamemode(ua_ingame_game_mode(button_selected-1+ua_mode_talk));
         break;
      }
      break;

      // options menu
   case ua_cmd_menu_options:
      switch(button_selected)
      {
      case 0: // save game
         parent->schedule_action(ua_action_save_game,true);
         menu = ua_cmd_menu_actions;
         //menu = ua_cmd_menu_save; // old-style save menu
         break;

      case 1: // restore game
         parent->schedule_action(ua_action_load_game,true);
         menu = ua_cmd_menu_actions;
         //menu = ua_cmd_menu_restore; // old-style restore menu
         break;

      case 2: // music
         menu = ua_cmd_menu_music;
         button_selected = options[0]+2;
         option_changed = true;
         break;

      case 3: // sound
         menu = ua_cmd_menu_sound;
         button_selected = options[1]+2;
         option_changed = true;
         break;

      case 4: // detail
         menu = ua_cmd_menu_details;
         button_selected = options[2]+2;
         option_changed = true;
         break;

      case 5: // return to game
         menu = ua_cmd_menu_actions;
         break;

      case 6: // quit game
         menu = ua_cmd_menu_quit;
         break;

      default:
         break;
      }
      break;

      // "really quit?" question
   case ua_cmd_menu_quit:
      switch(button_selected)
      {
      case 2: // yes
         parent->schedule_action(ua_action_exit,true);
         break;

      case 3: // no
         menu = ua_cmd_menu_actions; // better to options menu?
         break;

      default:
         break;
      }
      break;

      // music / sound
   case ua_cmd_menu_music:
   case ua_cmd_menu_sound:
      switch(button_selected)
      {
      case 2:
      case 3:
         options[menu == ua_cmd_menu_music ? 0 : 1] = (button_selected-2)&1;
         option_changed = true;
         break;

      case 4: // done
         menu = ua_cmd_menu_options;
         break;

      default:
         break;
      }
      break;

   case ua_cmd_menu_details:
      switch(button_selected)
      {
      case 2:// low ... very high
      case 3:
      case 4:
      case 5:
         options[2] = (button_selected-2)&3;
         option_changed = true;
         break;

      case 6: // done
         menu = ua_cmd_menu_options;
         break;

      default:
         break;
      }
      break;

   case ua_cmd_menu_save:
   case ua_cmd_menu_restore:
      switch(button_selected)
      {
      case 5: // cancel
         menu = ua_cmd_menu_options;
         break;

      default:
         break;
      }
      break;

   default:
      break;
   }

   // invalidate selection when changing menu
   if (old_menu != menu && !option_changed)
      button_selected = -1;

   // update menu when necessary
   if (old_menu != menu || option_changed)
      update_menu();
}
