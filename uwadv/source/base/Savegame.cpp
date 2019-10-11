//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Underworld Adventures Team
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
/// \file Savegame.cpp
/// \brief savegame and savegames manager classes implementation
//
#include "pch.hpp"
#include "Savegame.hpp"
#include "FileSystem.hpp"
#include "SDL_rwops_gzfile.h"
#include <zlib.h> // for ZLIB_VERSION
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <sstream>

using Base::SavegameInfo;
using Base::Savegame;
using Base::SavegamesManager;

/// \brief current savegame version
/// version history:
/// - version 1: initial version; used from version 0.xx on
/// - version 2: ???
/// - version 3: Introduced with version 0.10; rewrite of most underworld
///   classes required a new version.
///   \todo complete version history
const Uint32 Savegame::s_currentVersion = 3;

/// savegame error message
const char* c_savegameNotFound = "savegame file not found";


SavegameInfo::SavegameInfo()
   :m_gameType(Base::gameUw1),
   m_title("no savegame title"),
   m_gender(0),
   m_appearance(0),
   m_profession(0),
   m_mapLevel(0),
   m_strength(0),
   m_dexterity(0),
   m_intelligence(0),
   m_vitality(0),
   m_imageXRes(0),
   m_imageYRes(0)
{
   // note: this may not be Year 2038 save, depending on what compiler and runtime is used
   // since the date is only used to show it to the user, it should be no problem, though
   time_t now = time(NULL);
   struct tm tm_now = *localtime(&now);

   m_saveDate.m_year = static_cast<Uint16>(tm_now.tm_year);
   m_saveDate.m_month = static_cast<Uint8>(tm_now.tm_mon);
   m_saveDate.m_day = static_cast<Uint8>(tm_now.tm_mday);

   m_saveDate.m_hour = static_cast<Uint8>(tm_now.tm_hour);
   m_saveDate.m_minutes = static_cast<Uint8>(tm_now.tm_min);
   m_saveDate.m_seconds = static_cast<Uint8>(tm_now.tm_sec);
}

void SavegameInfo::Load(Savegame& savegame)
{
   // savegame infos
   m_gameType = savegame.Read8() == 0 ? Base::gameUw1 : Base::gameUw2;
   savegame.ReadString(m_title);
   savegame.ReadString(m_gamePrefix);

   // read save date
   if (savegame.GetVersion() >= 2)
   {
      m_saveDate.m_year = savegame.Read16();
      m_saveDate.m_month = savegame.Read8();
      m_saveDate.m_day = savegame.Read8();

      m_saveDate.m_hour = savegame.Read8();
      m_saveDate.m_minutes = savegame.Read8();
      m_saveDate.m_seconds = savegame.Read8();
   }

   // player infos
   savegame.ReadString(m_playerName);

   m_gender = savegame.Read8();
   m_appearance = savegame.Read8();
   m_profession = savegame.Read8();
   m_mapLevel = savegame.Read8();

   m_strength = savegame.Read8();
   m_dexterity = savegame.Read8();
   m_intelligence = savegame.Read8();
   m_vitality = savegame.Read8();

   // read image
   m_imageXRes = savegame.Read16();
   m_imageYRes = savegame.Read16();

   unsigned int max = m_imageXRes * m_imageYRes;

   m_imageRGBA.clear();
   m_imageRGBA.resize(max);

   for (unsigned int i = 0; i < max; i++)
      m_imageRGBA[i] = savegame.Read32();
}

void SavegameInfo::Save(Savegame& savegame)
{
   // savegame infos
   savegame.Write8(m_gameType == Base::gameUw1 ? 0 : 1);

   savegame.WriteString(m_title);
   savegame.WriteString(m_gamePrefix);

   // write save date
   if (savegame.GetVersion() >= 2)
   {
      savegame.Write16(m_saveDate.m_year);
      savegame.Write8(m_saveDate.m_month);
      savegame.Write8(m_saveDate.m_day);

      savegame.Write8(m_saveDate.m_hour);
      savegame.Write8(m_saveDate.m_minutes);
      savegame.Write8(m_saveDate.m_seconds);
   }

   // player infos
   savegame.WriteString(m_playerName);

   savegame.Write8(static_cast<Uint8>(m_gender));
   savegame.Write8(static_cast<Uint8>(m_appearance));
   savegame.Write8(static_cast<Uint8>(m_profession));
   savegame.Write8(static_cast<Uint8>(m_mapLevel));

   savegame.Write8(static_cast<Uint8>(m_strength));
   savegame.Write8(static_cast<Uint8>(m_dexterity));
   savegame.Write8(static_cast<Uint8>(m_intelligence));
   savegame.Write8(static_cast<Uint8>(m_vitality));

   // write image
   savegame.Write16(static_cast<Uint16>(m_imageXRes));
   savegame.Write16(static_cast<Uint16>(m_imageYRes));

   size_t max = m_imageXRes * m_imageYRes;
   for (size_t i = 0; i < max; i++)
      savegame.Write32(m_imageRGBA[i]);
}

Savegame::Savegame(const std::string& filename, const SavegameInfo& savegameInfo)
   :m_isSaving(true),
   m_saveVersion(s_currentVersion),
   m_info(savegameInfo)
{
   // use highest gz compression ratio
   SDL_RWopsPtr rwops = MakeRWopsPtr(SDL_RWFromGzFile(filename.c_str(), "wb9"));
   if (rwops.get() == NULL)
      throw Base::FileSystemException(c_savegameNotFound, filename, errno);

   this->Base::File::operator=(Base::File(rwops));

   // write header
   BeginSection("header");

   Write32(m_saveVersion);

   m_info.Save(*this);
   EndSection();
}

Savegame::Savegame(const std::string& filename)
   :m_isSaving(false),
   m_saveVersion(s_currentVersion)
{
   SDL_RWopsPtr rwops = MakeRWopsPtr(SDL_RWFromGzFile(filename.c_str(), "rb"));
   if (rwops.get() == NULL)
      throw Base::FileSystemException(c_savegameNotFound, filename, errno);

   this->Base::File::operator=(Base::File(rwops));

   // read header
   BeginSection("header");

   m_saveVersion = Read32();

   // message and assert about loading newer savegames
   if (m_saveVersion > s_currentVersion)
      UaTrace("cannot load savegames of newer version %d (only version up to %u is supported)", m_saveVersion, s_currentVersion);

   UaAssert(m_saveVersion <= s_currentVersion);

   m_info.Load(*this);

   EndSection();
}

void Savegame::ReadString(std::string& text)
{
   text.erase();
   Uint16 uiLen = Base::File::Read16();
   text.reserve(uiLen);

   for (unsigned int i = 0; i < uiLen; i++)
      text.append(1, static_cast<char>(Base::File::Read8()));
}

void Savegame::WriteString(const std::string& text)
{
   Uint16 uiLen = static_cast<Uint16>(text.size());
   Base::File::Write16(uiLen);

   for (unsigned int i = 0; i < uiLen; i++)
      Base::File::Write8(static_cast<Uint8>(text[i]));
}

void Savegame::BeginSection(const std::string& sectionName)
{
   if (m_isSaving)
      WriteString(sectionName);
   else
   {
      std::string readSectionName;
      ReadString(readSectionName);

      // check if section names are the same
      if (readSectionName != sectionName)
         throw Base::RuntimeException("savegame loading: section name mismatch");
   }
}


/// Initializes savegames manager with settings for savegames folder and
/// current game prefix (can be set later when not known yet). After
/// constructing the object call Rescan() to obtain the list of savegames.
SavegamesManager::SavegamesManager(const Base::Settings& settings)
   :m_savegameFolder(settings.GetString(Base::settingSavegameFolder)),
   m_gamePrefix(settings.GetString(Base::settingGamePrefix)),
   m_imageXRes(0),
   m_imageYRes(0)
{
   UaAssert(m_savegameFolder.size() > 0);

   UaTrace("savegames manager is using zlib %s\n", ZLIB_VERSION);

   if (!Base::FileSystem::FolderExists(m_savegameFolder))
   {
      UaTrace("creating savegame folder \"%s\"\n", m_savegameFolder.c_str());
      Base::FileSystem::MakeFolder(m_savegameFolder.c_str());
   }
}

void SavegamesManager::SetNewGamePrefix(const std::string& newGamePrefix)
{
   m_gamePrefix = newGamePrefix;
}

/// \todo filter out savegames that don't have the same prefix
void SavegamesManager::Rescan()
{
   UaAssert(m_savegameFolder.size() > 0);

   m_savegamesList.clear();

   std::string strSearchPath = m_savegameFolder + "/uasave*.uas";
   Base::FileSystem::FindFiles(strSearchPath, m_savegamesList);

   // add quicksave savegame name
   if (m_gamePrefix.size() > 0 && IsQuicksaveAvail())
   {
      std::string strQuicksaveName = GetQuicksaveFilename();
      m_savegamesList.push_back(strQuicksaveName);
   }

   // todo filter out other prefixes

   std::sort(m_savegamesList.begin(), m_savegamesList.end());
}

void SavegamesManager::GetSavegameInfo(size_t index, SavegameInfo& info)
{
   UaAssert(index < m_savegamesList.size());

   Savegame sg = LoadSavegame(index, false);
   info = sg.GetSavegameInfo();
}

/// \param index index in savegame list
/// \param storeImage when true, the savegame preview image is stored
///        internally, so when user decides to save again the same savegame
///        state, we already have a preview image. Set to false when just
///        doing savegame information querying.
Savegame SavegamesManager::LoadSavegame(size_t index, bool storeImage)
{
   UaAssert(index < m_savegamesList.size());

   std::string savegameFilename(GetSavegameFilename(index));

   Savegame sg(savegameFilename);

   if (storeImage)
   {
      const SavegameInfo& info = sg.GetSavegameInfo();
      SetSaveScreenshot(info.m_imageXRes, info.m_imageYRes, info.m_imageRGBA);
   }

   return sg;
}

/// The savegame is constructed with the given SavegameInfo.
/// \param info savegame info to store in savegame
/// \param index index of savegame slot to overwrite; if the default
///                parameter -1 is used, a new slot is used.
Savegame SavegamesManager::SaveSavegame(SavegameInfo info, size_t index)
{
   UaAssert(m_savegameFolder.size() > 0);

   std::string savegameFilename;

   // search new slot on default parameter
   if (index == size_t(-1))
   {
      // Note: This is only going to work when no two instances of uwadv do the
      // same searching at the same time, which is normally not the case.
      index = 0;
      do
      {
         std::ostringstream buffer;

         // create savegame name
         buffer << m_savegameFolder << "/uasave"
            << std::setfill('0') << std::setw(5) << index
            << ".uas";

         savegameFilename = buffer.str();
         index++;

      } while (Base::FileSystem::FileExists(savegameFilename));
   }
   else
      savegameFilename = GetSavegameFilename(index);

   info.m_gamePrefix = m_gamePrefix;

   info.m_imageXRes = m_imageXRes;
   info.m_imageYRes = m_imageYRes;
   info.m_imageRGBA = m_imageSavegame;

   return Savegame(savegameFilename, info);
}

bool SavegamesManager::IsQuicksaveAvail() const
{
   // no prefix? no quicksave savegame
   if (m_gamePrefix.empty())
      return false;

   std::string quicksaveName = GetQuicksaveFilename();

   // check if quicksave savegame file is available
   return Base::FileSystem::FileExists(quicksaveName);
}

Savegame SavegamesManager::SaveQuicksaveSavegame(SavegameInfo info)
{
   UaAssert(m_gamePrefix.size() > 0);

   std::string quicksaveName = GetQuicksaveFilename();

   info.m_title = "Quicksave Savegame";
   info.m_gamePrefix = m_gamePrefix;

   info.m_imageXRes = m_imageXRes;
   info.m_imageYRes = m_imageYRes;
   info.m_imageRGBA = m_imageSavegame;

   return Savegame(quicksaveName, info);
}

std::string SavegamesManager::GetQuicksaveFilename() const
{
   UaAssert(m_savegameFolder.size() > 0);
   UaAssert(m_gamePrefix.size() > 0);

   std::string quicksaveName = m_savegameFolder +
      "/quicksave_" + m_gamePrefix + ".uas";

   return quicksaveName;
}

Savegame SavegamesManager::GetSavegameFromFile(const char* filename)
{
   Savegame sg(filename);
   return sg;
}
