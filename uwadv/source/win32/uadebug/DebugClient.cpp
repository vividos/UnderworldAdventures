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
/*! \file DebugClient.cpp

   \brief debugger client class

*/

// includes
#include "stdatl.hpp"
#include "DebugClient.hpp"

// data

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


// pos and name don't count as columns in CDebugClient
const int g_nObjListColumns = 15-2;

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


// CDebugClientPlayerInterface methods

unsigned int CDebugClientPlayerInterface::GetAttrCount()
{
   return sizeof(g_aszAttrNames)/sizeof(g_aszAttrNames[0])-4;
}

LPCTSTR CDebugClientPlayerInterface::GetPosInfoName(unsigned int info)
{
   ATLASSERT(info < 4);
   return g_aszAttrNames[info];
}

LPCTSTR CDebugClientPlayerInterface::GetAttrName(unsigned int attr)
{
   ATLASSERT(attr < GetAttrCount());
   return g_aszAttrNames[attr+4];
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
   return sizeof(g_aColumnInfo)/sizeof(g_aColumnInfo[0]);
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
   nSubcode = nSubcode == 1 ? 0 : nSubcode-2;

   return m_pDebugInterface->get_objlist_info(m_nLevel, nPos, nSubcode);
}

void CDebugClientObjectInterface::SetItemInfo(unsigned int nPos, unsigned int nSubcode, unsigned int nInfo)
{
   ATLASSERT(nPos < 0x400);
   ATLASSERT(nSubcode != 0 && nSubcode != 2); // can't set pos and name

   // remap to fields
   nSubcode = nSubcode == 1 ? 0 : nSubcode-2;

   m_pDebugInterface->set_objlist_info(m_nLevel, nPos, nSubcode, nInfo);
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
   char szBuffer[512];
   szBuffer[511] = 0;

   m_pDebugInterface->get_game_path(szBuffer,511);

   CString cszGamePath(szBuffer);
   cszGamePath.Replace(_T("/"), _T("\\"));
   cszGamePath += _T("\\");

   return cszGamePath;
}

void CDebugClientInterface::LoadGameCfg(LPCTSTR pszPrefix)
{
   m_pDebugInterface->load_game(pszPrefix);
}

void CDebugClientInterface::PauseGame(bool pause)
{
   ATLASSERT(FALSE);
}

unsigned int CDebugClientInterface::GetNumLevels()
{
   return m_pDebugInterface->get_num_levels();
}

void CDebugClientInterface::SetWorkingLevel(unsigned int level)
{
   m_nLevel = level;
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


unsigned int CDebugClientInterface::GetTileInfo(unsigned int xpos, unsigned int ypos, unsigned int type)
{
   return m_pDebugInterface->get_tile_info_value(m_nLevel, xpos, ypos, type);
}

void CDebugClientInterface::SetTileInfo(unsigned int xpos, unsigned int ypos, unsigned int type, unsigned int val)
{
   // TODO
   ATLASSERT(FALSE);
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
   CString cszText;
   unsigned int ret = m_pDebugInterface->get_game_string(block, nr, cszText.GetBuffer(512), 512);
   cszText.ReleaseBuffer();

   return cszText;
}

CImageList CDebugClientInterface::GetObjectImageList()
{
   unsigned int num_objects = 0;
   m_pDebugInterface->get_object_list_imagelist(num_objects, NULL, 0);

   BYTE* pBitmapData = new BYTE[num_objects*16*16*4];

   m_pDebugInterface->get_object_list_imagelist(num_objects, pBitmapData, num_objects*16*16*4);

   // convert from RGBA to BGRA; CImageList needs this
   for(unsigned int i=0; i<num_objects*16*16; i++)
   {
      BYTE nSwap = pBitmapData[i*4+0];
      pBitmapData[i*4+0] = pBitmapData[i*4+2];
      pBitmapData[i*4+2] = nSwap;
   };

//   CBitmap bitmap;
//   bitmap.CreateBitmap(num_objects*16,  16, 1, 32, pBitmapData);

   CImageList imageList;
   imageList.Create(16, 16, ILC_COLOR32, 0, num_objects);

   for(unsigned int n=0; n<num_objects; n++)
   {
      BYTE buffer[16*16*4];

      memcpy(buffer, &pBitmapData[n*16*16*4], 16*16*4);

      CBitmap bitmap;
      bitmap.CreateBitmap(16,  16, 1, 32, buffer);
      imageList.Add(bitmap, RGB(255,255,255));
   }

   delete[] pBitmapData;

   return imageList;
}

/*
bool CDebugClientInterface::GetMessage(CDebugClientMessage& msg)
{
   // TODO
   ATLASSERT(FALSE);

   unsigned int text_size=0;
   bool ret = get_message(msg.msg_type, msg.msg_arg1, msg.msg_arg2,
      msg.msg_arg3, text_size);
   if (ret && text_size>0)
   {
      std::vector<char> buffer(text_size+1);
      ret = get_message_text(&buffer[0], text_size);

      buffer[text_size]=0;
      msg.msg_text.assign(&buffer[0]);
   }
   return ret; return false;
}
*/
