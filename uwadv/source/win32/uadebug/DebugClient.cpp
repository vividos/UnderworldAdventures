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
/// \file DebugClient.cpp
/// \brief debugger client class
//
#include "stdatl.hpp"
#include "DebugClient.hpp"

LPCTSTR g_attributeNames[] =
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


/// pos and name don't count as columns in DebugClient
const int c_objectListColumns = 15 - 2;

struct ObjectListColumnInfo
{
   LPCTSTR columnName;
   int columnSize;
   bool formatAsHex;
   unsigned formatHexLength;
} g_columnInfo[] =
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

unsigned int DebugClientPlayerInterface::GetAttrCount()
{
   return sizeof(g_attributeNames) / sizeof(g_attributeNames[0]) - 4;
}

LPCTSTR DebugClientPlayerInterface::GetPosInfoName(unsigned int info)
{
   ATLASSERT(info < 4);
   return g_attributeNames[info];
}

LPCTSTR DebugClientPlayerInterface::GetAttrName(unsigned int attr)
{
   ATLASSERT(attr < GetAttrCount());
   return g_attributeNames[attr + 4];
}

unsigned int DebugClientPlayerInterface::GetAttribute(unsigned int attr)
{
   ATLASSERT(attr < GetAttrCount());
   return m_debugInterface->GetPlayerAttribute(attr);
}

double DebugClientPlayerInterface::GetPosInfo(unsigned int info)
{
   ATLASSERT(info < 4);
   return m_debugInterface->GetPlayerPosInfo(info);
}

void DebugClientPlayerInterface::SetAttribute(unsigned int attr, unsigned int val)
{
   ATLASSERT(attr < GetAttrCount());
   m_debugInterface->SetPlayerAttribute(attr, val);
}

void DebugClientPlayerInterface::SetPosInfo(unsigned int info, double val)
{
   ATLASSERT(info < 4);
   m_debugInterface->SetPlayerPosInfo(info, val);
}

void DebugClientPlayerInterface::Teleport(unsigned int level, double xpos, double ypos)
{
   m_debugInterface->SetPlayerAttribute(4, level);
   m_debugInterface->SetPlayerPosInfo(0, xpos);
   m_debugInterface->SetPlayerPosInfo(1, ypos);

   double height = m_debugInterface->GetTileHeight(level, xpos, ypos);
   m_debugInterface->SetPlayerPosInfo(2, height);
}


// DebugClientObjectListInterface methods

unsigned int DebugClientObjectListInterface::GetColumnCount() const
{
   return sizeof(g_columnInfo) / sizeof(g_columnInfo[0]);
}

LPCTSTR DebugClientObjectListInterface::GetColumnName(unsigned int columnIndex) const
{
   ATLASSERT(columnIndex < GetColumnCount());
   return g_columnInfo[columnIndex].columnName;
}

unsigned int DebugClientObjectListInterface::GetColumnSize(unsigned int columnIndex) const
{
   ATLASSERT(columnIndex < GetColumnCount());
   return g_columnInfo[columnIndex].columnSize;
}

bool DebugClientObjectListInterface::ViewColumnAsHex(unsigned int columnIndex) const
{
   ATLASSERT(columnIndex < GetColumnCount());
   return g_columnInfo[columnIndex].formatAsHex;
}

unsigned int DebugClientObjectListInterface::ColumnHexDigitCount(unsigned int columnIndex) const
{
   ATLASSERT(columnIndex < GetColumnCount());
   return g_columnInfo[columnIndex].formatHexLength;
}

unsigned int DebugClientObjectListInterface::GetItemId(unsigned int pos)
{
   ATLASSERT(pos < 0x400);
   return GetItemInfo(pos, 1); // 1 currently is the item_id field
}

unsigned int DebugClientObjectListInterface::GetItemNext(unsigned int pos)
{
   ATLASSERT(pos < 0x400);
   return GetItemInfo(pos, 3); // 3 currently is the next field
}

bool DebugClientObjectListInterface::IsItemInfoAvail(unsigned int pos) const
{
   return m_debugInterface->IsObjectListIndexAvail(m_level, pos);
}

unsigned int DebugClientObjectListInterface::GetItemInfo(unsigned int pos, unsigned int subcode)
{
   ATLASSERT(pos < 0x400);
   ATLASSERT(subcode != 2); // can't get name

   if (subcode == 0)
      return pos;

   if (pos == 0)
      return 0;

   // remap to fields
   subcode = subcode == 1 ? 0 : subcode - 2;

   return m_debugInterface->GetObjectListInfo(m_level, pos, subcode);
}

void DebugClientObjectListInterface::SetItemInfo(unsigned int pos, unsigned int subcode, unsigned int nInfo)
{
   ATLASSERT(pos < 0x400);
   ATLASSERT(subcode != 0 && subcode != 2); // can't set pos and name

   // remap to fields
   subcode = subcode == 1 ? 0 : subcode - 2;

   m_debugInterface->SetObjectListInfo(m_level, pos, subcode, nInfo);
}

CodeDebuggerType IDebugClientCodeDebugger::GetDebuggerType()
{
   return static_cast<CodeDebuggerType>(m_codeDebugger->GetDebuggerType());
}

void IDebugClientCodeDebugger::PrepareDebugInfo()
{
   m_codeDebugger->PrepareDebugInfo();
}

bool IDebugClientCodeDebugger::IsSourceAvail() const
{
   // TODO implement
   return true;
}

bool IDebugClientCodeDebugger::IsCodeAvail() const
{
   // TODO implement
   return true;
}

void IDebugClientCodeDebugger::SetCommand(CodeDebuggerCommand command)
{
   m_codeDebugger->SetDebuggerCommand(static_cast<DebugServerCodeDebuggerCommand>(command));
   m_codeDebugger->SetDebuggerState(codeDebuggerStateRunning);
}

CodeDebuggerState IDebugClientCodeDebugger::GetState() const
{
   return static_cast<CodeDebuggerState>(m_codeDebugger->GetDebuggerState());
}

CodePosition IDebugClientCodeDebugger::GetCurrentPos()
{
   CodePosition pos;

   bool bSourcefileIsValid = false;

   m_codeDebugger->GetCurrentPos(pos.m_sourceFileNameIndex,
      pos.m_sourceFileLine, pos.m_codePosition, bSourcefileIsValid);

   if (bSourcefileIsValid)
      pos.m_codePosition = unsigned(-1);
   else
      pos.m_sourceFileNameIndex = unsigned(-1);

   return pos;
}

unsigned int IDebugClientCodeDebugger::GetBreakpointCount() const
{
   return m_codeDebugger->GetNumBreakpoints();
}

void IDebugClientCodeDebugger::GetBreakpointInfo(unsigned int nBreakpointIndex, BreakpointInfo& breakpointInfo) const
{
   ATLASSERT(nBreakpointIndex < GetBreakpointCount());
   m_codeDebugger->GetBreakpointInfo(nBreakpointIndex,
      breakpointInfo.m_location.m_sourceFileNameIndex,
      breakpointInfo.m_location.m_sourceFileLine,
      breakpointInfo.m_location.m_codePosition,
      breakpointInfo.m_isVisible);
}

unsigned int IDebugClientCodeDebugger::GetCallstackHeight() const
{
   // TODO implement
   return 0;
}

void IDebugClientCodeDebugger::GetCallstackInfo(unsigned int atLevel, CallstackInfo& callstackInfo) const
{
   ATLASSERT(atLevel < GetCallstackHeight());
   UNUSED(atLevel);
   UNUSED(callstackInfo);
   // TODO implement
}

unsigned int IDebugClientCodeDebugger::GetSourceFileCount() const
{
   return m_codeDebugger->GetNumSourcefiles();
}

CString IDebugClientCodeDebugger::GetSourceFileName(unsigned int index)
{
   ATLASSERT(index < GetSourceFileCount());

   unsigned int nSize = m_codeDebugger->GetSourcefileName(index, NULL, 0);

   CHAR* szText = new CHAR[nSize + 1];
   szText[nSize] = 0;

   m_codeDebugger->GetSourcefileName(index, szText, nSize + 1);

   USES_CONVERSION;
   CFilename sourceFilename(A2CT(szText));
   delete[] szText;

   if (sourceFilename.IsRelativePath())
      sourceFilename.MakeAbsoluteToCurrentDir();

   ATLASSERT(sourceFilename.IsValidObject());

   return sourceFilename.Get();
}

bool IDebugClientCodeDebugger::GetSourceFromCodePos(unsigned int codePos,
   CString& filename, unsigned int& lineNumber, unsigned int& lineDisplacement)
{
   unsigned int sourcefileIndex = 0; // TODO
   filename = GetSourceFileName(sourcefileIndex);
   UNUSED(codePos);
   UNUSED(lineNumber);
   UNUSED(lineDisplacement);

   // TODO implement
   return false;
}


// DebugClient methods

bool DebugClient::Init(IDebugServer* pDebugInterface)
{
   m_debugInterface = pDebugInterface;

   // check interface
   if (!m_debugInterface->CheckInterfaceVersion())
      return false;

   // get level
   m_level = m_debugInterface->GetPlayerAttribute(4);

   return true;
}

void DebugClient::Lock(bool locked)
{
   m_debugInterface->Lock(locked);
}

bool DebugClient::IsStudioMode()
{
   return 1 == m_debugInterface->GetFlag(debugServerFlagIsStudioMode);
}

unsigned int DebugClient::GetFlag(unsigned int flag)
{
   return m_debugInterface->GetFlag(flag);
}

CString DebugClient::GetGameConfigPath()
{
   CHAR szBuffer[512];
   szBuffer[511] = 0;

   m_debugInterface->GetGamePath(szBuffer, 511);

   USES_CONVERSION;
   CFilename gamePath(A2CT(szBuffer));

   gamePath.MakeAbsoluteToCurrentDir();
   ATLASSERT(gamePath.IsValidObject());

   return gamePath.Get();
}

void DebugClient::LoadGameConfig(LPCTSTR pszPrefix)
{
   m_debugInterface->LoadGame(pszPrefix);
}

bool DebugClient::IsGamePaused()
{
   m_debugInterface->Lock(true);
   bool paused = m_debugInterface->PauseGame(true);
   m_debugInterface->PauseGame(paused);
   m_debugInterface->Lock(false);

   return paused;
}

void DebugClient::PauseGame(bool pause)
{
   m_debugInterface->Lock(true);
   m_debugInterface->PauseGame(pause);
   m_debugInterface->Lock(false);
}

unsigned int DebugClient::GetNumLevels()
{
   return m_debugInterface->GetNumLevels();
}

void DebugClient::SetWorkingLevel(unsigned int level)
{
   m_level = level;
}

CString DebugClient::GetLevelName(unsigned int level) const
{
   switch (level)
   {
   case 0: return _T("Goblins");
   case 1: return _T("Mountainmen");
   case 2: return _T("Lizardmen");
   case 3: return _T("Knights");
   case 4: return _T("Ghouls");
   case 5: return _T("Mages");
   case 6: return _T("Tyball");
   case 7: return _T("Volcano");
   case 8: return _T("Moogate");
   default:
      ATLASSERT(false);
      break;
   }

   return _T("unknown");
}

void DebugClient::InsertNewLevel(unsigned int beforeLevel)
{
   UNUSED(beforeLevel);
   // TODO implement
}

void DebugClient::MoveLevel(unsigned int level, unsigned int level_insert_point)
{
   UNUSED(level);
   UNUSED(level_insert_point);
   // TODO implement
}

DebugClientPlayerInterface DebugClient::GetPlayerInterface()
{
   DebugClientPlayerInterface playerInterface;
   playerInterface.m_debugInterface = m_debugInterface;
   return playerInterface;
}

DebugClientObjectListInterface DebugClient::GetObjectInterface()
{
   DebugClientObjectListInterface objectInterface;
   objectInterface.m_debugInterface = m_debugInterface;
   objectInterface.m_level = m_level;
   return objectInterface;
}


unsigned int DebugClient::GetTileInfo(unsigned int xpos, unsigned int ypos, TileInfoType type)
{
   return m_debugInterface->GetTileInfoValue(m_level, xpos, ypos, static_cast<unsigned int>(type));
}

void DebugClient::SetTileInfo(unsigned int xpos, unsigned int ypos, TileInfoType type, unsigned int val)
{
   m_debugInterface->SetTileInfoValue(m_level, xpos, ypos, static_cast<unsigned int>(type), val);
}

bool DebugClient::EnumGameStringsBlock(int index, unsigned int& block)
{
   return m_debugInterface->EnumGameStringsBlocks(index, block);
}

unsigned int DebugClient::GetGameStringBlockSize(unsigned int block)
{
   return m_debugInterface->GetGameStringsBlockSize(block);
}

CString DebugClient::GetGameString(unsigned int block, unsigned int nr)
{
   unsigned int nLen = m_debugInterface->GetGameString(block, nr, NULL, 0);

   CHAR* szBuffer = new CHAR[nLen + 1];
   szBuffer[nLen] = 0;

   m_debugInterface->GetGameString(block, nr, szBuffer, nLen + 1);

   USES_CONVERSION;
   CString cszText = A2CT(szBuffer);
   delete[] szBuffer;

   return cszText;
}

void DebugClient::AddCodeDebugger(unsigned int codeDebuggerId)
{
   if (!IsValidCodeDebuggerId(codeDebuggerId))
      m_codeDebuggerIdList.Add(codeDebuggerId);
}

void DebugClient::RemoveCodeDebugger(unsigned int codeDebuggerId)
{
   ATLVERIFY(TRUE == m_codeDebuggerIdList.Remove(codeDebuggerId));
}

unsigned int DebugClient::GetCodeDebuggerCount() const
{
   return static_cast<unsigned int>(m_codeDebuggerIdList.GetSize());
}

unsigned int DebugClient::GetCodeDebuggerByIndex(unsigned int index) const
{
   ATLASSERT(index < GetCodeDebuggerCount());
   return m_codeDebuggerIdList[index];
}

bool DebugClient::IsValidCodeDebuggerId(unsigned int codeDebuggerId) const
{
   int nMax = m_codeDebuggerIdList.GetSize();
   for (int n = 0; n < nMax; n++)
   {
      if (m_codeDebuggerIdList[n] == codeDebuggerId)
         return true;
   }

   return false;
}

IDebugClientCodeDebugger DebugClient::GetCodeDebuggerInterface(unsigned int codeDebuggerId)
{
   ATLASSERT(true == IsValidCodeDebuggerId(codeDebuggerId));

   IDebugClientCodeDebugger cdi;
   cdi.m_codeDebugger = m_debugInterface->GetCodeDebugger(codeDebuggerId);
   ATLASSERT(cdi.m_codeDebugger != NULL);

   cdi.m_debugClient = this;

   return cdi;
}

CImageList DebugClient::GetObjectImageList()
{
   unsigned int numObjects = 0;
   m_debugInterface->GetObjectListImagelist(numObjects, NULL, 0);

   BYTE* pBitmapData = new BYTE[numObjects * 16 * 16 * 4];

   m_debugInterface->GetObjectListImagelist(numObjects, pBitmapData, numObjects * 16 * 16 * 4);

   // convert from RGBA to BGRA; CImageList needs this
   for (unsigned int i = 0; i < numObjects * 16 * 16; i++)
   {
      BYTE nSwap = pBitmapData[i * 4 + 0];
      pBitmapData[i * 4 + 0] = pBitmapData[i * 4 + 2];
      pBitmapData[i * 4 + 2] = nSwap;
   };

   CImageList imageList;
   imageList.Create(16, 16, ILC_COLOR32, 0, numObjects);

   for (unsigned int n = 0; n < numObjects; n++)
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

bool DebugClient::GetMessage(DebugClientMessage& msg)
{
   m_debugInterface->Lock(true);

   unsigned int nNum = m_debugInterface->GetNumMessages();
   if (nNum == 0)
   {
      m_debugInterface->Lock(false);
      return false;
   }

   unsigned int nTextSize = 0;
   m_debugInterface->GetMessage(msg.m_messageType, msg.m_messageArg1, msg.m_messageArg2, msg.m_messageArg3, nTextSize);

   CHAR* szBuffer = new CHAR[nTextSize + 1];
   szBuffer[nTextSize] = 0;

   ATLVERIFY(true == m_debugInterface->GetMessageText(szBuffer, nTextSize));

   msg.m_messageText = szBuffer;
   delete[] szBuffer;

   m_debugInterface->PopMessage();

   m_debugInterface->Lock(false);

   return true;
}
