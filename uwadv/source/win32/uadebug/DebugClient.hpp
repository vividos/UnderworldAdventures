/*
   Underworld Adventures Debugger - a debugger tool for Underworld Adventures
   Copyright (c) 2004,2005 Michael Fink

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
/*! \file DebugClient.hpp

   \brief debugger client class

*/
//! \ingroup uadebug

//@{

// include guard
#pragma once

// includes

// forward references
class ua_debug_server_interface;

// classes

//! debug server message that is received by the client
struct CDebugClientMessage
{
   //! ctor
   CDebugClientMessage()
      :msg_type(0), msg_arg1(0), msg_arg2(0), msg_arg3(0.0){}

   //! message type; see enum ua_debug_server_message_type
   unsigned int msg_type;

   //! message argument 1
   unsigned int msg_arg1;
   //! message argument 2
   unsigned int msg_arg2;
   //! message argument 3
   double msg_arg3;

   //! message text
   //CString msg_text;
};


class CDebugClientPlayerInterface
{
public:
   unsigned int GetAttrCount();
   LPCTSTR GetPosInfoName(unsigned int info);
   LPCTSTR GetAttrName(unsigned int attr);

   double GetPosInfo(unsigned int info);
   unsigned int GetAttribute(unsigned int attr);

   //! retrieves positional info; info can be 0..3
   void SetPosInfo(unsigned int info, double val);

   //! retrieves attributes info; attr can be 0..3
   void SetAttribute(unsigned int attr, unsigned int val);

   void Teleport(unsigned int level, double xpos, double ypos);

private:
   //! pointer to debug server interface
   ua_debug_server_interface* m_pDebugInterface;

   friend class CDebugClientInterface;
};


//! debugger client interface
class CDebugClientInterface  
{
public:
   CDebugClientInterface(){}
   virtual ~CDebugClientInterface(){}

   bool Init(ua_debug_server_interface* pDebugInterface);

   void Lock(bool bLock);

   bool IsStudioMode();
   unsigned int GetFlag(unsigned int flag);
   CString GetGameCfgPath();

   void LoadGameCfg(LPCTSTR pszPrefix);

   void PauseGame(bool pause);

   unsigned int GetNumLevels();
   void SetWorkingLevel(unsigned int level);
   unsigned int GetWorkingLevel() const { return m_nLevel; }


   CDebugClientPlayerInterface GetPlayerInterface();


   unsigned int GetObjectListInfo(unsigned int pos, unsigned int subcode);
   void SetObjectListInfo(unsigned int pos, unsigned int subcode, unsigned int info);

   unsigned int GetTileInfo(unsigned int xpos, unsigned int ypos, unsigned int type);
   void SetTileInfo(unsigned int xpos, unsigned int ypos, unsigned int type, unsigned int val);


   bool EnumGameStringsBlock(int index, unsigned int& block);
   unsigned int GetGameStringBlockSize(unsigned int block);
   CString GetGameString(unsigned int block, unsigned int nr);

//   bool GetMessage(CDebugClientMessage& msg);

private:
   //! pointer to debug server interface
   ua_debug_server_interface* m_pDebugInterface;

   //! current level we're operating
   unsigned int m_nLevel;
};

//@}
