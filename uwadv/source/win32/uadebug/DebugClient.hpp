//
// Underworld Adventures Debugger - a debugger tool for Underworld Adventures
// Copyright (c) 2004,2005,2019 Michael Fink
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

class ua_debug_server_interface;

/// debug server message that is received by the client
struct CDebugClientMessage
{
   /// ctor
   CDebugClientMessage()
      :m_nType(0), m_nArg1(0), m_nArg2(0), m_dArg3(0.0) {}

   /// message type; see enum ua_debug_server_message_type
   unsigned int m_nType;

   /// message argument 1
   unsigned int m_nArg1;
   /// message argument 2
   unsigned int m_nArg2;
   /// message argument 3
   double m_dArg3;

   /// message text
   CString m_cszText;
};


/// player info interface
class CDebugClientPlayerInterface
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
   CDebugClientPlayerInterface() {}

   /// pointer to debug server interface
   ua_debug_server_interface* m_pDebugInterface;

   friend class CDebugClientInterface;
};


/// object list interface
class CDebugClientObjectInterface
{
public:
   unsigned int GetColumnCount() const;

   LPCTSTR GetColumnName(unsigned int nColumn) const;
   unsigned int GetColumnSize(unsigned int nColumn) const;
   bool ViewColumnAsHex(unsigned int nColumn) const;
   unsigned int ColumnHexDigitCount(unsigned int nColumn) const;

   unsigned int GetItemId(unsigned int nPos);
   unsigned int GetItemNext(unsigned int nPos);

   unsigned int GetItemInfo(unsigned int nPos, unsigned int nSubcode);
   void SetItemInfo(unsigned int nPos, unsigned int nSubcode, unsigned int nInfo);

private:
   CDebugClientObjectInterface() {}

   /// pointer to debug server interface
   ua_debug_server_interface* m_pDebugInterface;

   unsigned int m_nLevel;

   friend class CDebugClientInterface;
};


/// code debugger type; corresponds with ua_debug_code_debugger_type in dbgserver.hpp
enum T_enCodeDebuggerType
{
   cdtUwConv = 0,   ///< uw conversation script
   cdtLuaScript   ///< Lua script
};

/// code debugger state; corresponds with ua_debug_code_debugger_state in dbgserver.hpp
enum T_enCodeDebuggerState
{
   cdsInactive = 0,
   cdsRunning,
   cdsBreak,
};

/// code debugger command; corresponds with ua_debug_code_debugger_command in dbgserver.hpp
enum T_enCodeDebuggerCommand
{
   cdcRun = 0,
   cdcStepOver,
   cdcStepInto,
   cdcStepOut
};


struct SCodePosition
{
   /// index value to retrieve source file name
   unsigned int m_nSourceFileNameIndex;
   unsigned int m_nSourceFileLine;

   /// code position; if not available, -1 is put in here
   unsigned int m_nCodePos;
};

struct SBreakpointInfo
{
   bool m_bIsVisible;
   SCodePosition m_location;
};

struct SCallstackInfo
{
   unsigned int m_nPos;
   unsigned int m_nReturnPos;
};



/// code debugger interface
class CDebugClientCodeDebuggerInterface
{
public:
   virtual ~CDebugClientCodeDebuggerInterface() {}

   // misc.

   /// returns code debugger type
   T_enCodeDebuggerType GetDebuggerType();

   /// prepares debug info for code debugger
   void PrepareDebugInfo();

   /// returns true when source file is available
   bool IsSourceAvail() const;

   /// returns true when code is available
   bool IsCodeAvail() const;

   /// sets new debugger command
   void SetCommand(T_enCodeDebuggerCommand enCommand);

   /// returns current code debugger state
   T_enCodeDebuggerState GetState() const;

   /// returns current code position
   SCodePosition GetCurrentPos();

   // breakpoints

   unsigned int GetBreakpointCount() const;
   void GetBreakpointInfo(unsigned int nBreakpointIndex, SBreakpointInfo& breakpointInfo) const;

   // call stack

   unsigned int GetCallstackHeight() const;
   void GetCallstackInfo(unsigned int nAtLevel, SCallstackInfo& callstackInfo) const;

   // sourcecode files

   // sourcecode files count; only valid if IsSourceAvail() returns a value > 0
   unsigned int GetSourcefileCount() const;

   /// returns sourcecode filename by index
   CString GetSourcefileFilename(unsigned int nIndex);

   /// returns sourcecode filename and line by code position
   bool GetSourceFromCodePos(unsigned int nCodePos, CString& cszFilename, unsigned int& nLine, unsigned int& nLineDisplacement);

protected:
   /// ctor
   CDebugClientCodeDebuggerInterface() {}

   /// pointer to code debugger interface
   class ua_debug_code_interface* m_pCodeDebugger;

   /// pointer to debug interface
   class CDebugClientInterface* m_pDebugClient;

   friend class CDebugClientInterface;
};


enum T_enTileInfoType
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
class CDebugClientInterface
{
public:
   CDebugClientInterface() {}
   virtual ~CDebugClientInterface() {}

   bool Init(ua_debug_server_interface* pDebugInterface);

   void Lock(bool bLock);

   bool IsStudioMode();
   unsigned int GetFlag(unsigned int flag);
   CString GetGameCfgPath();

   void LoadGameCfg(LPCTSTR pszPrefix);

   bool IsGamePaused();
   void PauseGame(bool pause);

   // level stuff

   unsigned int GetNumLevels();
   void SetWorkingLevel(unsigned int level);
   unsigned int GetWorkingLevel() const { return m_nLevel; }

   CString GetLevelName(unsigned int level) const;

   void InsertNewLevel(unsigned int before_level);

   void MoveLevel(unsigned int level, unsigned int level_insert_point);


   // sub-interfaces

   CDebugClientPlayerInterface GetPlayerInterface();

   CDebugClientObjectInterface GetObjectInterface();


   // tile info

   unsigned int GetTileInfo(unsigned int xpos, unsigned int ypos, T_enTileInfoType type);
   void SetTileInfo(unsigned int xpos, unsigned int ypos, T_enTileInfoType type, unsigned int val);


   // game strings

   bool EnumGameStringsBlock(int index, unsigned int& block);
   unsigned int GetGameStringBlockSize(unsigned int block);
   CString GetGameString(unsigned int block, unsigned int nr);


   // code debugger access

   void AddCodeDebugger(unsigned int nCodeDebuggerID);
   void RemoveCodeDebugger(unsigned int nCodeDebuggerID);

   bool IsValidCodeDebuggerID(unsigned int nCodeDebuggerID) const;

   unsigned int GetCodeDebuggerCount() const;
   unsigned int GetCodeDebuggerByIndex(unsigned int nIndex) const;

   CDebugClientCodeDebuggerInterface GetCodeDebuggerInterface(unsigned int nCodeDebuggerID);


   // misc.

   CImageList GetObjectImageList();

   bool GetMessage(CDebugClientMessage& msg);

private:
   /// pointer to debug server interface
   ua_debug_server_interface* m_pDebugInterface;

   /// current level we're operating
   unsigned int m_nLevel;

   /// array with all valid code debugger IDs
   CSimpleArray<unsigned int> m_anCodeDebuggerIDs;
};
