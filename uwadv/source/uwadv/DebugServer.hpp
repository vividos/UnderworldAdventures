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
/// \file debug.hpp
/// \brief underworld debugger interface
//
#pragma once

#include <SDL_thread.h>
#include <map>
#include <deque>
#include "IDebugServer.hpp"

class IBasicGame;

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

class DebuggerLibContext
{
public:
   virtual ~DebuggerLibContext() {}
   /// inits debug server
   virtual void Init() {}
   /// cleans up debug server
   virtual void Done() {}
   /// returns if debugger is available
   virtual bool IsAvail() { return false; }
   /// starts debugger
   virtual void StartDebugger(IDebugServer* server) { server; }
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

   /// starts debugger client; returns if debugger was already running
   bool StartDebugger(IBasicGame* game);

   /// returns if the debugger is running
   bool IsDebuggerRunning();

   /// does tick processing
   void Tick();

   /// waits for the debugger client to shutdown
   void Shutdown();

   /// locks or unlocks debugger access to underworld
   virtual void Lock(bool locked) override;

   /// starts code debugger
   void StartCodeDebugger(ICodeDebugger* codeDebugger);

   /// ends code debugging
   void EndCodeDebugger(ICodeDebugger* codeDebugger);

   /// sends code debugger update message
   void SendCodeDebuggerStatusUpdate(unsigned int debuggerId);

protected:
   /// debugger thread procedure
   static int ThreadProc(void* ptr);

   // IDebugServer methods

   virtual bool CheckInterfaceVersion(unsigned int interfaceVersion) override;

   virtual unsigned int GetFlag(unsigned int flagId) override;

   virtual unsigned int GetGamePath(char* buffer, unsigned int bufferSize) override;

   virtual void LoadGame(const char* path) override;

   virtual bool PauseGame(bool pause) override;

   virtual unsigned int GetNumMessages() override;

   virtual bool GetMessage(unsigned int& messageType,
      unsigned int& messageArg1, unsigned int& messageArg2, double& messageArg3,
      unsigned int& messageTextSize) override;

   virtual bool GetMessageText(char* buffer, unsigned int bufferSize) override;

   virtual bool PopMessage() override;

   virtual double GetPlayerPosInfo(unsigned int idx) override;

   virtual void SetPlayerPosInfo(unsigned int idx, double val) override;

   virtual unsigned int GetPlayerAttribute(unsigned int idx) override;

   virtual void SetPlayerAttribute(unsigned int idx, unsigned int val) override;

   virtual unsigned int GetNumLevels() override;

   virtual double GetTileHeight(unsigned int level, double xpos,
      double ypos) override;

   virtual unsigned int GetTileInfoValue(unsigned int level,
      unsigned int xpos, unsigned int ypos, unsigned int type) override;

   virtual void SetTileInfoValue(unsigned int level,
      unsigned int xpos, unsigned int ypos, unsigned int type,
      unsigned int val) override;

   virtual unsigned int GetObjectListInfo(unsigned int level,
      unsigned int pos, unsigned int type) override;

   virtual void SetObjectListInfo(unsigned int level,
      unsigned int pos, unsigned int type, unsigned int value) override;

   virtual bool EnumGameStringsBlocks(unsigned int index,
      unsigned int& blockNumber) override;

   virtual unsigned int GetGameStringsBlockSize(unsigned int block) override;

   virtual unsigned int GetGameString(unsigned int block, unsigned int nr,
      char* buffer, unsigned int maxsize) override;

   virtual bool GetObjectListImagelist(unsigned int& numObjects,
      unsigned char* buffer, unsigned int size) override;

   /// adds message to client message queue
   void AddMessage(DebugServerMessage& msg);

   virtual ICodeDebugger* GetCodeDebugger(unsigned int debuggerId) override;

protected:
   /// debug library context
   DebuggerLibContext* m_debugLibrary;

   /// debug thread
   SDL_Thread* m_debuggerThread;

   /// semaphore that indicates if debugger still runs
   SDL_sem* m_debuggerSemaphore;

   /// mutex to lock/unlock underworld object
   SDL_mutex* m_underworldLock;

   /// pointer to game interface
   IBasicGame* m_game;

   /// is true when new level textures should be prepared at next Tick()
   bool m_schedulePrepare;

   /// message queue
   std::deque<DebugServerMessage> m_messageQueue;

   /// map of all code debugger
   std::map<unsigned int, ICodeDebugger*> m_mapCodeDebugger;

   /// id of last code debugger
   unsigned int m_lastCodeDebuggerId;
};
