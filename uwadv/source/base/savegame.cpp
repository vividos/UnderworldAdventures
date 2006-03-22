/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2002,2003,2004,2005,2006 Michael Fink

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
/*! \file savegame.cpp

   \brief savegame and savegames manager classes implementation

*/

// needed includes
#include "base.hpp"
#include "savegame.hpp"
#include "filesystem.hpp"
#include "SDL_rwops_gzfile.h"
#include <zlib.h> // for ZLIB_VERSION
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <sstream>

using Base::SavegameInfo;
using Base::Savegame;
using Base::SavegamesManager;

// constants

//! current savegame version
/*! version history:
    - version 1: initial version; used from version 0.xx on
    - version 2: ??? 
    - version 3: Introduced with version 0.10; rewrite of most underworld
      classes required a new version.

   \todo complete version history
*/
const Uint32 Savegame::s_uiCurrentVersion = 3;

//! savegame error message
const char* c_strSavegameNotFound = "savegame file not found";


// SavegameInfo methods

SavegameInfo::SavegameInfo()
:gameType(Base::gameUw1),
 strTitle("no savegame title"),
 uiGender(0),
 uiAppearance(0),
 uiProfession(0),
 uiMapLevel(0),
 uiStrength(0),
 uiDexterity(0),
 uiIntelligence(0),
 uiVitality(0),
 uiImageXRes(0),
 uiImageYRes(0)
{
   // note: this may not be Year 2038 save, depending on what compiler and runtime is used
   // since the date is only used to show it to the user, it should be no problem, though
   time_t now = time(NULL);
   struct tm tm_now = *localtime(&now);

   saveDate.uiYear = static_cast<Uint16>(tm_now.tm_year);
   saveDate.uiMonth = static_cast<Uint8>(tm_now.tm_mon);
   saveDate.uiDay = static_cast<Uint8>(tm_now.tm_mday);

   saveDate.uiHours = static_cast<Uint8>(tm_now.tm_hour);
   saveDate.uiMinutes = static_cast<Uint8>(tm_now.tm_min);
   saveDate.uiSeconds = static_cast<Uint8>(tm_now.tm_sec);
}

void SavegameInfo::Load(Savegame& savegame)
{
   // savegame infos
   gameType = savegame.Read8() == 0 ? Base::gameUw1 : Base::gameUw2;
   savegame.ReadString(strTitle);
   savegame.ReadString(strGamePrefix);

   // read save date
   if (savegame.GetVersion() >= 2)
   {
      saveDate.uiYear = savegame.Read16();
      saveDate.uiMonth = savegame.Read8();
      saveDate.uiDay = savegame.Read8();

      saveDate.uiHours = savegame.Read8();
      saveDate.uiMinutes = savegame.Read8();
      saveDate.uiSeconds = savegame.Read8();
   }

   // player infos
   savegame.ReadString(strPlayerName);

   uiGender = savegame.Read8();
   uiAppearance = savegame.Read8();
   uiProfession = savegame.Read8();
   uiMapLevel = savegame.Read8();

   uiStrength = savegame.Read8();
   uiDexterity = savegame.Read8();
   uiIntelligence = savegame.Read8();
   uiVitality = savegame.Read8();

   // read image
   uiImageXRes = savegame.Read16();
   uiImageYRes = savegame.Read16();

   unsigned int max = uiImageXRes * uiImageYRes;

   vecImageRGBA.clear();
   vecImageRGBA.resize(max);

   for(unsigned int i=0; i<max; i++)
      vecImageRGBA[i] = savegame.Read32();
}

void SavegameInfo::Save(Savegame& savegame)
{
   // savegame infos
   savegame.Write8(gameType == Base::gameUw1 ? 0 : 1);

   savegame.WriteString(strTitle);
   savegame.WriteString(strGamePrefix);

   // write save date
   if (savegame.GetVersion() >= 2)
   {
      savegame.Write16(saveDate.uiYear);
      savegame.Write8(saveDate.uiMonth);
      savegame.Write8(saveDate.uiDay);

      savegame.Write8(saveDate.uiHours);
      savegame.Write8(saveDate.uiMinutes);
      savegame.Write8(saveDate.uiSeconds);
   }

   // player infos
   savegame.WriteString(strPlayerName);

   savegame.Write8(static_cast<Uint8>(uiGender));
   savegame.Write8(static_cast<Uint8>(uiAppearance));
   savegame.Write8(static_cast<Uint8>(uiProfession));
   savegame.Write8(static_cast<Uint8>(uiMapLevel));

   savegame.Write8(static_cast<Uint8>(uiStrength));
   savegame.Write8(static_cast<Uint8>(uiDexterity));
   savegame.Write8(static_cast<Uint8>(uiIntelligence));
   savegame.Write8(static_cast<Uint8>(uiVitality));

   // write image
   savegame.Write16(static_cast<Uint16>(uiImageXRes));
   savegame.Write16(static_cast<Uint16>(uiImageYRes));

   unsigned int max = uiImageXRes * uiImageYRes;
   for(unsigned int i=0; i<max; i++)
      savegame.Write32(vecImageRGBA[i]);
}


// Savegame methods

Savegame::Savegame(const std::string& strFilename, const SavegameInfo& savegameInfo)
:m_bSaving(true),
 m_uiSaveVersion(s_uiCurrentVersion),
 m_info(savegameInfo)
{
   // use highest gz compression ratio
   SDL_RWopsPtr rwops = SDL_RWopsPtr(SDL_RWFromGzFile(strFilename.c_str(), "wb9"));
   if (rwops.get() == NULL)
      throw Base::FileSystemException(c_strSavegameNotFound, strFilename, errno);

   this->Base::File::operator=(Base::File(rwops));

   // write header
   BeginSection("header");

   Write32(m_uiSaveVersion);

   m_info.Save(*this);
   EndSection();
}

Savegame::Savegame(const std::string& strFilename)
:m_bSaving(false),
 m_uiSaveVersion(s_uiCurrentVersion)
{
   SDL_RWopsPtr rwops = SDL_RWopsPtr(SDL_RWFromGzFile(strFilename.c_str(), "rb"));
   if (rwops.get() == NULL)
      throw Base::FileSystemException(c_strSavegameNotFound, strFilename, errno);

   this->Base::File::operator=(Base::File(rwops));

   // read header
   BeginSection("header");

   m_uiSaveVersion = Read32();

   // message and assert about loading newer savegames
   if (m_uiSaveVersion > s_uiCurrentVersion)
      UaTrace("cannot load savegames of newer version %d (only version up to %u is supported)", m_uiSaveVersion, s_uiCurrentVersion);

   UaAssert(m_uiSaveVersion <= s_uiCurrentVersion);

   m_info.Load(*this);

   EndSection();
}

void Savegame::ReadString(std::string& strText)
{
   // read length
   strText.erase();
   Uint16 uiLen = Base::File::Read16();
   strText.reserve(uiLen);

   // read characters
   for(unsigned int i=0; i<uiLen; i++)
      strText.append(1, static_cast<char>(Base::File::Read8()));
}

void Savegame::WriteString(const std::string& strText)
{
   // write length
   Uint16 uiLen = static_cast<Uint16>(strText.size());
   Base::File::Write16(uiLen);

   // write characters
   for(unsigned int i=0; i<uiLen; i++)
      Base::File::Write8(static_cast<Uint8>(strText[i]));
}

void Savegame::BeginSection(const std::string& strSectionName)
{
   if (m_bSaving)
      WriteString(strSectionName);
   else
   {
      std::string strReadSectionName;
      ReadString(strReadSectionName);

      // check if section names are the same
      if (strReadSectionName != strSectionName)
         throw Base::RuntimeException("savegame loading: section name mismatch");
   }
}


// SavegamesManager methods

/*! Initializes savegames manager with settings for savegames folder and
    current game prefix (can be set later when not known yet). After
    constructing the object call Rescan() to obtain the list of savegames.
*/
SavegamesManager::SavegamesManager(const Base::Settings& settings)
:m_strSavegameFolder(settings.GetString(Base::settingSavegameFolder)),
 m_strGamePrefix(settings.GetString(Base::settingGamePrefix)),
 m_uiImageXRes(0),
 m_uiImageYRes(0)
{
   UaAssert(m_strSavegameFolder.size() > 0);

   // print zlib version
   UaTrace("savegames manager is using zlib %s\n", ZLIB_VERSION);

   // try to create savegames folder (when not already present)
   if (!Base::FileSystem::FolderExists(m_strSavegameFolder))
   {
      UaTrace("creating savegame folder \"%s\"\n", m_strSavegameFolder.c_str());
      Base::FileSystem::MakeFolder(m_strSavegameFolder.c_str());
   }
}

/*! \todo filter out savegames that don't have the same prefix */
void SavegamesManager::Rescan()
{
   UaAssert(m_strSavegameFolder.size() > 0);

   m_vecSavegames.clear();

   // find all savegame files
   std::string strSearchPath = m_strSavegameFolder + "/uasave*.uas";
   Base::FileSystem::FindFiles(strSearchPath, m_vecSavegames);

   // add quicksave savegame name
   if (m_strGamePrefix.size() > 0 && IsQuicksaveAvail())
   {
      std::string strQuicksaveName = GetQuicksaveFilename();
      m_vecSavegames.push_back(strQuicksaveName);
   }

   // todo filter out other prefixes

   std::sort(m_vecSavegames.begin(), m_vecSavegames.end());
}

void SavegamesManager::GetSavegameInfo(unsigned int uiIndex, SavegameInfo& info)
{
   UaAssert(uiIndex < m_vecSavegames.size());

   // open savegame and retrieve info
   Savegame sg = LoadSavegame(uiIndex, false);
   info = sg.GetSavegameInfo();
}

/*! \param uiIndex index in savegame list
    \param bStoreImage when true, the savegame preview image is stored
           internally, so when user decides to save again the same savegame
           state, we already have a preview image. Set to false when just
           doing savegame information querying.
*/
Savegame SavegamesManager::LoadSavegame(unsigned int uiIndex, bool bStoreImage)
{
   UaAssert(uiIndex < m_vecSavegames.size());

   std::string strSavegameFilename(GetSavegameFilename(uiIndex));

   // open savegame for loading
   Savegame sg(strSavegameFilename);

   // set loaded image as new savegame image
   if (bStoreImage)
   {
      const SavegameInfo& info = sg.GetSavegameInfo();
      SetSaveScreenshot(info.uiImageXRes, info.uiImageYRes, info.vecImageRGBA);
   }

   return sg;
}

/*! The savegame is constructed with the given SavegameInfo.
    \param info savegame info to store in savegame
    \param uiIndex index of savegame slot to overwrite; if the default
                   parameter -1 is used, a new slot is used.
*/
Savegame SavegamesManager::SaveSavegame(SavegameInfo info, unsigned int uiIndex)
{
   UaAssert(m_strSavegameFolder.size() > 0);

   std::string strSavegameFilename;

   // search new slow on default parameter
   if (uiIndex == unsigned(-1))
   {
      // Note: This is only going to work when no two instances of uwadv do the
      // same searching at the same time, which is normally not the case.
      uiIndex = 0;
      do
      {
         std::ostringstream buffer;

         // create savegame name
         buffer << m_strSavegameFolder << "/uasave"
            << std::setfill('0') << std::setw(5) << uiIndex
            << ".uas";

         strSavegameFilename = buffer.str();
         uiIndex++;

      } while( Base::FileSystem::FileExists(strSavegameFilename) );
   }
   else
      strSavegameFilename = GetSavegameFilename(uiIndex);

   // set game prefix
   info.strGamePrefix = m_strGamePrefix;

   // add savegame preview image to savegame info
   info.uiImageXRes = m_uiImageXRes;
   info.uiImageYRes = m_uiImageYRes;
   info.vecImageRGBA = m_vecImageSavegame;

   // open savegame for saving
   return Savegame(strSavegameFilename, info);
}

bool SavegamesManager::IsQuicksaveAvail() const
{
   // no prefix? no quicksave savegame for you!
   if (m_strGamePrefix.size() == 0)
      return false;

   std::string strQuicksaveName = GetQuicksaveFilename();

   // check if quicksave savegame file is available
   return Base::FileSystem::FileExists(strQuicksaveName);
}

Savegame SavegamesManager::SaveQuicksaveSavegame(SavegameInfo info)
{
   UaAssert(m_strGamePrefix.size() > 0);

   std::string strQuicksaveName = GetQuicksaveFilename();

   // set title and prefix
   info.strTitle = "Quicksave Savegame";
   info.strGamePrefix = m_strGamePrefix;

   // set preview image data
   info.uiImageXRes = m_uiImageXRes;
   info.uiImageYRes = m_uiImageYRes;
   info.vecImageRGBA = m_vecImageSavegame;

   return Savegame(strQuicksaveName, info);
}

std::string SavegamesManager::GetQuicksaveFilename() const
{
   UaAssert(m_strSavegameFolder.size() > 0);
   UaAssert(m_strGamePrefix.size() > 0);

   // create quicksave filename
   std::string strQuicksaveName = m_strSavegameFolder +
      "/quicksave_" + m_strGamePrefix + ".uas";

   return strQuicksaveName;
}
