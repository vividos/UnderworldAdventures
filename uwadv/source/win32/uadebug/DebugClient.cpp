//
// Underworld Adventures Debugger - a debugger tool for Underworld Adventures
// Copyright (c) 2004,2005,2019,2022 Underworld Adventures Team
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
#include "pch.hpp"
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

/// tilemap names for uw1 and uw2
std::map<unsigned int, LPCTSTR> g_tilemapNames[2] =
{
   {
      { 0, _T("Goblins") },
      { 1, _T("Mountainmen") },
      { 2, _T("Lizardmen") },
      { 3, _T("Knights") },
      { 4, _T("Ghouls") },
      { 5, _T("Mages") },
      { 6, _T("Tyball") },
      { 7, _T("Volcano") },
      { 8, _T("Moongate") },
   },
   {
      { 0, _T("Castle B1") },
      { 1, _T("Sewers B2") },
      { 2, _T("Sewers B3") },
      { 3, _T("Sewers B4") },
      { 4, _T("Sewers B5") },

      { 8, _T("Prison Tower PT1") },
      { 9, _T("Prison Tower PT2") },
      { 10, _T("Prison Tower PT3") },
      { 11, _T("Prison Tower PT4") },
      { 12, _T("Prison Tower PT5") },
      { 13, _T("Prison Tower PT6") },
      { 14, _T("Prison Tower PT7") },
      { 15, _T("Prison Tower PT8") },

      { 16, _T("Killorn Keep KK1") },
      { 17, _T("Killorn Keep KK2") },
      // KKA

      { 24, _T("Ice Caverns IC1") },
      { 25, _T("Ice Caverns IC2") },

      { 32, _T("Talorus T1") },
      { 33, _T("Talorus T2") },

      { 40, _T("Scintillus Academy SA1") },
      { 41, _T("Scintillus Academy SA2") },
      { 42, _T("Scintillus Academy SA3") },
      { 43, _T("Scintillus Academy SA4") },
      { 44, _T("Scintillus Academy SA5") },
      { 45, _T("Scintillus Academy SA6") },
      { 46, _T("Scintillus Academy SA7") },
      { 47, _T("Scintillus Academy SA8") },

      { 48, _T("Loth's Tomb LT1") },
      { 49, _T("Loth's Tomb LT2") },
      { 50, _T("Loth's Tomb LT3") },
      { 51, _T("Loth's Tomb LT4") },

      { 56, _T("Pits of Carnage PC1") },
      { 57, _T("Pits of Carnage PC2") },
      { 58, _T("Pits of Carnage PC3") },

      { 64, _T("Ethereal Void Red EVR") },
      { 65, _T("Ethereal Void Blue EVB") },
      { 66, _T("Ethereal Void Yellow EVY") },
      { 67, _T("Ethereal Void Purple EVP") },

      // https://www.ttlg.com/forums/showthread.php?t=131926
      { 69, _T("Secret Level") },
   }
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

CodeDebuggerType DebugClientCodeDebugger::GetDebuggerType() const
{
   return static_cast<CodeDebuggerType>(m_codeDebugger->GetDebuggerType());
}

void DebugClientCodeDebugger::PrepareDebugInfo()
{
   m_codeDebugger->PrepareDebugInfo();
}

bool DebugClientCodeDebugger::IsSourceAvail() const
{
   // TODO implement
   return true;
}

bool DebugClientCodeDebugger::IsCodeAvail() const
{
   // TODO implement
   return true;
}

void DebugClientCodeDebugger::SetCommand(CodeDebuggerCommand command)
{
   m_codeDebugger->SetDebuggerCommand(static_cast<DebugServerCodeDebuggerCommand>(command));
   m_codeDebugger->SetDebuggerState(codeDebuggerStateRunning);
}

CodeDebuggerState DebugClientCodeDebugger::GetState() const
{
   return static_cast<CodeDebuggerState>(m_codeDebugger->GetDebuggerState());
}

CodePosition DebugClientCodeDebugger::GetCurrentPos()
{
   CodePosition pos;

   bool bSourcefileIsValid = false;

   m_codeDebugger->GetCurrentPos(pos.m_sourceFileNameIndex,
      pos.m_sourceFileLine, pos.m_codePosition, bSourcefileIsValid);

   if (bSourcefileIsValid)
      pos.m_codePosition = size_t(-1);
   else
      pos.m_sourceFileNameIndex = size_t(-1);

   return pos;
}

size_t DebugClientCodeDebugger::GetBreakpointCount() const
{
   return m_codeDebugger->GetNumBreakpoints();
}

void DebugClientCodeDebugger::GetBreakpointInfo(size_t breakpointIndex, BreakpointInfo& breakpointInfo) const
{
   ATLASSERT(breakpointIndex < GetBreakpointCount());
   m_codeDebugger->GetBreakpointInfo(breakpointIndex,
      breakpointInfo.m_location.m_sourceFileNameIndex,
      breakpointInfo.m_location.m_sourceFileLine,
      breakpointInfo.m_location.m_codePosition,
      breakpointInfo.m_isVisible);
}

unsigned int DebugClientCodeDebugger::GetCallstackHeight() const
{
   // TODO implement
   return 0;
}

void DebugClientCodeDebugger::GetCallstackInfo(unsigned int atLevel, CallstackInfo& callstackInfo) const
{
   ATLASSERT(atLevel < GetCallstackHeight());
   UNUSED(atLevel);
   UNUSED(callstackInfo);
   // TODO implement
}

size_t DebugClientCodeDebugger::GetSourceFileCount() const
{
   return m_codeDebugger->GetNumSourcefiles();
}

CString DebugClientCodeDebugger::GetSourceFileName(size_t index)
{
   ATLASSERT(index < GetSourceFileCount());

   size_t size = m_codeDebugger->GetSourcefileName(index, NULL, 0);

   CHAR* szText = new CHAR[size + 1];
   szText[size] = 0;

   m_codeDebugger->GetSourcefileName(index, szText, size + 1);

   USES_CONVERSION;
   CFilename sourceFilename(A2CT(szText));
   delete[] szText;

   if (sourceFilename.IsRelativePath())
      sourceFilename.MakeAbsoluteToCurrentDir();

   ATLASSERT(sourceFilename.IsValidObject());

   return sourceFilename.Get();
}

bool DebugClientCodeDebugger::GetSourceFromCodePos(unsigned int codePos,
   CString& filename, unsigned int& lineNumber, unsigned int& lineDisplacement)
{
   size_t sourcefileIndex = 0; // TODO
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

IGameInstance& DebugClient::GetGameInstance()
{
   return m_debugInterface->GetGameInstance();
}

void DebugClient::Lock(bool locked)
{
   m_debugInterface->Lock(locked);
}

bool DebugClient::IsStudioMode() const
{
   return m_debugInterface->IsStudioMode();
}

CString DebugClient::GetGameConfigPath()
{
   CHAR szBuffer[512];
   szBuffer[511] = 0;

   m_debugInterface->GetGamePath(szBuffer, 511);

   USES_CONVERSION;
   CFilename gamePath(A2CT(szBuffer));

   gamePath.MakeAbsoluteToCurrentDir();
   return gamePath.IsValidObject() ? gamePath.Get() : CString();
}

void DebugClient::LoadGameConfig(LPCTSTR pszPrefix)
{
   CStringA prefixA(pszPrefix);
   m_debugInterface->LoadGame(prefixA);
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

const Underworld::Underworld& DebugClient::GetUnderworld() const
{
   return m_debugInterface->GetUnderworld();
}

Underworld::Underworld& DebugClient::GetUnderworld()
{
   return m_debugInterface->GetUnderworld();
}

size_t DebugClient::GetNumLevels() const
{
   return m_debugInterface->GetNumLevels();
}

bool DebugClient::IsLevelEmpty(unsigned int level) const
{
   return m_debugInterface->IsLevelEmpty(level);
}

void DebugClient::SetWorkingLevel(unsigned int level)
{
   m_level = level;
}

CString DebugClient::GetLevelName(unsigned int level) const
{
   if (IsLevelEmpty(level))
      return _T("empty");

   bool isUw2 = m_debugInterface->GetGameInstance().GetSettings().GetGameType() == Base::gameUw2;
   const std::map<unsigned int, LPCTSTR>& tilemapNames =
      g_tilemapNames[isUw2 ? 1 : 0];

   if (tilemapNames.find(level) != tilemapNames.end())
      return tilemapNames.find(level)->second;
   else
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

std::shared_ptr<LevelEditor> DebugClient::CreateLevelEditor(const void* windowHandle)
{
   return m_debugInterface->CreateLevelEditor(windowHandle);
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
      m_codeDebuggerIdList.push_back(codeDebuggerId);
}

void DebugClient::RemoveCodeDebugger(unsigned int codeDebuggerId)
{
   ATLASSERT(IsValidCodeDebuggerId(codeDebuggerId));

   auto iter = std::find(m_codeDebuggerIdList.begin(), m_codeDebuggerIdList.end(), codeDebuggerId);
   m_codeDebuggerIdList.erase(iter);
}

size_t DebugClient::GetCodeDebuggerCount() const
{
   return m_codeDebuggerIdList.size();
}

unsigned int DebugClient::GetCodeDebuggerByIndex(size_t index) const
{
   ATLASSERT(index < GetCodeDebuggerCount());
   return m_codeDebuggerIdList[index];
}

bool DebugClient::IsValidCodeDebuggerId(unsigned int codeDebuggerId) const
{
   size_t max = m_codeDebuggerIdList.size();
   for (size_t index = 0; index < max; index++)
   {
      if (m_codeDebuggerIdList[index] == codeDebuggerId)
         return true;
   }

   return false;
}

DebugClientCodeDebugger DebugClient::GetCodeDebuggerInterface(unsigned int codeDebuggerId)
{
   ATLASSERT(true == IsValidCodeDebuggerId(codeDebuggerId));

   DebugClientCodeDebugger cdi;
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
