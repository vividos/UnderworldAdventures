//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2019,2021,2022 Underworld Adventures Team
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
/// \file DebugServer.cpp
/// \brief debug server implementation
//
#include "pch.hpp"
#include "DebugServer.hpp"
#include "GameInterface.hpp"
#include "GameLogic.hpp"
#include "Underworld.hpp"
#include "GameStrings.hpp"
#include "IndexedImage.hpp"
#include "ImageManager.hpp"
#include <algorithm>

/// debugger lib context
class DebuggerLibContext
{
public:
   /// ctor; inits debug server
   DebuggerLibContext()
   {
      // SDL adds the .dll or .so extension
      m_uadebugLib = SDL_LoadObject("uadebug");
   }

   /// dtor; cleans up debug server
   ~DebuggerLibContext()
   {
      SDL_UnloadObject(m_uadebugLib);
   }

   /// returns if debugger is available
   bool IsAvail()
   {
      return m_uadebugLib != NULL &&
         SDL_LoadFunction(m_uadebugLib, "uadebug_start") != NULL;
   }

   /// starts debugger
   void StartDebugger(IDebugServer* server)
   {
      UaAssert(IsAvail());

      typedef void(*UadebugStartFunc)(IDebugServer*);

      UadebugStartFunc uadebug_start =
         (UadebugStartFunc)SDL_LoadFunction(m_uadebugLib, "uadebug_start");

      uadebug_start(server);
   }

private:
   /// deleted copy ctor
   DebuggerLibContext(const DebuggerLibContext&) = delete;
   /// deleted assignment operator
   DebuggerLibContext& operator=(const DebuggerLibContext&) = delete;

   /// module handle to uadebug shared library
   void* m_uadebugLib;
};

DebugServer::DebugServer()
   :m_debuggerThread(NULL),
   m_debuggerSemaphore(NULL),
   m_underworldLock(NULL),
   m_game(NULL),
   m_schedulePrepare(false),
   m_lastCodeDebuggerId(0)
{
   // init mutex, semaphore and thread handle
   m_underworldLock = SDL_CreateMutex();
   m_debuggerSemaphore = SDL_CreateSemaphore(0);
}

DebugServer::~DebugServer()
{
   SDL_DestroyMutex(m_underworldLock);

   SDL_DestroySemaphore(m_debuggerSemaphore);

   if (m_debuggerThread != nullptr)
      SDL_WaitThread(m_debuggerThread, nullptr);
}

void DebugServer::Init()
{
   m_debugLibrary = std::make_unique<DebuggerLibContext>();
   UaAssert(m_debugLibrary != NULL);

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
   RemoveMessages(debugMessageStartCodeDebugger, debuggerId);

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
   RemoveMessages(msg.messageType, msg.messageArg1);

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

size_t DebugServer::GetGamePath(char* buffer, size_t bufferSize)
{
   Base::Settings& settings = m_game->GetSettings();

   std::string prefix = settings.GetString(Base::settingGamePrefix);
   if (prefix.empty())
   {
      if (buffer != NULL)
         buffer[0] = 0;
      return 0;
   }

   std::string game_path = settings.GetString(Base::settingUadataPath);
   game_path += prefix;

   size_t strsize = game_path.size();

   if (buffer == nullptr || bufferSize == 0 || bufferSize < strsize + 1)
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

const Underworld::Underworld& DebugServer::GetUnderworld() const
{
   return m_game->GetUnderworld();
}

Underworld::Underworld& DebugServer::GetUnderworld()
{
   return m_game->GetUnderworld();
}

size_t DebugServer::GetNumMessages()
{
   return m_messageQueue.size();
}

bool DebugServer::GetMessage(unsigned int& messageType,
   unsigned int& messageArg1, unsigned int& messageArg2, double& messageArg3,
   size_t& messageTextSize)
{
   if (!m_messageQueue.empty())
   {
      DebugServerMessage& msg = m_messageQueue.front();
      messageType = msg.messageType;
      messageArg1 = msg.messageArg1;
      messageArg2 = msg.messageArg2;
      messageArg3 = msg.messageArg3;
      messageTextSize = msg.messageText.size() + 1;
   }

   return !m_messageQueue.empty();
}

bool DebugServer::GetMessageText(char* buffer, size_t bufferSize)
{
   if (!m_messageQueue.empty())
   {
      DebugServerMessage& msg = m_messageQueue.front();
      if (buffer != nullptr)
         strncpy(buffer, msg.messageText.c_str(), bufferSize);
   }

   return !m_messageQueue.empty();
}

bool DebugServer::PopMessage()
{
   if (!m_messageQueue.empty())
      m_messageQueue.pop_front();

   return !m_messageQueue.empty();
}

double DebugServer::GetPlayerPosInfo(size_t index)
{
   double value = 0.0;
   if (m_game == NULL)
      return value;

   Underworld::Player& pl = m_game->GetUnderworld().GetPlayer();
   switch (index)
   {
   case 0: value = pl.GetXPos(); break;
   case 1: value = pl.GetYPos(); break;
   case 2: value = pl.GetHeight(); break;
   case 3: value = pl.GetRotateAngle(); break;
   default:
      break;
   }

   return value;
}

void DebugServer::SetPlayerPosInfo(size_t index, double value)
{
   if (m_game == NULL)
      return;

   Underworld::Player& pl = m_game->GetUnderworld().GetPlayer();
   switch (index)
   {
   case 0: pl.SetPos(value, pl.GetYPos()); break;
   case 1: pl.SetPos(pl.GetXPos(), value); break;
   case 2: pl.SetHeight(value); break;
   case 3: pl.SetRotateAngle(value); break;
   default:
      break;
   }
}

unsigned int DebugServer::GetPlayerAttribute(size_t index)
{
   if (m_game == NULL)
      return 0;

   if (index < Underworld::attrMax)
   {
      return m_game->GetUnderworld().GetPlayer().GetAttribute((Underworld::PlayerAttribute)index);
   }
   else
   {
      size_t skillIndex = index - Underworld::attrMax;
      return m_game->GetUnderworld().GetPlayer().GetSkill((Underworld::PlayerSkill)skillIndex);
   }
}

void DebugServer::SetPlayerAttribute(size_t index, unsigned int value)
{
   if (m_game == NULL)
      return;

   Underworld::Player& pl = m_game->GetUnderworld().GetPlayer();

   if (index < Underworld::attrMax)
   {
      unsigned int oldlevel = pl.GetAttribute(Underworld::attrMapLevel);

      pl.SetAttribute((Underworld::PlayerAttribute)index, value);

      // changing level? then schedule new level preparation
      if (Underworld::attrMapLevel == (Underworld::PlayerAttribute)index && oldlevel != value)
      {
         // note: cannot prepare textures in this thread, since it doesn't own
         //       the OpenGL rendering context.
         m_schedulePrepare = true;
      }
   }
   else
   {
      size_t skillIndex = index - Underworld::attrMax;
      m_game->GetUnderworld().GetPlayer().SetSkill((Underworld::PlayerSkill)skillIndex, value);
   }
}

size_t DebugServer::GetNumLevels() const
{
   if (m_game == NULL)
      return 0;
   return m_game->GetUnderworld().GetLevelList().GetNumLevels();
}

bool DebugServer::IsLevelEmpty(size_t levelIndex) const
{
   if (m_game == nullptr)
      return true;
   return !m_game->GetUnderworld().GetLevelList().GetLevel(levelIndex).GetTilemap().IsUsed();
}

double DebugServer::GetTileHeight(size_t level, double xpos,
   double ypos)
{
   if (m_game == NULL)
      return 0.0;
   return m_game->GetUnderworld().GetLevelList().GetLevel(level).
      GetTilemap().GetFloorHeight(xpos, ypos);
}

unsigned int DebugServer::GetTileInfoValue(size_t level,
   unsigned int xpos, unsigned int ypos, unsigned int type)
{
   unsigned int value = 0;

   Underworld::TileInfo& tile = m_game->GetUnderworld().GetLevelList().
      GetLevel(level).GetTilemap().GetTileInfo(xpos, ypos);

   switch (type)
   {
   case debuggerTileInfoType:
      value = tile.m_type;
      break;
   case debuggerTileInfoFloorHeight:
      value = tile.m_floor;
      break;
   case debuggerTileInfoCeilingHeight:
      value = tile.m_ceiling;
      break;
   case debuggerTileInfoSlope:
      value = tile.m_slope;
      break;
   case debuggerTileInfoTextureWall:
      value = tile.m_textureWall;
      break;
   case debuggerTileInfoTextureFloor:
      value = tile.m_textureFloor;
      break;
   case debuggerTileInfoTextureCeiling:
      value = tile.m_textureCeiling;
      break;
   case debuggerTileInfoObjectListStart:
      value = m_game->GetUnderworld().GetLevelList().
         GetLevel(level).GetObjectList().GetListStart(xpos, ypos);
      break;
   default:
      UaAssert(false);
      break;
   }

   return value;
}

void DebugServer::SetTileInfoValue(size_t level,
   unsigned int xpos, unsigned int ypos, unsigned int type,
   unsigned int value)
{
   Underworld::TileInfo& tile = m_game->GetUnderworld().GetLevelList().
      GetLevel(level).GetTilemap().GetTileInfo(xpos, ypos);

   switch (type)
   {
   case debuggerTileInfoType:
      tile.m_type = static_cast<Underworld::TilemapTileType>(value);
      break;
   case debuggerTileInfoFloorHeight:
      tile.m_floor = static_cast<Uint16>(value);
      break;
   case debuggerTileInfoCeilingHeight:
      tile.m_ceiling = static_cast<Uint16>(value);
      break;
   case debuggerTileInfoSlope:
      tile.m_slope = static_cast<Uint8>(value);
      break;
   case debuggerTileInfoTextureWall:
      tile.m_textureWall = static_cast<Uint16>(value);
      break;
   case debuggerTileInfoTextureFloor:
      tile.m_textureFloor = static_cast<Uint16>(value);
      break;
   case debuggerTileInfoTextureCeiling:
      tile.m_textureCeiling = static_cast<Uint16>(value);
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

bool DebugServer::IsObjectListIndexAvail(size_t level, size_t pos) const
{
   if (pos == 0)
      return false;

   Underworld::ObjectPtr obj = m_game->GetUnderworld().GetLevelList().
      GetLevel(level).GetObjectList().GetObject(static_cast<Uint16>(pos));

   return obj != NULL;
}

unsigned int DebugServer::GetObjectListInfo(size_t level,
   size_t pos, unsigned int type)
{
   unsigned int value = 0;

   Underworld::ObjectPtr obj = m_game->GetUnderworld().GetLevelList().
      GetLevel(level).GetObjectList().GetObject(static_cast<Uint16>(pos));

   if (obj == NULL)
      return 0;

   Underworld::ObjectInfo& objinfo = obj->GetObjectInfo();
   Underworld::ObjectPositionInfo& posInfo = obj->GetPosInfo();

   switch (type)
   {
   case debuggerObjectListInfoItemId:
      value = objinfo.m_itemID;
      break;
   case debuggerObjectListInfoLink:
      value = objinfo.m_link;
      break;
   case debuggerObjectListInfoQuality:
      value = objinfo.m_quality;
      break;
   case debuggerObjectListInfoOwner:
      value = objinfo.m_owner;
      break;
   case debuggerObjectListInfoQuantity:
      value = objinfo.m_quantity;
      break;
   case debuggerObjectListInfoXPos:
      value = posInfo.m_xpos;
      break;
   case debuggerObjectListInfoYPos:
      value = posInfo.m_ypos;
      break;
   case debuggerObjectListInfoZPos:
      value = posInfo.m_zpos;
      break;
   case debuggerObjectListInfoHeading:
      value = posInfo.m_heading;
      break;
   case debuggerObjectListInfoFlags:
      value = objinfo.m_flags;
      break;
   case debuggerObjectListInfoEnchanted:
      value = objinfo.m_isEnchanted;
      break;
   case debuggerObjectListInfoIsQuantity:
      value = objinfo.m_isQuantity;
      break;
   case debuggerObjectListInfoHidden:
      value = objinfo.m_isHidden;
      break;
   default:
      UaAssert(false);
      break;
   }

   return value;
}

void DebugServer::SetObjectListInfo(size_t level,
   size_t pos, unsigned int type, unsigned int value)
{
   Underworld::ObjectPtr obj = m_game->GetUnderworld().GetLevelList().
      GetLevel(level).GetObjectList().GetObject(static_cast<Uint16>(pos));

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
      posInfo.m_xpos = value;
      break;
   case debuggerObjectListInfoYPos:
      posInfo.m_ypos = value;
      break;
   case debuggerObjectListInfoZPos:
      posInfo.m_zpos = value;
      break;
   case debuggerObjectListInfoHeading:
      posInfo.m_heading = value;
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

bool DebugServer::EnumGameStringsBlocks(size_t index,
   size_t& blockNumber)
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

   blockNumber = static_cast<size_t>(*iter);
   return true;
}

size_t DebugServer::GetGameStringsBlockSize(size_t block)
{
   const std::vector<std::string>& stringBlock = m_game->GetGameStrings().GetStringBlock(static_cast<Uint16>(block));

   return stringBlock.size();
}

size_t DebugServer::GetGameString(size_t block,
   size_t number, char* buffer, size_t maxSize)
{
   std::string text = m_game->GetGameStrings().GetString(static_cast<Uint16>(block), number);
   size_t strsize = text.size();

   if (buffer == NULL || maxSize == 0 || maxSize < strsize + 1)
      return strsize + 1;

   strncpy(buffer, text.c_str(), strsize);
   buffer[strsize] = 0;

   return strsize;
}

bool DebugServer::GetObjectListImagelist(size_t& numObjects, unsigned char* buffer, size_t size)
{
   ImageManager& imageManager = m_game->GetImageManager();

   std::vector<IndexedImage> imageList;
   imageManager.LoadList(imageList, "objects");

   if (buffer == NULL || size == 0)
   {
      numObjects = imageList.size();
      return true;
   }

   size_t xres = imageList[0].GetXRes(), yres = imageList[0].GetYRes(), max = imageList.size();

   size_t neededSpace = max * xres * yres * 4;
   if (size < neededSpace)
      return false;

   Uint8* pixels = new Uint8[xres * yres * max];

   for (size_t index = 0; index < max; index++)
      memcpy(&pixels[index * xres * yres], &imageList[index].GetPixels()[0], xres * yres);

   // convert color indices to 32-bit texture
   Uint32* palptr = reinterpret_cast<Uint32*>(imageList[0].GetPalette().get());
   Uint32* texptr = reinterpret_cast<Uint32*>(buffer);

   for (size_t index = 0; index < xres * yres * max; index++)
      *texptr++ = palptr[pixels[index]];

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

void DebugServer::RemoveMessages(DebugServerMessageType messageType, unsigned int messageArg1)
{
   size_t max = m_messageQueue.size();

   // NOSONAR
   for (size_t index = 0; index < max; index++)
   {
      DebugServerMessage& queue_msg = m_messageQueue[index];
      if (queue_msg.messageType == messageType && queue_msg.messageArg1 == messageArg1)
      {
         m_messageQueue.erase(m_messageQueue.begin() + index);
         --index;
         --max;
      }
   }
}
