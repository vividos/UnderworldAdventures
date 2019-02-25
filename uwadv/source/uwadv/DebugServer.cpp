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
#include "DebugServer.hpp"
#include "GameInterface.hpp"
#include "GameLogic.hpp"
#include "Underworld.hpp"
#include "GameStrings.hpp"
#include "IndexedImage.hpp"
#include <algorithm>

#ifdef WIN32
/// win32 debug lib context
class DebuggerLibContextWin32 : public DebuggerLibContext
{
public:
   DebuggerLibContextWin32()
      :m_dll(NULL)
   {
   }

   virtual void Init() override
   {
      m_dll = ::LoadLibrary("uadebug.dll");
   }

   virtual void Done() override
   {
      ::FreeLibrary(m_dll);
   }

   virtual bool IsAvail() override
   {
      return m_dll != NULL && ::GetProcAddress(m_dll, "uadebug_start") != NULL;
   }

   virtual void StartDebugger(IDebugServer* server) override
   {
      typedef void(*uadebug_start_func)(void*);

      // get function pointer
      uadebug_start_func uadebug_start =
         (uadebug_start_func)::GetProcAddress(m_dll, "uadebug_start");

      // start debugger
      uadebug_start(server);
   }

protected:
   /// DLL module handle
   HMODULE m_dll;
};
#endif

#ifdef LINUX
/// \brief linux debug lib context
/// Uses dlopen() and dlsym() to load the shared object libuadebug.so and
/// start the debugger
class DebuggerContextLinux : public DebuggerLibContext
{
public:
   /// ctor
   DebuggerContextLinux() {}
   /// inits debug server
   virtual void Init() override {}
   /// cleans up debug server
   virtual void Done() override {}
   /// returns if debugger is available
   virtual bool IsAvail() override { return false; }
   /// starts debugger
   virtual void StartDebugger(IDebugServer* server) override {}
};
#endif

DebugServer::DebugServer()
   :m_debugLibrary(NULL),
   m_debuggerThread(NULL),
   m_debuggerSemaphore(NULL),
   m_underworldLock(NULL),
   m_game(NULL),
   m_schedulePrepare(false),
   m_lastCodeDebuggerId(0)
{
   // init mutex, semaphore and thread handle
   m_underworldLock = SDL_CreateMutex();
   m_debuggerSemaphore = SDL_CreateSemaphore(0);

#ifdef WIN32
   m_debugLibrary = new DebuggerLibContextWin32;
#else
   m_debugLibrary = new DebuggerLibContext;
#endif

   UaAssert(m_debugLibrary != NULL);
}

DebugServer::~DebugServer()
{
   SDL_DestroyMutex(m_underworldLock);

   SDL_DestroySemaphore(m_debuggerSemaphore);

   // TODO if (m_debuggerThread != NULL)
   //   SDL_KillThread(m_debuggerThread);

   m_debugLibrary->Done();
   delete m_debugLibrary;
   m_debugLibrary = NULL;
}

void DebugServer::Init()
{
   m_debugLibrary->Init();

   UaTrace("debug server inited; debugger is %savailable\n",
      m_debugLibrary->IsAvail() ? "" : "not ");
}

bool DebugServer::StartDebugger(IBasicGame* game)
{
   m_game = game;

   // check if debugger already runs
   if (m_debugLibrary->IsAvail() && SDL_SemValue(m_debuggerSemaphore) == 0)
   {
      UaTrace("starting uadebug thread\n");

      // start new thread
      m_debuggerThread = SDL_CreateThread(ThreadProc, "debugger-thread", this);
   }
   else
   {
      UaTrace(!m_debugLibrary->IsAvail() ?
         "debugger not available\n" : "debugger already running\n");
   }

   return false;
}

bool DebugServer::IsDebuggerRunning()
{
   Lock(true);
   bool ret = SDL_SemValue(m_debuggerSemaphore) != 0;
   Lock(false);

   return ret;
}

void DebugServer::Tick()
{
   if (IsDebuggerRunning())
   {
      Lock(true);

      if (m_schedulePrepare)
      {
         // change level per underworld object
         m_schedulePrepare = false;
         Underworld::GameLogic& gameLogic = m_game->GetGameLogic();
         gameLogic.ChangeLevel(gameLogic.GetUnderworld().GetPlayer().GetAttribute(Underworld::attrMapLevel));
      }

      Lock(false);
   }
}

void DebugServer::Shutdown()
{
   if (m_debugLibrary->IsAvail() && m_debuggerThread != NULL)
   {
      UaTrace("shutting down debugger...\n");

      // send quit message
      DebugServerMessage msg;
      msg.messageType = debugMessageShutdown;

      Lock(true);
      m_messageQueue.clear();
      AddMessage(msg);
      Lock(false);

      // wait for thread
      SDL_WaitThread(m_debuggerThread, NULL);
      m_debuggerThread = NULL;
   }
}

void DebugServer::Lock(bool locked)
{
   if (locked)
      SDL_LockMutex(m_underworldLock);
   else
      SDL_UnlockMutex(m_underworldLock);
}

int DebugServer::ThreadProc(void* ptr)
{
   DebugServer* This = reinterpret_cast<DebugServer*>(ptr);

   // increase semaphore count
   SDL_SemPost(This->m_debuggerSemaphore);

   if (This->m_debugLibrary->IsAvail())
      This->m_debugLibrary->StartDebugger(This);

   UaTrace("uadebug thread ended\n");

   // decrease semaphore count
   SDL_SemWait(This->m_debuggerSemaphore);

   return 0;
}

void DebugServer::StartCodeDebugger(ICodeDebugger* codeDebugger)
{
   unsigned int debuggerId = ++m_lastCodeDebuggerId;

   UaTrace("debug: starting code debugger, id=%u\n", debuggerId);

   codeDebugger->SetDebuggerId(debuggerId);

   // send "start" message
   DebugServerMessage msg;
   msg.messageType = debugMessageStartCodeDebugger;
   msg.messageArg1 = debuggerId;

   Lock(true);
   AddMessage(msg);

   m_mapCodeDebugger.insert(std::make_pair(debuggerId, codeDebugger));

   Lock(false);
}

void DebugServer::EndCodeDebugger(ICodeDebugger* codeDebugger)
{
   Lock(true);

   // search code debugger in map
   std::map<unsigned int, ICodeDebugger*>::iterator iter, stop;
   iter = m_mapCodeDebugger.begin();
   stop = m_mapCodeDebugger.end();

   unsigned int debuggerId = 0;
   for (; iter != stop; iter++)
   {
      if (iter->second == codeDebugger)
      {
         debuggerId = iter->first;

         m_mapCodeDebugger.erase(iter);
         break;
      }
   }

   //UaAssert(iter != stop); // assert when pointer was not found

   UaTrace("debug: ending code debugger, id=%u\n", debuggerId);

   // remove all "start code debugger" messages from the same code debugger
   unsigned int max = static_cast<unsigned int>(m_messageQueue.size());
   for (unsigned int n = 0; n < max; n++)
   {
      DebugServerMessage& queue_msg = m_messageQueue[n];
      if (queue_msg.messageType == debugMessageStartCodeDebugger && queue_msg.messageArg1 == debuggerId)
      {
         m_messageQueue.erase(m_messageQueue.begin() + n);
         --n;
         --max;
      }
   }

   // send "end" message
   DebugServerMessage msg;
   msg.messageType = debugMessageEndCodeDebugger;
   msg.messageArg1 = debuggerId;

   AddMessage(msg);
   Lock(false);
}

void DebugServer::SendCodeDebuggerStatusUpdate(unsigned int debuggerId)
{
   // send notification about changed state
   DebugServerMessage msg;
   msg.messageType = debugMessageCodeDebuggerStateUpdate;
   msg.messageArg1 = debuggerId;

   Lock(true);

   // remove all previous "status update" messages from the same code debugger
   unsigned int max = static_cast<unsigned int>(m_messageQueue.size());
   for (unsigned int n = 0; n < max; n++)
   {
      DebugServerMessage& queue_msg = m_messageQueue[n];
      if (queue_msg.messageType == msg.messageType && queue_msg.messageArg1 == msg.messageArg1)
      {
         m_messageQueue.erase(m_messageQueue.begin() + n);
         --n;
         --max;
      }
   }

   AddMessage(msg);
   Lock(false);
}

bool DebugServer::CheckInterfaceVersion(unsigned int interfaceVersion)
{
   return interfaceVersion == c_debugServerInterfaceVersion;
}

unsigned int DebugServer::GetFlag(unsigned int flagId)
{
   unsigned int flag = 0;
   switch (flagId)
   {
   case debugServerFlagIsStudioMode:
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

unsigned int DebugServer::GetGamePath(char* buffer, unsigned int bufferSize)
{
   Base::Settings& settings = m_game->GetSettings();

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

   if (bufferSize < strsize + 1)
      return strsize + 1;

   strncpy(buffer, game_path.c_str(), strsize);
   buffer[strsize] = 0;

   return std::min(game_path.size(), bufferSize);
}

void DebugServer::LoadGame(const char* path)
{
   m_game->DoneGame();

   std::string strpath(path);
   m_game->GetSettings().SetValue(Base::settingGamePrefix, strpath);

   m_game->InitGame();
}

bool DebugServer::PauseGame(bool pause)
{
   return m_game->PauseGame(pause);
}

unsigned int DebugServer::GetNumMessages()
{
   return m_messageQueue.size();
}

bool DebugServer::GetMessage(unsigned int& messageType,
   unsigned int& messageArg1, unsigned int& messageArg2, double& messageArg3,
   unsigned int& messageTextSize)
{
   if (m_messageQueue.size() > 0)
   {
      DebugServerMessage& msg = m_messageQueue.front();
      messageType = msg.messageType;
      messageArg1 = msg.messageArg1;
      messageArg2 = msg.messageArg2;
      messageArg3 = msg.messageArg3;
      messageTextSize = msg.messageText.size() + 1;
   }
   return m_messageQueue.size() > 0;
}

bool DebugServer::GetMessageText(char* buffer, unsigned int bufferSize)
{
   if (m_messageQueue.size() > 0)
   {
      DebugServerMessage& msg = m_messageQueue.front();
      strncpy(buffer, msg.messageText.c_str(), bufferSize);
   }
   return m_messageQueue.size() > 0;
}

bool DebugServer::PopMessage()
{
   if (m_messageQueue.size() > 0)
   {
      m_messageQueue.pop_front();
   }
   return m_messageQueue.size() > 0;
}

double DebugServer::GetPlayerPosInfo(unsigned int idx)
{
   double val = 0.0;
   if (m_game == NULL) return val;

   Underworld::Player& pl = m_game->GetUnderworld().GetPlayer();
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

void DebugServer::SetPlayerPosInfo(unsigned int idx, double val)
{
   if (m_game == NULL) return;

   Underworld::Player& pl = m_game->GetUnderworld().GetPlayer();
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

unsigned int DebugServer::GetPlayerAttribute(unsigned int idx)
{
   if (m_game == NULL) return 0;
   return m_game->GetUnderworld().GetPlayer().GetAttribute((Underworld::PlayerAttribute)idx);
}

void DebugServer::SetPlayerAttribute(unsigned int idx, unsigned int val)
{
   if (m_game == NULL) return;

   Underworld::Player& pl = m_game->GetUnderworld().GetPlayer();

   unsigned int oldlevel = pl.GetAttribute(Underworld::attrMapLevel);

   pl.SetAttribute((Underworld::PlayerAttribute)idx, val);

   // changing level? then schedule new level preparation
   if (Underworld::attrMapLevel == (Underworld::PlayerAttribute)idx && oldlevel != val)
   {
      // note: cannot prepare textures in this thread, since it doesn't own
      //       the OpenGL rendering context.
      m_schedulePrepare = true;
   }
}

unsigned int DebugServer::GetNumLevels()
{
   if (m_game == NULL) return 0;
   return m_game->GetUnderworld().GetLevelList().GetNumLevels();
}

double DebugServer::GetTileHeight(unsigned int level, double xpos,
   double ypos)
{
   if (m_game == NULL) return 0.0;
   return m_game->GetUnderworld().GetLevelList().GetLevel(level).
      GetTilemap().GetFloorHeight(xpos, ypos);
}

unsigned int DebugServer::GetTileInfoValue(unsigned int level,
   unsigned int xpos, unsigned int ypos, unsigned int type)
{
   unsigned int val = 0;

   Underworld::TileInfo& tile = m_game->GetUnderworld().GetLevelList().
      GetLevel(level).GetTilemap().GetTileInfo(xpos, ypos);

   switch (type)
   {
   case debuggerTileInfoType:
      val = tile.m_type;
      break;
   case debuggerTileInfoFloorHeight:
      val = tile.m_floor;
      break;
   case debuggerTileInfoCeilingHeight:
      val = tile.m_ceiling;
      break;
   case debuggerTileInfoSlope:
      val = tile.m_slope;
      break;
   case debuggerTileInfoTextureWall:
      val = tile.m_textureWall;
      break;
   case debuggerTileInfoTextureFloor:
      val = tile.m_textureFloor;
      break;
   case debuggerTileInfoTextureCeiling:
      val = tile.m_textureCeiling;
      break;
   case debuggerTileInfoObjectListStart:
      val = m_game->GetUnderworld().GetLevelList().
         GetLevel(level).GetObjectList().GetListStart(xpos, ypos);
      break;

   default:
      UaAssert(false);
      break;
   }
   return val;
}

void DebugServer::SetTileInfoValue(unsigned int level,
   unsigned int xpos, unsigned int ypos, unsigned int type,
   unsigned int val)
{
   Underworld::TileInfo& tile = m_game->GetUnderworld().GetLevelList().
      GetLevel(level).GetTilemap().GetTileInfo(xpos, ypos);

   switch (type)
   {
   case debuggerTileInfoType:
      tile.m_type = static_cast<Underworld::TilemapTileType>(val);
      break;
   case debuggerTileInfoFloorHeight:
      tile.m_floor = static_cast<Uint16>(val);
      break;
   case debuggerTileInfoCeilingHeight:
      tile.m_ceiling = static_cast<Uint16>(val);
      break;
   case debuggerTileInfoSlope:
      tile.m_slope = static_cast<Uint8>(val);
      break;
   case debuggerTileInfoTextureWall:
      tile.m_textureWall = static_cast<Uint16>(val);
      break;
   case debuggerTileInfoTextureFloor:
      tile.m_textureFloor = static_cast<Uint16>(val);
      break;
   case debuggerTileInfoTextureCeiling:
      tile.m_textureCeiling = static_cast<Uint16>(val);
      break;
   case debuggerTileInfoObjectListStart:
      UaAssert(false); // TODO implement
      m_game->GetUnderworld().GetLevelList().
         GetLevel(level).GetObjectList();//.GetListStart(xpos,ypos);
      break;

   default:
      UaAssert(false);
      break;
   }
}

unsigned int DebugServer::GetObjectListInfo(unsigned int level,
   unsigned int pos, unsigned int type)
{
   unsigned int val = 0;

   Underworld::ObjectPtr obj = m_game->GetUnderworld().GetLevelList().
      GetLevel(level).GetObjectList().GetObject(pos);

   Underworld::ObjectInfo& objinfo = obj->GetObjectInfo();
   Underworld::ObjectPositionInfo& posInfo = obj->GetPosInfo();

   switch (type)
   {
   case debuggerObjectListInfoItemId:
      val = objinfo.m_itemID;
      break;
   case debuggerObjectListInfoLink:
      val = objinfo.m_link;
      break;
   case debuggerObjectListInfoQuality:
      val = objinfo.m_quality;
      break;
   case debuggerObjectListInfoOwner:
      val = objinfo.m_owner;
      break;
   case debuggerObjectListInfoQuantity:
      val = objinfo.m_quantity;
      break;
   case debuggerObjectListInfoXPos:
      val = posInfo.m_xpos;
      break;
   case debuggerObjectListInfoYPos:
      val = posInfo.m_ypos;
      break;
   case debuggerObjectListInfoZPos:
      val = posInfo.m_zpos;
      break;
   case debuggerObjectListInfoHeading:
      val = posInfo.m_heading;
      break;
   case debuggerObjectListInfoFlags:
      val = objinfo.m_flags;
      break;
   case debuggerObjectListInfoEnchanted:
      val = objinfo.m_isEnchanted;
      break;
   case debuggerObjectListInfoIsQuantity:
      val = objinfo.m_isQuantity;
      break;
   case debuggerObjectListInfoHidden:
      val = objinfo.m_isHidden;
      break;

   default:
      UaAssert(false);
      break;
   }
   return val;
}

void DebugServer::SetObjectListInfo(unsigned int level,
   unsigned int pos, unsigned int type, unsigned int value)
{
   unsigned int val = 0;

   Underworld::ObjectPtr obj = m_game->GetUnderworld().GetLevelList().
      GetLevel(level).GetObjectList().GetObject(pos);

   Underworld::ObjectInfo& objinfo = obj->GetObjectInfo();
   Underworld::ObjectPositionInfo& posInfo = obj->GetPosInfo();

   switch (type)
   {
   case debuggerObjectListInfoItemId:
      objinfo.m_itemID = value;
      break;
   case debuggerObjectListInfoLink:
      objinfo.m_link = value;
      break;
   case debuggerObjectListInfoQuality:
      objinfo.m_quality = value;
      break;
   case debuggerObjectListInfoOwner:
      objinfo.m_owner = value;
      break;
   case debuggerObjectListInfoQuantity:
      objinfo.m_quantity = value;
      break;
   case debuggerObjectListInfoXPos:
      val = posInfo.m_xpos = value;
      break;
   case debuggerObjectListInfoYPos:
      val = posInfo.m_ypos = value;
      break;
   case debuggerObjectListInfoZPos:
      val = posInfo.m_zpos = value;
      break;
   case debuggerObjectListInfoHeading:
      val = posInfo.m_heading = value;
      break;
   case debuggerObjectListInfoFlags:
      objinfo.m_flags = value;
      break;
   case debuggerObjectListInfoEnchanted:
      objinfo.m_isEnchanted = value != 0;
      break;
   case debuggerObjectListInfoIsQuantity:
      objinfo.m_isQuantity = value != 0;
      break;
   case debuggerObjectListInfoHidden:
      objinfo.m_isHidden = value != 0;
      break;

   default:
      UaAssert(false);
      break;
   }
}

bool DebugServer::EnumGameStringsBlocks(unsigned int index,
   unsigned int& blockNumber)
{
   const GameStrings& gameStrings = m_game->GetGameStrings();
   const std::set<Uint16>& blockset = gameStrings.GetStringBlockSet();

   if (index > blockset.size())
      return false;

   std::set<Uint16>::const_iterator iter = blockset.begin();
   for (unsigned int n = 0; n < index && iter != blockset.end(); n++)
      iter++;

   if (iter == blockset.end())
      return false;

   blockNumber = static_cast<unsigned int>(*iter);
   return true;
}

unsigned int DebugServer::GetGameStringsBlockSize(unsigned int block)
{
   const std::vector<std::string>& strblock = m_game->GetGameStrings().GetStringBlock(block);

   return strblock.size();
}

unsigned int DebugServer::GetGameString(unsigned int block,
   unsigned int nr, char* buffer, unsigned int maxsize)
{
   std::string str = m_game->GetGameStrings().GetString(block, nr);
   std::string::size_type strsize = str.size();

   if (buffer == NULL || maxsize == 0 || maxsize < strsize + 1)
      return strsize + 1;

   strncpy(buffer, str.c_str(), strsize);
   buffer[strsize] = 0;

   return strsize;
}

bool DebugServer::GetObjectListImagelist(unsigned int& numObjects, unsigned char* buffer, unsigned int size)
{
   ImageManager imageManager;
   std::vector<IndexedImage> img_list;

   imageManager.Init(m_game->GetSettings());
   imageManager.LoadList(img_list, "objects");

   if (buffer == NULL || size == 0)
   {
      numObjects = img_list.size();
      return true;
   }

   unsigned int xres = img_list[0].GetXRes(), yres = img_list[0].GetYRes(), max = img_list.size();

   unsigned int needed_space = max * xres*yres * 4;
   if (size < needed_space)
      return false;

   Uint8* pixels = new Uint8[xres*yres*max];

   for (unsigned int n = 0; n < max; n++)
      memcpy(&pixels[n*xres*yres], &img_list[n].GetPixels()[0], xres*yres);

   // convert color indices to 32-bit texture
   Uint32* palptr = reinterpret_cast<Uint32*>(img_list[0].GetPalette().get());
   Uint32* texptr = reinterpret_cast<Uint32*>(buffer);

   for (unsigned int i = 0; i < xres*yres*max; i++)
      *texptr++ = palptr[pixels[i]];

   delete[] pixels;

   return true;
}

ICodeDebugger* DebugServer::GetCodeDebugger(unsigned int debuggerId)
{
   std::map<unsigned int, ICodeDebugger*>::iterator iter;

   iter = m_mapCodeDebugger.find(debuggerId);
   return (iter != m_mapCodeDebugger.end()) ? iter->second : NULL;
}

void DebugServer::AddMessage(DebugServerMessage& msg)
{
   m_messageQueue.push_back(msg);
}
