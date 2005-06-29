/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004,2005 Underworld Adventures Team

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
/*! \file debug.cpp

   \brief debug server implementation

*/

// needed includes
#include "common.hpp"
#include "debug.hpp"
#include "dbgserver.hpp"
#include "game_interface.hpp"
#include "underworld.hpp"
#include "gamestrings.hpp"
#include "image.hpp"


// impl. classes

#ifdef WIN32
//! win32 debug lib context
class ua_debug_lib_context_win32: public ua_debug_lib_context
{
public:
   ua_debug_lib_context_win32():dll(NULL){}

   virtual void init()
   {
      dll = ::LoadLibrary("uadebug.dll");
   }

   virtual void done()
   {
      ::FreeLibrary(dll);
   }

   virtual bool is_avail()
   {
      return dll != NULL && ::GetProcAddress(dll,"uadebug_start") != NULL;
   }

   virtual void debug_start(ua_debug_server_interface* server)
   {
      typedef void (*uadebug_start_func)(void*);

      // get function pointer
      uadebug_start_func uadebug_start =
         (uadebug_start_func)::GetProcAddress(dll, "uadebug_start");

      // start debugger
      uadebug_start(server);
   }

protected:
   //! DLL module handle
   HMODULE dll;
};
#endif

#ifdef LINUX
//! linux debug lib context
/*! Uses dlopen() and dlsym() to load the shared object libuadebug.so and
    start the debugger
*/
class ua_debug_lib_context_linux: public ua_debug_lib_context
{
public:
   //! ctor
   ua_debug_lib_context_linux(){}
   //! inits debug server
   virtual void init(){}
   //! cleans up debug server
   virtual void done(){}
   //! returns if debugger is available
   virtual bool is_avail(){ return false; }
   //! starts debugger
   virtual void debug_start(ua_debug_server_interface* server){}
};
#endif


// ua_debug_server methods

ua_debug_server::ua_debug_server()
:debug_lib(NULL), thread_debugger(NULL), sem_debugger(NULL), mutex_lock(NULL),
 game(NULL), schedule_prepare(false), last_code_debugger_id(0)
{
   // init mutex, semaphore and thread handle
   mutex_lock = SDL_CreateMutex();
   sem_debugger = SDL_CreateSemaphore(0);

#ifdef WIN32
   debug_lib = new ua_debug_lib_context_win32;
#else
   debug_lib = new ua_debug_lib_context;
#endif

   ua_assert(debug_lib != NULL);
}

ua_debug_server::~ua_debug_server()
{
   SDL_DestroyMutex(mutex_lock);

   SDL_DestroySemaphore(sem_debugger);

   if (thread_debugger != NULL)
      SDL_KillThread(thread_debugger);

   debug_lib->done();
   delete debug_lib;
   debug_lib = NULL;
}

void ua_debug_server::init()
{
   debug_lib->init();

   ua_trace("debug server inited; debugger is %savailable\n",
      debug_lib->is_avail() ? "" : "not ");
}

bool ua_debug_server::start_debugger(ua_basic_game_interface* the_game)
{
   game = the_game;

   // check if debugger already runs
   if (debug_lib->is_avail() && SDL_SemValue(sem_debugger)==0)
   {
      ua_trace("starting uadebug thread\n");

      // start new thread
      thread_debugger = SDL_CreateThread(thread_proc,this);
   }
   else
   {
      ua_trace(!debug_lib->is_avail() ?
         "debugger not available\n" : "debugger already running\n");
   }

   return false;
}

bool ua_debug_server::is_debugger_running()
{
   lock(true);
   bool ret = SDL_SemValue(sem_debugger) != 0;
   lock(false);

   return ret;
}

void ua_debug_server::tick()
{
   if (is_debugger_running())
   {
      lock(true);

      if (schedule_prepare)
      {
         // change level per underworld object
         schedule_prepare = false;
         ua_underworld& underw = game->get_underworld();
         underw.change_level(underw.get_player().get_attr(ua_attr_maplevel));
      }

      lock(false);
   }
}

void ua_debug_server::shutdown()
{
   if (debug_lib->is_avail() && thread_debugger != NULL)
   {
      ua_trace("shutting down debugger...\n");

      // send quit message
      ua_debug_server_message msg;
      msg.msg_type = ua_msg_shutdown;

      lock(true);
      add_message(msg);
      lock(false);

      // wait for thread
      SDL_WaitThread(thread_debugger,NULL);
      thread_debugger = NULL;
   }
}

void ua_debug_server::lock(bool set_lock)
{
   if (set_lock)
      SDL_LockMutex(mutex_lock);
   else
      SDL_UnlockMutex(mutex_lock);
}

int ua_debug_server::thread_proc(void* ptr)
{
   ua_debug_server* This = reinterpret_cast<ua_debug_server*>(ptr);

   // increase semaphore count
   SDL_SemPost(This->sem_debugger);

   if (This->debug_lib->is_avail())
      This->debug_lib->debug_start(This);

   ua_trace("uadebug thread ended\n");

   // decrease semaphore count
   SDL_SemWait(This->sem_debugger);

   return 0;
}


void ua_debug_server::start_code_debugger(ua_debug_code_interface* code_debugger)
{
   unsigned int debugger_id = ++last_code_debugger_id;

   code_debugger->set_debugger_id(debugger_id);

   // send "start" message
   ua_debug_server_message msg;
   msg.msg_type = ua_msg_start_code_debugger;
   msg.msg_arg1 = debugger_id;

   lock(true);
   add_message(msg);

   all_code_debugger.insert(std::make_pair<unsigned int, ua_debug_code_interface*>(debugger_id, code_debugger));

   lock(false);
}

void ua_debug_server::end_code_debugger(ua_debug_code_interface* code_debugger)
{
   lock(true);

   // search code debugger in map
   std::map<unsigned int, ua_debug_code_interface*>::iterator iter, stop;
   iter = all_code_debugger.begin();
   stop = all_code_debugger.end();

   unsigned int debugger_id = 0;
   for(; iter != stop; iter++)
   {
      if (iter->second == code_debugger)
      {
         debugger_id = iter->first;

         all_code_debugger.erase(iter);
         break;
      }
   }

   ua_assert(iter != stop); // assert when pointer was not found

   // send "end" message
   ua_debug_server_message msg;
   msg.msg_type = ua_msg_end_code_debugger;
   msg.msg_arg1 = debugger_id;

   add_message(msg);
   lock(false);
}

void ua_debug_server::send_code_debugger_status_update(unsigned int debugger_id)
{
   // send notification about changed state
   ua_debug_server_message msg;
   msg.msg_type = ua_msg_code_debugger_state_update;
   msg.msg_arg1 = debugger_id;

   lock(true);
   add_message(msg);
   lock(false);
}

bool ua_debug_server::check_interface_version(unsigned int interface_ver)
{
   return interface_ver == ua_debug_server_interface_version;
}

unsigned int ua_debug_server::get_flag(unsigned int flag_id)
{
   unsigned int flag = 0;
   switch(flag_id)
   {
   case ua_flag_is_studio_mode:
#ifdef COMPILE_UASTUDIO
      flag = 1; // in uastudio mode
#else
      flag = 0;
#endif
      break;

   default:
      ua_assert(false);
      break;
   }
   return flag;
}

unsigned int ua_debug_server::get_game_path(char* buffer, unsigned int bufsize)
{
   ua_settings& settings = game->get_settings();

   std::string prefix = settings.get_string(ua_setting_game_prefix);
   if (prefix.size()==0)
   {
      if (buffer != NULL)
         buffer[0] = 0;
      return 0;
   }

   std::string game_path = settings.get_string(ua_setting_uadata_path);
   game_path += prefix;

   std::string::size_type strsize = game_path.size();

   if (bufsize < strsize + 1)
      return strsize + 1;

   strncpy(buffer, game_path.c_str(), strsize);
   buffer[strsize] = 0;

   return ua_min(game_path.size(), bufsize);
}

void ua_debug_server::load_game(const char* path)
{
   game->done_game();

   std::string strpath(path);
   game->get_settings().set_value(ua_setting_game_prefix, strpath);

   game->init_game();
}

bool ua_debug_server::pause_game(bool pause)
{
   return game->pause_game(pause);
}

unsigned int ua_debug_server::get_message_num()
{
   return message_queue.size();
}

bool ua_debug_server::get_message(unsigned int& msg_type,
   unsigned int& msg_arg1, unsigned int& msg_arg2, double& msg_arg3,
   unsigned int& msg_text_size)
{
   if (message_queue.size()>0)
   {
      ua_debug_server_message& msg = message_queue.front();
      msg_type = msg.msg_type;
      msg_arg1 = msg.msg_arg1;
      msg_arg2 = msg.msg_arg2;
      msg_arg3 = msg.msg_arg3;
      msg_text_size = msg.msg_text.size()+1;
   }
   return message_queue.size()>0;
}

bool ua_debug_server::get_message_text(char* buffer, unsigned int bufsize)
{
   if (message_queue.size()>0)
   {
      ua_debug_server_message& msg = message_queue.front();
      strncpy(buffer,msg.msg_text.c_str(), bufsize);
   }
   return message_queue.size()>0;
}

bool ua_debug_server::pop_message()
{
   if (message_queue.size()>0)
   {
      message_queue.pop_front();
   }
   return message_queue.size()>0;
}

double ua_debug_server::get_player_pos_info(unsigned int idx)
{
   double val = 0.0;
   if (game == NULL) return val;

   ua_player& pl = game->get_underworld().get_player();
   switch(idx)
   {
   case 0: val = pl.get_xpos(); break;
   case 1: val = pl.get_ypos(); break;
   case 2: val = pl.get_height(); break;
   case 3: val = pl.get_angle_rot(); break;
   default:
      break;
   }
   return val;
}

void ua_debug_server::set_player_pos_info(unsigned int idx, double val)
{
   if (game == NULL) return;

   ua_player& pl = game->get_underworld().get_player();
   switch(idx)
   {
   case 0: pl.set_pos(val, pl.get_ypos()); break;
   case 1: pl.set_pos(pl.get_xpos(), val); break;
   case 2: pl.set_height(val); break;
   case 3: pl.set_angle_rot(val); break;
   default:
      break;
   }
}

unsigned int ua_debug_server::get_player_attr(unsigned int idx)
{
   if (game == NULL) return 0;
   return game->get_underworld().get_player().get_attr((ua_player_attributes)idx);
}

void ua_debug_server::set_player_attr(unsigned int idx, unsigned int val)
{
   if (game == NULL) return;

   ua_player& pl = game->get_underworld().get_player();

   unsigned int oldlevel = pl.get_attr(ua_attr_maplevel);

   pl.set_attr((ua_player_attributes)idx,val);

   // changing level? then schedule new level preparation
   if (ua_attr_maplevel == (ua_player_attributes)idx && oldlevel != val)
   {
      // note: cannot prepare textures in this thread, since it doesn't own
      //       the OpenGL rendering context.
      schedule_prepare = true;
   }
}

unsigned int ua_debug_server::get_num_levels()
{
   if (game == NULL) return 0;
   return game->get_underworld().get_levelmaps_list().get_num_levels();
}

double ua_debug_server::get_tile_height(unsigned int level, double xpos,
   double ypos)
{
   if (game == NULL) return 0.0;
   return game->get_underworld().get_levelmaps_list().get_level(level).
      get_floor_height(xpos,ypos);
}

unsigned int ua_debug_server::get_tile_info_value(unsigned int level,
   unsigned int xpos, unsigned int ypos, unsigned int type)
{
   unsigned int val = 0;

   ua_levelmap_tile& tile = game->get_underworld().get_levelmaps_list().
      get_level(level).get_tile(xpos,ypos);

   switch(type)
   {
   case ua_tile_info_type:
      val = tile.type;
      break;
   case ua_tile_info_floor_height:
      val = tile.floor;
      break;
   case ua_tile_info_ceiling_height:
      val = tile.ceiling;
      break;
   case ua_tile_info_slope:
      val = tile.slope;
      break;
   case ua_tile_info_tex_wall:
      val = tile.texture_wall;
      break;
   case ua_tile_info_tex_floor:
      val = tile.texture_floor;
      break;
   case ua_tile_info_tex_ceil:
      val = tile.texture_ceiling;
      break;
   case ua_tile_info_objlist_start:
      val = game->get_underworld().get_levelmaps_list().
         get_level(level).get_mapobjects().get_tile_list_start(xpos,ypos);
      break;

   default:
      ua_assert(false);
      break;
   }
   return val;
}

void ua_debug_server::set_tile_info_value(unsigned int level,
   unsigned int xpos, unsigned int ypos, unsigned int type,
   unsigned int val)
{
   ua_levelmap_tile& tile = game->get_underworld().get_levelmaps_list().
      get_level(level).get_tile(xpos,ypos);

   switch(type)
   {
   case ua_tile_info_type:
      tile.type = static_cast<ua_levelmap_tiletype>(val);
      break;
   case ua_tile_info_floor_height:
      tile.floor = static_cast<Uint16>(val);
      break;
   case ua_tile_info_ceiling_height:
      tile.ceiling = static_cast<Uint16>(val);
      break;
   case ua_tile_info_slope:
      tile.slope = static_cast<Uint8>(val);
      break;
   case ua_tile_info_tex_wall:
      tile.texture_wall = static_cast<Uint16>(val);
      break;
   case ua_tile_info_tex_floor:
      tile.texture_floor = static_cast<Uint16>(val);
      break;
   case ua_tile_info_tex_ceil:
      tile.texture_ceiling = static_cast<Uint16>(val);
      break;
   case ua_tile_info_objlist_start:
      ua_assert(false); // TODO implement
      game->get_underworld().get_levelmaps_list().
         get_level(level).get_mapobjects();//.get_tile_list_start(xpos,ypos);
      break;

   default:
      ua_assert(false);
      break;
   }
}

unsigned int ua_debug_server::get_objlist_info(unsigned int level,
   unsigned int pos, unsigned int type)
{
   unsigned int val = 0;

   ua_object& obj = game->get_underworld().get_levelmaps_list().
      get_level(level).get_mapobjects().get_object(pos);

   ua_object_info& objinfo = obj.get_object_info();
   ua_object_info_ext& extobjinfo = obj.get_ext_object_info();
   switch(type)
   {
   case ua_objlist_info_item_id:
      val = objinfo.item_id;
      break;
   case ua_objlist_info_link:
      val = objinfo.link;
      break;
   case ua_objlist_info_quality:
      val = objinfo.quality;
      break;
   case ua_objlist_info_owner:
      val = objinfo.owner;
      break;
   case ua_objlist_info_quantity:
      val = objinfo.quantity;
      break;
   case ua_objlist_info_xpos:
      val = extobjinfo.xpos;
      break;
   case ua_objlist_info_ypos:
      val = extobjinfo.ypos;
      break;
   case ua_objlist_info_zpos:
      val = extobjinfo.zpos;
      break;
   case ua_objlist_info_heading:
      val = extobjinfo.heading;
      break;
   case ua_objlist_info_flags:
      val = objinfo.flags;
      break;
   case ua_objlist_info_ench:
      val = objinfo.enchanted;
      break;
   case ua_objlist_info_is_quant:
      val = objinfo.is_quantity;
      break;
   case ua_objlist_info_hidden:
      val = objinfo.is_hidden;
      break;

   default:
      ua_assert(false);
      break;
   }
   return val;
}

void ua_debug_server::set_objlist_info(unsigned int level,
   unsigned int pos, unsigned int type, unsigned int value)
{
   unsigned int val = 0;

   ua_object& obj = game->get_underworld().get_levelmaps_list().
      get_level(level).get_mapobjects().get_object(pos);

   ua_object_info& objinfo = obj.get_object_info();
   ua_object_info_ext& extobjinfo = obj.get_ext_object_info();
   switch(type)
   {
   case ua_objlist_info_item_id:
      objinfo.item_id = value;
      break;
   case ua_objlist_info_link:
      objinfo.link = value;
      break;
   case ua_objlist_info_quality:
      objinfo.quality = value;
      break;
   case ua_objlist_info_owner:
      objinfo.owner = value;
      break;
   case ua_objlist_info_quantity:
      objinfo.quantity = value;
      break;
   case ua_objlist_info_xpos:
      val = extobjinfo.xpos = value;
      break;
   case ua_objlist_info_ypos:
      val = extobjinfo.ypos = value;
      break;
   case ua_objlist_info_zpos:
      val = extobjinfo.zpos = value;
      break;
   case ua_objlist_info_heading:
      val = extobjinfo.heading = value;
      break;
   case ua_objlist_info_flags:
      objinfo.flags = value;
      break;
   case ua_objlist_info_ench:
      objinfo.enchanted = value != 0;
      break;
   case ua_objlist_info_is_quant:
      objinfo.is_quantity = value != 0;
      break;
   case ua_objlist_info_hidden:
      objinfo.is_hidden = value != 0;
      break;

   default:
      ua_assert(false);
      break;
   }
}

bool ua_debug_server::enum_gamestr_block(unsigned int index,
   unsigned int& blocknum)
{
   std::set<Uint16>& blockset = game->get_gamestrings().get_stringblock_set();

   if (index > blockset.size())
      return false;

   std::set<Uint16>::const_iterator iter = blockset.begin();
   for(unsigned int n=0; n<index && iter != blockset.end(); n++)
      iter++;

   if (iter == blockset.end())
      return false;

   blocknum = static_cast<unsigned int>(*iter);
   return true;
}

unsigned int ua_debug_server::get_gamestr_blocksize(unsigned int block)
{
   std::vector<std::string> strblock;
   game->get_gamestrings().get_stringblock(block, strblock);

   return strblock.size();
}

unsigned int ua_debug_server::get_game_string(unsigned int block,
   unsigned int nr, char* buffer, unsigned int maxsize)
{
   std::string str = game->get_gamestrings().get_string(block, nr);
   std::string::size_type strsize = str.size();

   if (buffer == NULL || maxsize == 0 || maxsize < strsize+1)
      return strsize + 1;

   strncpy(buffer, str.c_str(), strsize);
   buffer[strsize] = 0;

   return strsize;
}

bool ua_debug_server::get_object_list_imagelist(unsigned int& num_objects, unsigned char* buffer, unsigned int size)
{
   ua_image_manager img_manager;
   std::vector<ua_image> img_list;

   img_manager.init(game->get_settings());
   img_manager.load_list(img_list, "objects");

   if (buffer == NULL || size == 0)
   {
      num_objects = img_list.size();
      return true;
   }

   unsigned int xres = img_list[0].get_xres(), yres = img_list[0].get_yres(), max = img_list.size();

   unsigned int needed_space = max*xres*yres*4;
   if (size < needed_space)
      return false;

   Uint8* pixels = new Uint8[xres*yres*max];

   for(unsigned int n=0; n<max; n++)
      memcpy(&pixels[n*xres*yres], &img_list[n].get_pixels()[0], xres*yres);

   // convert color indices to 32-bit texture
   Uint32* palptr = reinterpret_cast<Uint32*>(img_list[0].get_palette().get());
   Uint32* texptr = reinterpret_cast<Uint32*>(buffer);

   for(unsigned int i=0; i<xres*yres*max; i++)
      *texptr++ = palptr[pixels[i]];

   delete[] pixels;

   return true;
}

ua_debug_code_interface* ua_debug_server::get_code_debugger(unsigned int debugger_id)
{
   std::map<unsigned int, ua_debug_code_interface*>::iterator iter;

   iter = all_code_debugger.find(debugger_id);
   return (iter != all_code_debugger.end()) ? iter->second : NULL;
}

void ua_debug_server::add_message(ua_debug_server_message& msg)
{
   message_queue.push_back(msg);
}
