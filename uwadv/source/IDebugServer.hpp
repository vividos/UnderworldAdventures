//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2004,2005,2019 Underworld Adventures Team
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
/// \file IDebugServer.hpp
/// \brief debug server interface
//
#pragma once

class IBasicGame;

/// current debug server interface version
const unsigned int c_debugServerInterfaceVersion = 1;

/// debug server message types; usable in DebugServerMessage::messageType
enum DebugServerMessageType
{
   /// a message to shutdown the debugger
   debugMessageShutdown = 0,

   /// \brief a code debugger has started
   /// messageArg1 contains the code debugger handle
   debugMessageStartCodeDebugger,

   /// \brief a code debugger has ended
   /// messageArg1 contains the code debugger handle
   debugMessageEndCodeDebugger,

   /// \brief a code debugger has updated his debug state
   /// messageArg1 contains the code debugger handle
   debugMessageCodeDebuggerStateUpdate,

   /// a code debugger message
   //debugMessageCodeDebugger,

   /// miscellaneous message \todo needed?
   debugMessageMisc,

   /// unknown message
   debugMessageUnknown,
};

/// debug server flags, used in IDebugServer::GetFlag()
enum DebugServerFlags
{
   debugServerFlagIsStudioMode = 0, ///< is in studio mode?
};

/// tile info type for GetTileInfoValue() and SetTileInfoValue()
enum DebugServerTileInfo
{
   debuggerTileInfoType = 0,
   debuggerTileInfoFloorHeight,
   debuggerTileInfoCeilingHeight,
   debuggerTileInfoSlope,
   debuggerTileInfoTextureWall,
   debuggerTileInfoTextureFloor,
   debuggerTileInfoTextureCeiling,
   debuggerTileInfoObjectListStart,
};

/// object list info type for GetObjectListInfo
enum DebugServerObjectListInfo
{
   debuggerObjectListInfoItemId = 0,
   debuggerObjectListInfoLink,
   debuggerObjectListInfoQuality,
   debuggerObjectListInfoOwner,
   debuggerObjectListInfoQuantity,
   debuggerObjectListInfoXPos,
   debuggerObjectListInfoYPos,
   debuggerObjectListInfoZPos,
   debuggerObjectListInfoHeading,
   debuggerObjectListInfoFlags,
   debuggerObjectListInfoEnchanted,
   debuggerObjectListInfoIsQuantity,
   debuggerObjectListInfoHidden,
};

/// code debugger type
enum DebugServerCodeDebuggerType
{
   codeDebuggerTypeUnderworldConversation = 0, ///< uw conversation script
   codeDebuggerTypeLuaScript, ///< Lua script
};

/// code debugger state
enum DebugServerCodeDebuggerState
{
   codeDebuggerStateInactive = 0,
   codeDebuggerStateRunning,
   codeDebuggerStateBreak,
};

/// code debugger command
enum DebugServerCodeDebuggerCommand
{
   codeDebuggerCommandRun = 0,   ///< continue running code until next breakpoint or end
   codeDebuggerCommandStepOver,  ///< continue with one line of code, not stepping into functions
   codeDebuggerCommandStepInto,  ///< continue with one line of code, stepping into functions
   codeDebuggerCommandStepOut,   ///< continue to the function above
};


/// code location
struct DebugCodeLocation
{
   DebugCodeLocation()
      :sourcefileIndex(unsigned(-1)),
      sourcefileLine(0), codePosition(unsigned(-1))
   {
   }

   unsigned int sourcefileIndex;
   unsigned int sourcefileLine;
   unsigned int codePosition;
};


/// breakpoint informations
struct DebugCodeBreakpointInfo
{
   DebugCodeBreakpointInfo()
      :visible(true)
   {
   }

   /// breakpoint location
   DebugCodeLocation pos;

   bool visible; ///< indicates if breakpoint is visible to debugger

   //unsigned int count_before_trigger;
   //std::string condition;
};


/// code debugger interface
class ICodeDebugger
{
public:
   /// returns code debugger type
   virtual DebugServerCodeDebuggerType GetDebuggerType() = 0;

   /// called by client to let the code debugger prepare debug info
   virtual void PrepareDebugInfo() = 0;

   /// returns current debugger state
   virtual DebugServerCodeDebuggerState GetDebuggerState() const = 0;

   /// sets new debugger state
   virtual void SetDebuggerState(DebugServerCodeDebuggerState state) = 0;

   /// returns current debugger command
   virtual DebugServerCodeDebuggerCommand GetDebuggerCommand() const = 0;

   /// sets new debugger command
   virtual void SetDebuggerCommand(DebugServerCodeDebuggerCommand command) = 0;

   /// returns current position
   virtual void GetCurrentPos(unsigned int& sourcefileIndex, unsigned int& sourcefileLine,
      unsigned int& codePosition, bool& isSourcefileValid) = 0;

   /// returns number of source files
   virtual unsigned int GetNumSourcefiles() const = 0;

   /// returns sourcefile name by index
   virtual unsigned int GetSourcefileName(unsigned int index, char* buffer, unsigned int len) = 0;

   /// returns number of breakpoints
   virtual unsigned int GetNumBreakpoints() = 0;

   /// returns breakpoint info for a given breakpoint index
   virtual void GetBreakpointInfo(unsigned int breakpointIndex,
      unsigned int& sourcefileIndex, unsigned int& sourcefileLine,
      unsigned int& codePosition, bool& visible) = 0;

protected:
   /// sets debugger id
   void SetDebuggerId(unsigned int debuggerId) { m_debuggerId = debuggerId; }

   /// code debugger id
   unsigned int m_debuggerId;

   friend class DebugServer;
};


/// \brief debug server interface definition
/// All methods of this interface class are virtual, so that they can be
/// called via the vtable, from the uadebug shared library.
class IDebugServer
{
public:
   /// dtor
   virtual ~IDebugServer() {}

   // methods called from debug client

   /// \brief checks interface version
   /// checks interface version used in debug server; when interface used by
   /// the client is different from the version used by the server, the
   /// debugger mustn't proceed
   virtual bool CheckInterfaceVersion(
      unsigned int interfaceVersion = c_debugServerInterfaceVersion) = 0;

   /// returns server flag
   virtual unsigned int GetFlag(unsigned int flagId) = 0;

   /// returns current game path, or "" when none
   virtual unsigned int GetGamePath(char* buffer, unsigned int bufferSize) = 0;

   /// loads new game, or unloads game when "" is set as path
   virtual void LoadGame(const char* path) = 0;

   /// locks/unlocks underworld
   virtual void Lock(bool locked) = 0;

   /// pauses or unpauses game; returns previous game state
   virtual bool PauseGame(bool pause) = 0;

   // messaging stuff

   /// returns number of messages in the message queue
   virtual unsigned int GetNumMessages() = 0;

   /// returns current message
   virtual bool GetMessage(unsigned int& messageType,
      unsigned int& messageArg1, unsigned int& messageArg2, double& messageArg3,
      unsigned int& messageTextSize) = 0;

   /// returns message text of current message
   virtual bool GetMessageText(char* buffer, unsigned int bufferSize) = 0;

   /// removes current message
   virtual bool PopMessage() = 0;

   // player stuff

   /// gets player position info; 0=xpos, 1=ypos, 2=height, 3=rotangle
   virtual double GetPlayerPosInfo(unsigned int idx) = 0;

   /// sets player position info
   virtual void SetPlayerPosInfo(unsigned int idx, double val) = 0;

   /// returns player attribute
   virtual unsigned int GetPlayerAttribute(unsigned int idx) = 0;

   /// sets player attribute
   virtual void SetPlayerAttribute(unsigned int idx, unsigned int val) = 0;

   // level/tile stuff

   /// returns number of levels
   virtual unsigned int GetNumLevels() = 0;

   /// returns tile height at given coordinates
   virtual double GetTileHeight(unsigned int level, double xpos,
      double ypos) = 0;

   /// returns tile info value
   virtual unsigned int GetTileInfoValue(unsigned int level,
      unsigned int xpos, unsigned int ypos, unsigned int type) = 0;

   /// sets tile info value
   virtual void SetTileInfoValue(unsigned int level,
      unsigned int xpos, unsigned int ypos, unsigned int type,
      unsigned int val) = 0;

   /// returns if an object at given object list index is available
   virtual bool IsObjectListIndexAvail(unsigned int level, unsigned int pos) const = 0;

   /// returns object list info
   virtual unsigned int GetObjectListInfo(unsigned int level,
      unsigned int pos, unsigned int type) = 0;

   /// sets object list info
   virtual void SetObjectListInfo(unsigned int level,
      unsigned int pos, unsigned int type, unsigned int value) = 0;

   // game strings stuff

   /// enumerates game string blocks
   virtual bool EnumGameStringsBlocks(unsigned int index,
      unsigned int& blockNumber) = 0;

   /// returns number of strings in the given block
   virtual unsigned int GetGameStringsBlockSize(unsigned int block) = 0;

   /// returns game string
   virtual unsigned int GetGameString(unsigned int block, unsigned int nr,
      char* buffer, unsigned int maxsize) = 0;

   /// retrieves an object list imagelist in 32-bit RGBA format
   virtual bool GetObjectListImagelist(unsigned int& numObjects,
      unsigned char* buffer, unsigned int size) = 0;

   // code debugger

   /// retrieves code debugger interface by debugger id
   virtual ICodeDebugger* GetCodeDebugger(unsigned int debuggerId) = 0;
};
