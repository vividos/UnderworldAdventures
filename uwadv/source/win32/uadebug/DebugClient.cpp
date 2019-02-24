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
/// \file DebugClient.cpp
/// \brief debugger client class
//
#include "stdatl.hpp"
#include "DebugClient.hpp"

LPCTSTR g_aszAttrNames[] =
{
   _T("xpos"),
   _T("ypos"),
   _T("zpos"),
   _T("angle"),

   // attributes
   _T("gender"),
   _T("handedness"),
   _T("appearance"),
   _T("profession"),
   _T("maplevel"),
   _T("strength"),
   _T("dexterity"),
   _T("intelligence"),
   _T("life"),
   _T("max_life"),
   _T("mana"),
   _T("max_mana"),
   _T("weariness"),
   _T("hungriness"),
   _T("poisoned"),
   _T("mentalstate"),
   _T("nightvision"),
   _T("talks"),
   _T("kills"),
   _T("level"),
   _T("exp_points"),
   _T("difficulty"),

   // skills
   _T("attack"),
   _T("defense"),
   _T("unarmed"),
   _T("sword"),
   _T("axe"),
   _T("mace"),
   _T("missile"),
   _T("mana"),
   _T("lore"),
   _T("casting"),
   _T("traps"),
   _T("search"),
   _T("track"),
   _T("sneak"),
   _T("repair"),
   _T("charm"),
   _T("picklock"),
   _T("acrobat"),
   _T("appraise"),
   _T("swimming")
};


/// pos and name don't count as columns in CDebugClient
const int g_nObjListColumns = 15 - 2;

struct SObjectListColumnInfo
{
   LPCTSTR pszColumnName;
   int nColumnSize;
   bool bFormatHex;
   unsigned nFormatHexLength;
} g_aColumnInfo[] =
{
   { _T("pos"),      70,   true,    4 },
   { _T("item_id"),  55,   true,    4 },
   { _T("name"),     170,  false,   0 },
   { _T("link"),     55,   true,    4 },
   { _T("quality"),  50,   true,    3 },
   { _T("owner"),    50,   true,    2 },
   { _T("quantity"), 55,   true,    3 },
   { _T("x"),        35,   false,   1 },
   { _T("y"),        35,   false,   1 },
   { _T("z"),        35,   false,   1 },
   { _T("heading"),  50,   false,   1 },
   { _T("flags"),    40,   true,    1 },
   { _T("ench"),     40,   true,    1 },
   { _T("is_quant"), 55,   false,   1 },
   { _T("hidden"),   55,   false,   1 },
};

unsigned int CDebugClientPlayerInterface::GetAttrCount()
{
   return sizeof(g_aszAttrNames) / sizeof(g_aszAttrNames[0]) - 4;
}

LPCTSTR CDebugClientPlayerInterface::GetPosInfoName(unsigned int info)
{
   ATLASSERT(info < 4);
   return g_aszAttrNames[info];
}

LPCTSTR CDebugClientPlayerInterface::GetAttrName(unsigned int attr)
{
   ATLASSERT(attr < GetAttrCount());
   return g_aszAttrNames[attr + 4];
}

unsigned int CDebugClientPlayerInterface::GetAttribute(unsigned int attr)
{
   ATLASSERT(attr < GetAttrCount());
   return m_pDebugInterface->get_player_attr(attr);
}

double CDebugClientPlayerInterface::GetPosInfo(unsigned int info)
{
   ATLASSERT(info < 4);
   return m_pDebugInterface->get_player_pos_info(info);
}

void CDebugClientPlayerInterface::SetAttribute(unsigned int attr, unsigned int val)
{
   ATLASSERT(attr < GetAttrCount());
   m_pDebugInterface->set_player_attr(attr, val);
}

void CDebugClientPlayerInterface::SetPosInfo(unsigned int info, double val)
{
   ATLASSERT(info < 4);
   m_pDebugInterface->set_player_pos_info(info, val);
}

void CDebugClientPlayerInterface::Teleport(unsigned int level, double xpos, double ypos)
{
   m_pDebugInterface->set_player_attr(4, level);
   m_pDebugInterface->set_player_pos_info(0, xpos);
   m_pDebugInterface->set_player_pos_info(1, ypos);

   double height = m_pDebugInterface->get_tile_height(level, xpos, ypos);
   m_pDebugInterface->set_player_pos_info(2, height);
}


// CDebugClientObjectInterface methods

unsigned int CDebugClientObjectInterface::GetColumnCount() const
{
   return sizeof(g_aColumnInfo) / sizeof(g_aColumnInfo[0]);
}

LPCTSTR CDebugClientObjectInterface::GetColumnName(unsigned int nColumn) const
{
   ATLASSERT(nColumn < GetColumnCount());
   return g_aColumnInfo[nColumn].pszColumnName;
}

unsigned int CDebugClientObjectInterface::GetColumnSize(unsigned int nColumn) const
{
   ATLASSERT(nColumn < GetColumnCount());
   return g_aColumnInfo[nColumn].nColumnSize;
}

bool CDebugClientObjectInterface::ViewColumnAsHex(unsigned int nColumn) const
{
   ATLASSERT(nColumn < GetColumnCount());
   return g_aColumnInfo[nColumn].bFormatHex;
}

unsigned int CDebugClientObjectInterface::ColumnHexDigitCount(unsigned int nColumn) const
{
   ATLASSERT(nColumn < GetColumnCount());
   return g_aColumnInfo[nColumn].nFormatHexLength;
}

unsigned int CDebugClientObjectInterface::GetItemId(unsigned int nPos)
{
   ATLASSERT(nPos < 0x400);
   return GetItemInfo(nPos, 1); // 1 currently is the item_id field
}

unsigned int CDebugClientObjectInterface::GetItemNext(unsigned int nPos)
{
   ATLASSERT(nPos < 0x400);
   return GetItemInfo(nPos, 3); // 3 currently is the next field
}

unsigned int CDebugClientObjectInterface::GetItemInfo(unsigned int nPos, unsigned int nSubcode)
{
   ATLASSERT(nPos < 0x400);
   ATLASSERT(nSubcode != 2); // can't get name

   if (nSubcode == 0)
      return nPos;

   // remap to fields
   nSubcode = nSubcode == 1 ? 0 : nSubcode - 2;

   return m_pDebugInterface->get_objlist_info(m_nLevel, nPos, nSubcode);
}

void CDebugClientObjectInterface::SetItemInfo(unsigned int nPos, unsigned int nSubcode, unsigned int nInfo)
{
   ATLASSERT(nPos < 0x400);
   ATLASSERT(nSubcode != 0 && nSubcode != 2); // can't set pos and name

   // remap to fields
   nSubcode = nSubcode == 1 ? 0 : nSubcode - 2;

   m_pDebugInterface->set_objlist_info(m_nLevel, nPos, nSubcode, nInfo);
}

T_enCodeDebuggerType CDebugClientCodeDebuggerInterface::GetDebuggerType()
{
   return static_cast<T_enCodeDebuggerType>(m_pCodeDebugger->get_debugger_type());
}

void CDebugClientCodeDebuggerInterface::PrepareDebugInfo()
{
   m_pCodeDebugger->prepare_debug_info();
}

bool CDebugClientCodeDebuggerInterface::IsSourceAvail() const
{
   // TODO implement
   return true;
}

bool CDebugClientCodeDebuggerInterface::IsCodeAvail() const
{
   // TODO implement
   return true;
}

void CDebugClientCodeDebuggerInterface::SetCommand(T_enCodeDebuggerCommand enCommand)
{
   m_pCodeDebugger->set_debugger_command(static_cast<ua_debug_code_debugger_command>(enCommand));
   m_pCodeDebugger->set_debugger_state(ua_debugger_state_running);
}

T_enCodeDebuggerState CDebugClientCodeDebuggerInterface::GetState() const
{
   return static_cast<T_enCodeDebuggerState>(m_pCodeDebugger->get_debugger_state());
}

SCodePosition CDebugClientCodeDebuggerInterface::GetCurrentPos()
{
   SCodePosition pos;

   bool bSourcefileIsValid = false;

   m_pCodeDebugger->get_current_pos(pos.m_nSourceFileNameIndex,
      pos.m_nSourceFileLine, pos.m_nCodePos, bSourcefileIsValid);

   if (bSourcefileIsValid)
      pos.m_nCodePos = unsigned(-1);
   else
      pos.m_nSourceFileNameIndex = unsigned(-1);

   return pos;
}

unsigned int CDebugClientCodeDebuggerInterface::GetBreakpointCount() const
{
   return m_pCodeDebugger->get_num_breakpoints();
}

void CDebugClientCodeDebuggerInterface::GetBreakpointInfo(unsigned int nBreakpointIndex, SBreakpointInfo& breakpointInfo) const
{
   ATLASSERT(nBreakpointIndex < GetBreakpointCount());
   m_pCodeDebugger->get_breakpoint_info(nBreakpointIndex,
      breakpointInfo.m_location.m_nSourceFileNameIndex,
      breakpointInfo.m_location.m_nSourceFileLine,
      breakpointInfo.m_location.m_nCodePos,
      breakpointInfo.m_bIsVisible);
}

unsigned int CDebugClientCodeDebuggerInterface::GetCallstackHeight() const
{
   // TODO implement
   return 0;
}

void CDebugClientCodeDebuggerInterface::GetCallstackInfo(unsigned int nAtLevel, SCallstackInfo& callstackInfo) const
{
   ATLASSERT(nAtLevel < GetCallstackHeight());
   nAtLevel;
   callstackInfo;
   // TODO implement
}

unsigned int CDebugClientCodeDebuggerInterface::GetSourcefileCount() const
{
   return m_pCodeDebugger->get_num_sourcefiles();
}

CString CDebugClientCodeDebuggerInterface::GetSourcefileFilename(unsigned int nIndex)
{
   ATLASSERT(nIndex < GetSourcefileCount());

   unsigned int nSize = m_pCodeDebugger->get_sourcefile_name(nIndex, NULL, 0);

   CHAR* szText = new CHAR[nSize + 1];
   szText[nSize] = 0;

   m_pCodeDebugger->get_sourcefile_name(nIndex, szText, nSize + 1);

   USES_CONVERSION;
   CFilename sourceFilename(A2CT(szText));
   delete[] szText;

   if (sourceFilename.IsRelativePath())
      sourceFilename.MakeAbsoluteToCurrentDir();

   ATLASSERT(sourceFilename.IsValidObject());

   return sourceFilename.Get();
}

bool CDebugClientCodeDebuggerInterface::GetSourceFromCodePos(unsigned int nCodePos,
   CString& cszFilename, unsigned int& nLine, unsigned int& nLineDisplacement)
{
   unsigned int nSourcefileIndex = 0; // TODO
   cszFilename = GetSourcefileFilename(nSourcefileIndex);
   nCodePos;
   nLine;
   nLineDisplacement;

   // TODO implement
   return false;
}


// CDebugClientInterface methods

bool CDebugClientInterface::Init(ua_debug_server_interface* pDebugInterface)
{
   m_pDebugInterface = pDebugInterface;

   // check interface
   if (!m_pDebugInterface->check_interface_version())
      return false;

   // get level
   m_nLevel = m_pDebugInterface->get_player_attr(4);

   return true;
}

void CDebugClientInterface::Lock(bool bLock)
{
   m_pDebugInterface->lock(bLock);
}

bool CDebugClientInterface::IsStudioMode()
{
   return 1 == m_pDebugInterface->get_flag(ua_flag_is_studio_mode);
}

unsigned int CDebugClientInterface::GetFlag(unsigned int flag)
{
   return m_pDebugInterface->get_flag(flag);
}

CString CDebugClientInterface::GetGameCfgPath()
{
   CHAR szBuffer[512];
   szBuffer[511] = 0;

   m_pDebugInterface->get_game_path(szBuffer, 511);

   USES_CONVERSION;
   CFilename gamePath(A2CT(szBuffer));

   gamePath.MakeAbsoluteToCurrentDir();
   ATLASSERT(gamePath.IsValidObject());

   return gamePath.Get();
}

void CDebugClientInterface::LoadGameCfg(LPCTSTR pszPrefix)
{
   m_pDebugInterface->load_game(pszPrefix);
}

bool CDebugClientInterface::IsGamePaused()
{
   m_pDebugInterface->lock(true);
   bool paused = m_pDebugInterface->pause_game(true);
   m_pDebugInterface->pause_game(paused);
   m_pDebugInterface->lock(false);

   return paused;
}

void CDebugClientInterface::PauseGame(bool pause)
{
   m_pDebugInterface->lock(true);
   m_pDebugInterface->pause_game(pause);
   m_pDebugInterface->lock(false);
}

unsigned int CDebugClientInterface::GetNumLevels()
{
   return m_pDebugInterface->GetNumLevels();
}

void CDebugClientInterface::SetWorkingLevel(unsigned int level)
{
   m_nLevel = level;
}

CString CDebugClientInterface::GetLevelName(unsigned int level) const
{
   level;
   // TODO
   return _T("unknown");
}

void CDebugClientInterface::InsertNewLevel(unsigned int before_level)
{
   before_level;
   // TODO implement
}

void CDebugClientInterface::MoveLevel(unsigned int level, unsigned int level_insert_point)
{
   level;
   level_insert_point;
   // TODO implement
}

CDebugClientPlayerInterface CDebugClientInterface::GetPlayerInterface()
{
   CDebugClientPlayerInterface playerInterface;
   playerInterface.m_pDebugInterface = m_pDebugInterface;
   return playerInterface;
}

CDebugClientObjectInterface CDebugClientInterface::GetObjectInterface()
{
   CDebugClientObjectInterface objectInterface;
   objectInterface.m_pDebugInterface = m_pDebugInterface;
   objectInterface.m_nLevel = m_nLevel;
   return objectInterface;
}


unsigned int CDebugClientInterface::GetTileInfo(unsigned int xpos, unsigned int ypos, T_enTileInfoType type)
{
   return m_pDebugInterface->get_tile_info_value(m_nLevel, xpos, ypos, static_cast<unsigned int>(type));
}

void CDebugClientInterface::SetTileInfo(unsigned int xpos, unsigned int ypos, T_enTileInfoType type, unsigned int val)
{
   m_pDebugInterface->set_tile_info_value(m_nLevel, xpos, ypos, static_cast<unsigned int>(type), val);
}

bool CDebugClientInterface::EnumGameStringsBlock(int index, unsigned int& block)
{
   return m_pDebugInterface->enum_gamestr_block(index, block);
}

unsigned int CDebugClientInterface::GetGameStringBlockSize(unsigned int block)
{
   return m_pDebugInterface->get_gamestr_blocksize(block);
}

CString CDebugClientInterface::GetGameString(unsigned int block, unsigned int nr)
{
   unsigned int nLen = m_pDebugInterface->get_game_string(block, nr, NULL, 0);

   CHAR* szBuffer = new CHAR[nLen + 1];
   szBuffer[nLen] = 0;

   m_pDebugInterface->get_game_string(block, nr, szBuffer, nLen + 1);

   USES_CONVERSION;
   CString cszText = A2CT(szBuffer);
   delete[] szBuffer;

   return cszText;
}

void CDebugClientInterface::AddCodeDebugger(unsigned int nCodeDebuggerID)
{
   if (!IsValidCodeDebuggerID(nCodeDebuggerID))
      m_anCodeDebuggerIDs.Add(nCodeDebuggerID);
}

void CDebugClientInterface::RemoveCodeDebugger(unsigned int nCodeDebuggerID)
{
   ATLVERIFY(TRUE == m_anCodeDebuggerIDs.Remove(nCodeDebuggerID));
}

unsigned int CDebugClientInterface::GetCodeDebuggerCount() const
{
   return static_cast<unsigned int>(m_anCodeDebuggerIDs.GetSize());
}

unsigned int CDebugClientInterface::GetCodeDebuggerByIndex(unsigned int nIndex) const
{
   ATLASSERT(nIndex < GetCodeDebuggerCount());
   return m_anCodeDebuggerIDs[nIndex];
}

bool CDebugClientInterface::IsValidCodeDebuggerID(unsigned int nCodeDebuggerID) const
{
   int nMax = m_anCodeDebuggerIDs.GetSize();
   for (int n = 0; n < nMax; n++)
   {
      if (m_anCodeDebuggerIDs[n] == nCodeDebuggerID)
         return true;
   }

   return false;
}

CDebugClientCodeDebuggerInterface CDebugClientInterface::GetCodeDebuggerInterface(unsigned int nCodeDebuggerID)
{
   ATLASSERT(true == IsValidCodeDebuggerID(nCodeDebuggerID));

   CDebugClientCodeDebuggerInterface cdi;
   cdi.m_pCodeDebugger = m_pDebugInterface->get_code_debugger(nCodeDebuggerID);
   ATLASSERT(cdi.m_pCodeDebugger != NULL);

   cdi.m_pDebugClient = this;

   return cdi;
}

CImageList CDebugClientInterface::GetObjectImageList()
{
   unsigned int num_objects = 0;
   m_pDebugInterface->get_object_list_imagelist(num_objects, NULL, 0);

   BYTE* pBitmapData = new BYTE[num_objects * 16 * 16 * 4];

   m_pDebugInterface->get_object_list_imagelist(num_objects, pBitmapData, num_objects * 16 * 16 * 4);

   // convert from RGBA to BGRA; CImageList needs this
   for (unsigned int i = 0; i < num_objects * 16 * 16; i++)
   {
      BYTE nSwap = pBitmapData[i * 4 + 0];
      pBitmapData[i * 4 + 0] = pBitmapData[i * 4 + 2];
      pBitmapData[i * 4 + 2] = nSwap;
   };

   CImageList imageList;
   imageList.Create(16, 16, ILC_COLOR32, 0, num_objects);

   for (unsigned int n = 0; n < num_objects; n++)
   {
      BYTE buffer[16 * 16 * 4];

      memcpy(buffer, &pBitmapData[n * 16 * 16 * 4], 16 * 16 * 4);

      CBitmap bitmap;
      bitmap.CreateBitmap(16, 16, 1, 32, buffer);
      imageList.Add(bitmap, RGB(255, 255, 255));
   }

   delete[] pBitmapData;

   return imageList;
}

bool CDebugClientInterface::GetMessage(CDebugClientMessage& msg)
{
   m_pDebugInterface->lock(true);

   unsigned int nNum = m_pDebugInterface->get_message_num();
   if (nNum == 0)
   {
      m_pDebugInterface->lock(false);
      return false;
   }

   unsigned int nTextSize = 0;
   m_pDebugInterface->get_message(msg.m_nType, msg.m_nArg1, msg.m_nArg2, msg.m_dArg3, nTextSize);

   CHAR* szBuffer = new CHAR[nTextSize + 1];
   szBuffer[nTextSize] = 0;

   ATLVERIFY(true == m_pDebugInterface->get_message_text(szBuffer, nTextSize));

   msg.m_cszText = szBuffer;
   delete[] szBuffer;

   m_pDebugInterface->pop_message();

   m_pDebugInterface->lock(false);

   return true;
}
