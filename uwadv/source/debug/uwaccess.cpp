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
/*! \file uwaccess.cpp

   \brief underworld access API implementation

*/

// needed includes
#include "common.hpp"
#include "uwaccess.hpp"
#include "core.hpp"
#include "renderer.hpp"


// static variables

//! current access api object
ua_uw_access_api* ua_uw_access_api::cur_api = NULL;


// ua_uw_access_api methods

ua_uw_access_api::ua_uw_access_api()
:change_level(false)
{
}

void ua_uw_access_api::init(ua_game_core_interface* thecore, ua_debug_interface* dbgint)
{
   core = thecore;
   debug = dbgint;
   cur_api = this;
}

void ua_uw_access_api::tick()
{
   debug->lock();

   if (change_level)
   {
      change_level = false;

      // do scheduled change_level() call
      // this must be done using the thread which created the OpenGL window
      unsigned int level = core->get_underworld().get_player().get_attr(ua_attr_maplevel);
      core->get_underworld().change_level(level);
   }

   debug->unlock();
}

unsigned int ua_uw_access_api::command_func(
   unsigned int cmd, unsigned int type,
   ua_debug_param* param1, ua_debug_param* param2)
{
   unsigned int ret = 0;

   ua_game_core_interface* core = cur_api->core;
   ua_debug_interface* debug = cur_api->debug;
   ua_underworld& underw = core->get_underworld();

   switch(cmd)
   {
   case udc_nop: // do noting
      ret = 42;
      break;

   case udc_lock: // lock underworld
      debug->lock();
      break;

   case udc_unlock: // unlock underworld
      debug->unlock();
      break;

   case udc_game_suspend: // suspend game
      underw.pause_eval(true);
      break;

   case udc_game_resume: // resume game
      underw.pause_eval(false);
      break;

   case udc_player_get: // get player value
      {
         ua_player& pl = underw.get_player();

         unsigned int value = param1->val.i;

         if (value<4)
         {
            switch(value)
            {
            case 0: param1->val.d = pl.get_xpos(); break;
            case 1: param1->val.d = pl.get_ypos(); break;
            case 2: param1->val.d = pl.get_height(); break;
            case 3: param1->val.d = pl.get_angle_rot(); break;
            }

            param1->type = ua_param_double;
         }
         else
         {
            value -= 4;
            param1->type = ua_param_int;

            if (value<ua_attr_max)
               param1->val.i = pl.get_attr((ua_player_attributes)value);
            else
               param1->val.i = pl.get_skill((ua_player_skills)(value-ua_attr_max));
         }
      }
      break;

   case udc_player_set: // set player value
      {
         ua_player& pl = underw.get_player();

         unsigned int value = param1->val.i;

         if (value<4)
         {
            ua_level& level = underw.get_current_level();

            switch(value)
            {
            case 0: // xpos
               pl.set_pos(param2->val.d,pl.get_ypos());
               pl.set_height(level.get_floor_height(param2->val.d,pl.get_ypos()));
               ua_trace("teleporting to x=%f, y=%f\n",pl.get_xpos(),pl.get_ypos());
               break;
            case 1: // ypos
               pl.set_pos(pl.get_xpos(),param2->val.d);
               pl.set_height(level.get_floor_height(param2->val.d,pl.get_ypos()));
               ua_trace("teleporting to x=%f, y=%f\n",pl.get_xpos(),pl.get_ypos());
               break;
            case 2: pl.set_height(param2->val.d); break;
            case 3: pl.set_angle_rot(param2->val.d); break;
            }
         }
         else
         {
            value -= 4;

            if (value<ua_attr_max)
            {
               // setting attribute
               unsigned int prev_val = pl.get_attr((ua_player_attributes)value);

               pl.set_attr((ua_player_attributes)value,param2->val.i);

               if (value==ua_attr_maplevel && prev_val != param2->val.i)
                  cur_api->change_level = true; // schedule change_level() call
            }
            else
            {
               value -= ua_attr_max;
               pl.set_skill((ua_player_skills)value,param2->val.i);
            }
         }
      }
      break;

   case udc_objlist_get: // get master object list value
      {
         // get object info
         ua_level& level = type > underw.get_num_levels() ? underw.get_current_level() :
            underw.get_level(type);

         ua_object& obj = level.get_mapobjects().get_object(static_cast<Uint16>(param1->val.i));
         ua_object_info& objinfo = obj.get_object_info();
         ua_object_info_ext& extobjinfo = obj.get_ext_object_info();

         switch(param2->val.i)
         {
         case 0: // item_id
            param1->set_int(objinfo.item_id);
            break;

         case 1: // link
            param1->set_int(objinfo.link);
            break;

         case 2: // quality
            param1->set_int(objinfo.quality);
            break;

         case 3: // owner
            param1->set_int(objinfo.owner);
            break;

         case 4: // quantity
            param1->set_int(objinfo.quantity);
            break;

         case 5: // xpos
            param1->set_int(extobjinfo.xpos);
            break;

         case 6: // ypos
            param1->set_int(extobjinfo.ypos);
            break;

         case 7: // zpos
            param1->set_int(extobjinfo.zpos);
            break;

         case 8: // heading
            param1->set_int(extobjinfo.heading);
            break;

         case 9: // flags
            param1->set_int(objinfo.flags);
            break;

         case 10: // enchanted
            param1->set_int(objinfo.enchanted ? 1:0);
            break;

         case 11: // is_quantity
            param1->set_int(objinfo.is_quantity);
            break;

         case 12: // is_hidden
            param1->set_int(objinfo.is_hidden ? 1:0);
            break;
         }
      }
      break;

   case udc_objlist_set: // set master object list value
      {
      }
      break;

   case udc_strings_get: // returns string from given string block
      {
         std::string text;
         text = underw.get_strings().get_string(param1->val.i,param2->val.i);
         param1->set(text.c_str());
      }
      break;

   case udc_render: // renders underworld
      cur_api->render(type,param1 != NULL ? param1->val.i : 0);
      break;

   default:
      ret = 1;
   }

   return ret;
}

void ua_uw_access_api::render(unsigned int type, unsigned int level)
{
   switch(type)
   {
   case 0: // init renderer
      renderer = new ua_renderer;
      renderer->init(&core->get_underworld(),
         &core->get_texmgr(),
         &core->get_critter_pool(),
         &core->get_model_manager(),
         ua_vector3d(0.0, 0.0, 0.0) );
      break;

   case 1: // clean up renderer
      delete renderer;
      renderer = NULL;
      break;

   case 2: // render given level
      if (renderer==NULL) break;

      // TODO render any level
      renderer->render();
      break;

   case 3: // prepare level textures
      {
         // TODO prepare any level
         const std::vector<Uint16>& used_textures =
            core->get_underworld().get_current_level().get_used_textures();

         ua_texture_manager& texmgr = core->get_texmgr();

         unsigned int max = used_textures.size();
         for(unsigned int n=0; n<max; n++)
            texmgr.prepare(used_textures[n]);
      }
      break;
   }
}
