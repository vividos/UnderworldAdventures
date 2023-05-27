//
// Underworld Adventures - an Ultima Underworld remake project
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
/// \file DebugServer.hpp
/// \brief underworld debugger interface
//
#pragma once

#include <SDL2/SDL_thread.h>
#include <map>
#include <deque>
#include "IDebugServer.hpp"

class IGameInstance;
class DebuggerLibContext;
class LevelEditor;

/// debug server message that gets sent to client
struct DebugServerMessage
{
   /// ctor
   DebugServerMessage()
      :messageType(debugMessageUnknown),
      messageArg1(0),
      messageArg2(0),
      messageArg3(0.0)
   {
   }

   /// message type; see enum DebugServerMessageType
   DebugServerMessageType messageType;

   /// message argument 1
   unsigned int messageArg1;
   /// message argument 2
   unsigned int messageArg2;
   /// message argument 3
   double messageArg3;

   /// message text
   std::string messageText;
};

/// server side class for underworld debugger
class DebugServer : public IDebugServer
{
public:
   /// ctor
   DebugServer();
   /// dtor
   virtual ~DebugServer();

   /// inits debug server
   void Init();

   /// returns game instance
   virtual IGameInstance& GetGameInstance() override { return *m_game; }

   /// starts debugger client; returns if debugger was already running
   virtual bool StartDebugger(IGameInstance* game) override;

   /// returns if the debugger is running
   bool IsDebuggerRunning();

   /// does tick processing
   void Tick();

   /// waits for the debugger client to shutdown
   void Shutdown();

   /// locks or unlocks debugger access to underworld
   virtual void Lock(bool locked) override;

   /// returns underworld object; const version
   virtual const Underworld::Underworld& GetUnderworld() const override;

   /// returns underworld object; non-const version
   virtual Underworld::Underworld& GetUnderworld() override;

   /// starts code debugger
   virtual void StartCodeDebugger(ICodeDebugger* codeDebugger) override;

   /// ends code debugging
   virtual void EndCodeDebugger(ICodeDebugger* codeDebugger) override;

   /// sends code debugger update message
   virtual void SendCodeDebuggerStatusUpdate(unsigned int debuggerId) override;

private:
   /// debugger thread procedure
   static int ThreadProc(void* ptr);

   // IDebugServer methods

   virtual bool CheckInterfaceVersion(unsigned int interfaceVersion) override;

   virtual unsigned int GetFlag(unsigned int flagId) override;

   virtual size_t GetGamePath(char* buffer, size_t bufferSize) override;

   virtual void LoadGame(const char* path) override;

   virtual bool PauseGame(bool pause) override;

   virtual size_t GetNumMessages() override;

   virtual bool GetMessage(unsigned int& messageType,
      unsigned int& messageArg1, unsigned int& messageArg2, double& messageArg3,
      size_t& messageTextSize) override;

   virtual bool GetMessageText(char* buffer, size_t bufferSize) override;

   virtual bool PopMessage() override;

   virtual double GetPlayerPosInfo(size_t index) override;

   virtual void SetPlayerPosInfo(size_t index, double value) override;

   virtual unsigned int GetPlayerAttribute(size_t index) override;

   virtual void SetPlayerAttribute(size_t index, unsigned int value) override;

   virtual size_t GetNumLevels() const override;

   virtual bool IsLevelEmpty(size_t levelIndex) const override;

   virtual double GetTileHeight(size_t level, double xpos,
      double ypos) override;

   virtual unsigned int GetTileInfoValue(size_t level,
      unsigned int xpos, unsigned int ypos, unsigned int type) override;

   virtual void SetTileInfoValue(size_t level,
      unsigned int xpos, unsigned int ypos, unsigned int type,
      unsigned int value) override;

   virtual bool IsObjectListIndexAvail(size_t level, size_t pos) const override;

   virtual unsigned int GetObjectListInfo(size_t level,
      size_t pos, unsigned int type) override;

   virtual void SetObjectListInfo(size_t level,
      size_t pos, unsigned int type, unsigned int value) override;

   virtual bool EnumGameStringsBlocks(size_t index,
      size_t& blockNumber) override;

   virtual size_t GetGameStringsBlockSize(size_t block) override;

   virtual size_t GetGameString(size_t block, size_t number,
      char* buffer, size_t maxsize) override;

   virtual bool GetObjectListImagelist(size_t& numObjects,
      unsigned char* buffer, size_t size) override;

   /// adds message to client message queue
   void AddMessage(DebugServerMessage& msg);

   /// removes messages with matching message type and arg1 from message queue
   void RemoveMessages(DebugServerMessageType messageType, unsigned int messageArg1);

   virtual ICodeDebugger* GetCodeDebugger(unsigned int debuggerId) override;

   /// creates a level editor for given window handle
   virtual std::shared_ptr<LevelEditor> CreateLevelEditor(const void* windowHandle) override;

private:
   /// debug library context
   std::unique_ptr<DebuggerLibContext> m_debugLibrary;

   /// debug thread
   SDL_Thread* m_debuggerThread;

   /// semaphore that indicates if debugger still runs
   SDL_sem* m_debuggerSemaphore;

   /// mutex to lock/unlock underworld object
   SDL_mutex* m_underworldLock;

   /// game instance
   IGameInstance* m_game;

   /// is true when new level textures should be prepared at next Tick()
   bool m_schedulePrepare;

   /// message queue
   std::deque<DebugServerMessage> m_messageQueue;

   /// map of all code debugger
   std::map<unsigned int, ICodeDebugger*> m_mapCodeDebugger;

   /// id of last code debugger
   unsigned int m_lastCodeDebuggerId;
};
