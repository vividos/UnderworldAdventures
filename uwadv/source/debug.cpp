//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2004,2005,2019 Underworld Adventures Team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
/// \file debug.cpp
/// \brief debug server implementation
//
#include "common.hpp"
#include "debug.hpp"
#include "dbgserver.hpp"
#include "game_interface.hpp"
#include "gamelogic.hpp"
#include "underworld.hpp"
#include "gamestrings.hpp"
#include "image.hpp"
#include <algorithm>

#ifdef WIN32
/// win32 debug lib context
class ua_debug_lib_context_win32 : public ua_debug_lib_context
{
public:
   ua_debug_lib_context_win32()
      :dll(NULL)
   {
   }

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
      return dll != NULL && ::GetProcAddress(dll, "uadebug_start") != NULL;
   }

   virtual void debug_start(ua_debug_server_interface* server)
   {
      typedef void(*uadebug_start_func)(void*);

      // get function pointer
      uadebug_start_func uadebug_start =
         (uadebug_start_func)::GetProcAddress(dll, "uadebug_start");

      // start debugger
      uadebug_start(server);
   }

protected:
   /// DLL module handle
   HMODULE dll;
};
#endif

#ifdef LINUX
/// linux debug lib context
/*! Uses dlopen() and dlsym() to load the shared object libuadebug.so and
    start the debugger
*/
class ua_debug_lib_context_linux : public ua_debug_lib_context
{
public:
   /// ctor
   ua_debug_lib_context_linux() {}
   /// inits debug server
   virtual void init() {}
   /// cleans up debug server
   virtual void done() {}
   /// returns if debugger is available
   virtual bool is_avail() { return false; }
   /// starts debugger
   virtual void debug_start(ua_debug_server_interface* server) {}
};
#endif

ua_debug_server::ua_debug_server()
   :debug_lib(NULL),
   thread_debugger(NULL),
   sem_debugger(NULL),
   mutex_lock(NULL),
   game(NULL),
   schedule_prepare(false),
   last_code_debugger_id(0)
{
   // init mutex, semaphore and thread handle
   mutex_lock = SDL_CreateMutex();
   sem_debugger = SDL_CreateSemaphore(0);

#ifdef WIN32
   debug_lib = new ua_debug_lib_context_win32;
#else
   debug_lib = new ua_debug_lib_context;
#endif

   UaAssert(debug_lib != NULL);
}

ua_debug_server::~ua_debug_server()
{
   SDL_DestroyMutex(mutex_lock);

   SDL_DestroySemaphore(sem_debugger);

   // TODO if (thread_debugger != NULL)
   //   SDL_KillThread(thread_debugger);

   debug_lib->done();
   delete debug_lib;
   debug_lib = NULL;
}

void ua_debug_server::init()
{
   debug_lib->init();

   UaTrace("debug server inited; debugger is %savailable\n",
      debug_lib->is_avail() ? "" : "not ");
}

bool ua_debug_server::start_debugger(IBasicGame* the_game)
{
   game = the_game;

   // check if debugger already runs
   if (debug_lib->is_avail() && SDL_SemValue(sem_debugger) == 0)
   {
      UaTrace("starting uadebug thread\n");

      // start new thread
      thread_debugger = SDL_CreateThread(thread_proc, "debugger-thread", this);
   }
   else
   {
      UaTrace(!debug_lib->is_avail() ?
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
         Underworld::GameLogic& gameLogic = game->GetGameLogic();
         gameLogic.ChangeLevel(gameLogic.GetUnderworld().GetPlayer().GetAttribute(Underworld::attrMapLevel));
      }

      lock(false);
   }
}

void ua_debug_server::shutdown()
{
   if (debug_lib->is_avail() && thread_debugger != NULL)
   {
      UaTrace("shutting down debugger...\n");

      // send quit message
      ua_debug_server_message msg;
      msg.msg_type = ua_msg_shutdown;

      lock(true);
      message_queue.clear();
      add_message(msg);
      lock(false);

      // wait for thread
      SDL_WaitThread(thread_debugger, NULL);
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

   UaTrace("uadebug thread ended\n");

   // decrease semaphore count
   SDL_SemWait(This->sem_debugger);

   return 0;
}

void ua_debug_server::start_code_debugger(ua_debug_code_interface* code_debugger)
{
   unsigned int debugger_id = ++last_code_debugger_id;

   // send "start" message
   ua_debug_server_message msg;
   msg.msg_type = ua_msg_start_code_debugger;
   msg.msg_arg1 = debugger_id;

   lock(true);
   add_message(msg);

   all_code_debugger.insert(std::make_pair(debugger_id, code_debugger));

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
   for (; iter != stop; iter++)
   {
      if (iter->second == code_debugger)
      {
         debugger_id = iter->first;

         all_code_debugger.erase(iter);
         break;
      }
   }

   UaAssert(iter != stop); // assert when pointer was not found

   // send "end" message
   ua_debug_server_message msg;
   msg.msg_type = ua_msg_end_code_debugger;
   msg.msg_arg1 = debugger_id;

   add_message(msg);
   lock(false);
}

#if 0
void ua_debug_server::start_code_debugger(ua_debug_code_interface* code_debugger)
{
   unsigned int debugger_id = ++last_code_debugger_id;

   UaTrace("debug: starting code debugger, id=%u\n", debugger_id);

   code_debugger->set_debugger_id(debugger_id);

   // send "start" message
   ua_debug_server_message msg;
   msg.msg_type = ua_msg_start_code_debugger;
   msg.msg_arg1 = debugger_id;

   lock(true);

   all_code_debugger.insert(std::make_pair(debugger_id, code_debugger));

   add_message(msg);

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
   for (; iter != stop; iter++)
   {
      if (iter->second == code_debugger)
      {
         debugger_id = iter->first;

         all_code_debugger.erase(iter);
         break;
      }
   }

   UaAssert(iter != stop); // assert when pointer was not found

   UaTrace("debug: ending code debugger, id=%u\n", debugger_id);

   // remove all "start code debugger" messages from the same code debugger
   unsigned int max = static_cast<unsigned int>(message_queue.size());
   for (unsigned int n = 0; n < max; n++)
   {
      ua_debug_server_message& queue_msg = message_queue[n];
      if (queue_msg.msg_type == ua_msg_start_code_debugger && queue_msg.msg_arg1 == debugger_id)
      {
         message_queue.erase(message_queue.begin() + n);
         --n;
         --max;
      }
   }

   // send "end" message
   ua_debug_server_message msg;
   msg.msg_type = ua_msg_end_code_debugger;
   msg.msg_arg1 = debugger_id;

   add_message(msg);
   lock(false);
}
#endif

void ua_debug_server::send_code_debugger_status_update(unsigned int debugger_id)
{
   // send notification about changed state
   ua_debug_server_message msg;
   msg.msg_type = ua_msg_code_debugger_state_update;
   msg.msg_arg1 = debugger_id;

   lock(true);

   // remove all previous "status update" messages from the same code debugger
   unsigned int max = static_cast<unsigned int>(message_queue.size());
   for (unsigned int n = 0; n < max; n++)
   {
      ua_debug_server_message& queue_msg = message_queue[n];
      if (queue_msg.msg_type == msg.msg_type && queue_msg.msg_arg1 == msg.msg_arg1)
      {
         message_queue.erase(message_queue.begin() + n);
         --n;
         --max;
      }
   }

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
   switch (flag_id)
   {
   case ua_flag_is_studio_mode:
#ifdef COMPILE_UASTUDIO
      flag = 1; // in uastudio mode
#else
      flag = 0;
#endif
      break;

   default:
      UaAssert(false);
      break;
   }
   return flag;
}

unsigned int ua_debug_server::get_game_path(char* buffer, unsigned int bufsize)
{
   Base::Settings& settings = game->get_settings();

   std::string prefix = settings.GetString(Base::settingGamePrefix);
   if (prefix.size() == 0)
   {
      if (buffer != NULL)
         buffer[0] = 0;
      return 0;
   }

   std::string game_path = settings.GetString(Base::settingUadataPath);
   game_path += prefix;

   std::string::size_type strsize = game_path.size();

   if (bufsize < strsize + 1)
      return strsize + 1;

   strncpy(buffer, game_path.c_str(), strsize);
   buffer[strsize] = 0;

   return std::min(game_path.size(), bufsize);
}

void ua_debug_server::load_game(const char* path)
{
   game->done_game();

   std::string strpath(path);
   game->get_settings().SetValue(Base::settingGamePrefix, strpath);

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
   if (message_queue.size() > 0)
   {
      ua_debug_server_message& msg = message_queue.front();
      msg_type = msg.msg_type;
      msg_arg1 = msg.msg_arg1;
      msg_arg2 = msg.msg_arg2;
      msg_arg3 = msg.msg_arg3;
      msg_text_size = msg.msg_text.size() + 1;
   }
   return message_queue.size() > 0;
}

bool ua_debug_server::get_message_text(char* buffer, unsigned int bufsize)
{
   if (message_queue.size() > 0)
   {
      ua_debug_server_message& msg = message_queue.front();
      strncpy(buffer, msg.msg_text.c_str(), bufsize);
   }
   return message_queue.size() > 0;
}

bool ua_debug_server::pop_message()
{
   if (message_queue.size() > 0)
   {
      message_queue.pop_front();
   }
   return message_queue.size() > 0;
}

double ua_debug_server::get_player_pos_info(unsigned int idx)
{
   double val = 0.0;
   if (game == NULL) return val;

   Underworld::Player& pl = game->GetUnderworld().GetPlayer();
   switch (idx)
   {
   case 0: val = pl.GetXPos(); break;
   case 1: val = pl.GetYPos(); break;
   case 2: val = pl.GetHeight(); break;
   case 3: val = pl.GetRotateAngle(); break;
   default:
      break;
   }
   return val;
}

void ua_debug_server::set_player_pos_info(unsigned int idx, double val)
{
   if (game == NULL) return;

   Underworld::Player& pl = game->GetUnderworld().GetPlayer();
   switch (idx)
   {
   case 0: pl.SetPos(val, pl.GetYPos()); break;
   case 1: pl.SetPos(pl.GetXPos(), val); break;
   case 2: pl.SetHeight(val); break;
   case 3: pl.SetRotateAngle(val); break;
   default:
      break;
   }
}

unsigned int ua_debug_server::get_player_attr(unsigned int idx)
{
   if (game == NULL) return 0;
   return game->GetUnderworld().GetPlayer().GetAttribute((Underworld::EPlayerAttribute)idx);
}

void ua_debug_server::set_player_attr(unsigned int idx, unsigned int val)
{
   if (game == NULL) return;

   Underworld::Player& pl = game->GetUnderworld().GetPlayer();

   unsigned int oldlevel = pl.GetAttribute(Underworld::attrMapLevel);

   pl.SetAttribute((Underworld::EPlayerAttribute)idx, val);

   // changing level? then schedule new level preparation
   if (Underworld::attrMapLevel == (Underworld::EPlayerAttribute)idx && oldlevel != val)
   {
      // note: cannot prepare textures in this thread, since it doesn't own
      //       the OpenGL rendering context.
      schedule_prepare = true;
   }
}

unsigned int ua_debug_server::GetNumLevels()
{
   if (game == NULL) return 0;
   return game->GetUnderworld().GetLevelList().GetNumLevels();
}

double ua_debug_server::get_tile_height(unsigned int level, double xpos,
   double ypos)
{
   if (game == NULL) return 0.0;
   return game->GetUnderworld().GetLevelList().GetLevel(level).
      GetTilemap().GetFloorHeight(xpos, ypos);
}

unsigned int ua_debug_server::get_tile_info_value(unsigned int level,
   unsigned int xpos, unsigned int ypos, unsigned int type)
{
   unsigned int val = 0;

   Underworld::TileInfo& tile = game->GetUnderworld().GetLevelList().
      GetLevel(level).GetTilemap().GetTileInfo(xpos, ypos);

   switch (type)
   {
   case ua_tile_info_type:
      val = tile.m_type;
      break;
   case ua_tile_info_floor_height:
      val = tile.m_floor;
      break;
   case ua_tile_info_ceiling_height:
      val = tile.m_ceiling;
      break;
   case ua_tile_info_slope:
      val = tile.m_slope;
      break;
   case ua_tile_info_tex_wall:
      val = tile.m_textureWall;
      break;
   case ua_tile_info_tex_floor:
      val = tile.m_textureFloor;
      break;
   case ua_tile_info_tex_ceil:
      val = tile.m_textureCeiling;
      break;
   case ua_tile_info_objlist_start:
      val = game->GetUnderworld().GetLevelList().
         GetLevel(level).GetObjectList().GetListStart(xpos, ypos);
      break;

   default:
      UaAssert(false);
      break;
   }
   return val;
}

void ua_debug_server::set_tile_info_value(unsigned int level,
   unsigned int xpos, unsigned int ypos, unsigned int type,
   unsigned int val)
{
   Underworld::TileInfo& tile = game->GetUnderworld().GetLevelList().
      GetLevel(level).GetTilemap().GetTileInfo(xpos, ypos);

   switch (type)
   {
   case ua_tile_info_type:
      tile.m_type = static_cast<Underworld::TilemapTileType>(val);
      break;
   case ua_tile_info_floor_height:
      tile.m_floor = static_cast<Uint16>(val);
      break;
   case ua_tile_info_ceiling_height:
      tile.m_ceiling = static_cast<Uint16>(val);
      break;
   case ua_tile_info_slope:
      tile.m_slope = static_cast<Uint8>(val);
      break;
   case ua_tile_info_tex_wall:
      tile.m_textureWall = static_cast<Uint16>(val);
      break;
   case ua_tile_info_tex_floor:
      tile.m_textureFloor = static_cast<Uint16>(val);
      break;
   case ua_tile_info_tex_ceil:
      tile.m_textureCeiling = static_cast<Uint16>(val);
      break;
   case ua_tile_info_objlist_start:
      UaAssert(false); // TODO implement
      game->GetUnderworld().GetLevelList().
         GetLevel(level).GetObjectList();//.GetListStart(xpos,ypos);
      break;

   default:
      UaAssert(false);
      break;
   }
}

unsigned int ua_debug_server::get_objlist_info(unsigned int level,
   unsigned int pos, unsigned int type)
{
   unsigned int val = 0;

   Underworld::ObjectPtr obj = game->GetUnderworld().GetLevelList().
      GetLevel(level).GetObjectList().GetObject(pos);

   Underworld::ObjectInfo& objinfo = obj->GetObjectInfo();
   Underworld::ObjectPositionInfo& posInfo = obj->GetPosInfo();

   switch (type)
   {
   case ua_objlist_info_item_id:
      val = objinfo.m_itemID;
      break;
   case ua_objlist_info_link:
      val = objinfo.m_link;
      break;
   case ua_objlist_info_quality:
      val = objinfo.m_quality;
      break;
   case ua_objlist_info_owner:
      val = objinfo.m_owner;
      break;
   case ua_objlist_info_quantity:
      val = objinfo.m_quantity;
      break;
   case ua_objlist_info_xpos:
      val = posInfo.m_xpos;
      break;
   case ua_objlist_info_ypos:
      val = posInfo.m_ypos;
      break;
   case ua_objlist_info_zpos:
      val = posInfo.m_zpos;
      break;
   case ua_objlist_info_heading:
      val = posInfo.m_heading;
      break;
   case ua_objlist_info_flags:
      val = objinfo.m_flags;
      break;
   case ua_objlist_info_ench:
      val = objinfo.m_isEnchanted;
      break;
   case ua_objlist_info_is_quant:
      val = objinfo.m_isQuantity;
      break;
   case ua_objlist_info_hidden:
      val = objinfo.m_isHidden;
      break;

   default:
      UaAssert(false);
      break;
   }
   return val;
}

void ua_debug_server::set_objlist_info(unsigned int level,
   unsigned int pos, unsigned int type, unsigned int value)
{
   unsigned int val = 0;

   Underworld::ObjectPtr obj = game->GetUnderworld().GetLevelList().
      GetLevel(level).GetObjectList().GetObject(pos);

   Underworld::ObjectInfo& objinfo = obj->GetObjectInfo();
   Underworld::ObjectPositionInfo& posInfo = obj->GetPosInfo();

   switch (type)
   {
   case ua_objlist_info_item_id:
      objinfo.m_itemID = value;
      break;
   case ua_objlist_info_link:
      objinfo.m_link = value;
      break;
   case ua_objlist_info_quality:
      objinfo.m_quality = value;
      break;
   case ua_objlist_info_owner:
      objinfo.m_owner = value;
      break;
   case ua_objlist_info_quantity:
      objinfo.m_quantity = value;
      break;
   case ua_objlist_info_xpos:
      val = posInfo.m_xpos = value;
      break;
   case ua_objlist_info_ypos:
      val = posInfo.m_ypos = value;
      break;
   case ua_objlist_info_zpos:
      val = posInfo.m_zpos = value;
      break;
   case ua_objlist_info_heading:
      val = posInfo.m_heading = value;
      break;
   case ua_objlist_info_flags:
      objinfo.m_flags = value;
      break;
   case ua_objlist_info_ench:
      objinfo.m_isEnchanted = value != 0;
      break;
   case ua_objlist_info_is_quant:
      objinfo.m_isQuantity = value != 0;
      break;
   case ua_objlist_info_hidden:
      objinfo.m_isHidden = value != 0;
      break;

   default:
      UaAssert(false);
      break;
   }
}

bool ua_debug_server::enum_gamestr_block(unsigned int index,
   unsigned int& blocknum)
{
   const GameStrings& gameStrings = game->GetGameStrings();
   const std::set<Uint16>& blockset = gameStrings.GetStringBlockSet();

   if (index > blockset.size())
      return false;

   std::set<Uint16>::const_iterator iter = blockset.begin();
   for (unsigned int n = 0; n < index && iter != blockset.end(); n++)
      iter++;

   if (iter == blockset.end())
      return false;

   blocknum = static_cast<unsigned int>(*iter);
   return true;
}

unsigned int ua_debug_server::get_gamestr_blocksize(unsigned int block)
{
   const std::vector<std::string>& strblock = game->GetGameStrings().GetStringBlock(block);

   return strblock.size();
}

unsigned int ua_debug_server::get_game_string(unsigned int block,
   unsigned int nr, char* buffer, unsigned int maxsize)
{
   std::string str = game->GetGameStrings().GetString(block, nr);
   std::string::size_type strsize = str.size();

   if (buffer == NULL || maxsize == 0 || maxsize < strsize + 1)
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

   unsigned int needed_space = max * xres*yres * 4;
   if (size < needed_space)
      return false;

   Uint8* pixels = new Uint8[xres*yres*max];

   for (unsigned int n = 0; n < max; n++)
      memcpy(&pixels[n*xres*yres], &img_list[n].get_pixels()[0], xres*yres);

   // convert color indices to 32-bit texture
   Uint32* palptr = reinterpret_cast<Uint32*>(img_list[0].get_palette().get());
   Uint32* texptr = reinterpret_cast<Uint32*>(buffer);

   for (unsigned int i = 0; i < xres*yres*max; i++)
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
