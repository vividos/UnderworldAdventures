//
// Underworld Adventures Debugger - a debugger tool for Underworld Adventures
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
/// \file DebugClient.hpp
/// \brief debugger client class
//
#pragma once

class IDebugServer;

/// debug server message that is received by the client
struct DebugClientMessage
{
   /// ctor
   DebugClientMessage()
      :m_messageType(0), m_messageArg1(0), m_messageArg2(0), m_messageArg3(0.0) {}

   /// message type; see enum DebugServerMessageType
   unsigned int m_messageType;

   /// message argument 1
   unsigned int m_messageArg1;
   /// message argument 2
   unsigned int m_messageArg2;
   /// message argument 3
   double m_messageArg3;

   /// message text
   CString m_messageText;
};


/// player info interface
class DebugClientPlayerInterface
{
public:
   unsigned int GetAttrCount();
   LPCTSTR GetPosInfoName(unsigned int info);
   LPCTSTR GetAttrName(unsigned int attr);

   double GetPosInfo(unsigned int info);
   unsigned int GetAttribute(unsigned int attr);

   /// retrieves positional info; info can be 0..3
   void SetPosInfo(unsigned int info, double val);

   /// retrieves attributes info; attr can be 0..3
   void SetAttribute(unsigned int attr, unsigned int val);

   void Teleport(unsigned int level, double xpos, double ypos);

private:
   DebugClientPlayerInterface() {}

   /// pointer to debug server interface
   IDebugServer* m_debugInterface;

   friend class DebugClient;
};


/// object list interface
class DebugClientObjectListInterface
{
public:
   unsigned int GetColumnCount() const;

   LPCTSTR GetColumnName(unsigned int columnIndex) const;
   unsigned int GetColumnSize(unsigned int columnIndex) const;
   bool ViewColumnAsHex(unsigned int columnIndex) const;
   unsigned int ColumnHexDigitCount(unsigned int columnIndex) const;

   unsigned int GetItemId(unsigned int pos);
   unsigned int GetItemNext(unsigned int pos);

   bool IsItemInfoAvail(unsigned int pos) const;
   unsigned int GetItemInfo(unsigned int pos, unsigned int subcode);
   void SetItemInfo(unsigned int pos, unsigned int subcode, unsigned int nInfo);

private:
   DebugClientObjectListInterface() {}

   /// pointer to debug server interface
   IDebugServer* m_debugInterface;

   unsigned int m_level;

   friend class DebugClient;
};


/// code debugger type; corresponds with DebugServerCodeDebuggerType in DebugServer.hpp
enum CodeDebuggerType
{
   cdtUwConv = 0,   ///< uw conversation script
   cdtLuaScript   ///< Lua script
};

/// code debugger state; corresponds with DebugServerCodeDebuggerState in DebugServer.hpp
enum CodeDebuggerState
{
   cdsInactive = 0,
   cdsRunning,
   cdsBreak,
};

/// code debugger command; corresponds with DebugServerCodeDebuggerCommand in DebugServer.hpp
enum CodeDebuggerCommand
{
   cdcRun = 0,
   cdcStepOver,
   cdcStepInto,
   cdcStepOut
};


struct CodePosition
{
   /// index value to retrieve source file name
   size_t m_sourceFileNameIndex;
   /// source file line number; 1-based
   size_t m_sourceFileLine;

   /// code position; if not available, -1 is put in here
   size_t m_codePosition;
};

struct BreakpointInfo
{
   bool m_isVisible;
   CodePosition m_location;
};

struct CallstackInfo
{
   unsigned int m_pos;
   unsigned int m_returnPos;
};



/// code debugger on debug client side
class DebugClientCodeDebugger
{
public:
   virtual ~DebugClientCodeDebugger() {}

   // misc.

   /// returns code debugger type
   CodeDebuggerType GetDebuggerType() const;

   /// prepares debug info for code debugger
   void PrepareDebugInfo();

   /// returns true when source file is available
   bool IsSourceAvail() const;

   /// returns true when code is available
   bool IsCodeAvail() const;

   /// sets new debugger command
   void SetCommand(CodeDebuggerCommand command);

   /// returns current code debugger state
   CodeDebuggerState GetState() const;

   /// returns current code position
   CodePosition GetCurrentPos();

   // breakpoints

   size_t GetBreakpointCount() const;
   void GetBreakpointInfo(size_t breakpointIndex, BreakpointInfo& breakpointInfo) const;

   // call stack

   unsigned int GetCallstackHeight() const;
   void GetCallstackInfo(unsigned int atLevel, CallstackInfo& callstackInfo) const;

   // sourcecode files

   // sourcecode files count; only valid if IsSourceAvail() returns a value > 0
   size_t GetSourceFileCount() const;

   /// returns sourcecode filename by index
   CString GetSourceFileName(size_t index);

   /// returns sourcecode filename and line by code position
   bool GetSourceFromCodePos(unsigned int codePos, CString& filename, unsigned int& lineNumber, unsigned int& lineDisplacement);

private:
   /// ctor
   DebugClientCodeDebugger() {}

   /// pointer to code debugger interface
   class ICodeDebugger* m_codeDebugger;

   /// pointer to debug interface
   class DebugClient* m_debugClient;

   friend class DebugClient;
};


enum TileInfoType
{
   tiType = 0,
   tiFloorHeight,
   tiCeilingHeight,
   tiSlope,
   tiTextureWall,
   tiTextureFloor,
   tiTextureCeil,
   tiObjlistStart
};

/// debugger client interface
class DebugClient
{
public:
   DebugClient() {}
   virtual ~DebugClient() {}

   bool Init(IDebugServer* pDebugInterface);

   void Lock(bool locked);

   bool IsStudioMode();
   unsigned int GetFlag(unsigned int flag);
   CString GetGameConfigPath();

   void LoadGameConfig(LPCTSTR pszPrefix);

   bool IsGamePaused();
   void PauseGame(bool pause);

   /// returns underworld object; const version
   const Underworld::Underworld& GetUnderworld() const;

   /// returns underworld object; non-const version
   Underworld::Underworld& GetUnderworld();

   // level stuff

   size_t GetNumLevels();
   void SetWorkingLevel(unsigned int level);
   unsigned int GetWorkingLevel() const { return m_level; }

   CString GetLevelName(unsigned int level) const;

   void InsertNewLevel(unsigned int before_level);

   void MoveLevel(unsigned int level, unsigned int level_insert_point);


   // sub-interfaces

   DebugClientPlayerInterface GetPlayerInterface();

   DebugClientObjectListInterface GetObjectInterface();


   // tile info

   unsigned int GetTileInfo(unsigned int xpos, unsigned int ypos, TileInfoType type);
   void SetTileInfo(unsigned int xpos, unsigned int ypos, TileInfoType type, unsigned int val);


   // game strings

   bool EnumGameStringsBlock(int index, unsigned int& block);
   unsigned int GetGameStringBlockSize(unsigned int block);
   CString GetGameString(unsigned int block, unsigned int nr);


   // code debugger access

   void AddCodeDebugger(unsigned int codeDebuggerId);
   void RemoveCodeDebugger(unsigned int codeDebuggerId);

   bool IsValidCodeDebuggerId(unsigned int codeDebuggerId) const;

   size_t GetCodeDebuggerCount() const;
   unsigned int GetCodeDebuggerByIndex(size_t index) const;

   DebugClientCodeDebugger GetCodeDebuggerInterface(unsigned int codeDebuggerId);


   // misc.

   CImageList GetObjectImageList();

   bool GetMessage(DebugClientMessage& msg);

private:
   /// pointer to debug server interface
   IDebugServer* m_debugInterface;

   /// current level we're operating
   unsigned int m_level;

   /// array with all valid code debugger IDs
   std::vector<unsigned int> m_codeDebuggerIdList;
};
