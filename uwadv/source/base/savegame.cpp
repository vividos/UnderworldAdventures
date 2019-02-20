//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Michael Fink
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
/// \file savegame.cpp
/// \brief savegame and savegames manager classes implementation
//
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

/// \brief current savegame version
// version history:
// - version 1: initial version; used from version 0.xx on
// - version 2: ???
// - version 3: Introduced with version 0.10; rewrite of most underworld
//   classes required a new version.
//   \todo complete version history
const Uint32 Savegame::s_currentVersion = 3;

/// savegame error message
const char* c_savegameNotFound = "savegame file not found";


SavegameInfo::SavegameInfo()
   :gameType(Base::gameUw1),
   title("no savegame title"),
   gender(0),
   appearance(0),
   profession(0),
   mapLevel(0),
   strength(0),
   dexterity(0),
   intelligence(0),
   vitality(0),
   imageXRes(0),
   imageYRes(0)
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
   savegame.ReadString(title);
   savegame.ReadString(gamePrefix);

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
   savegame.ReadString(playerName);

   gender = savegame.Read8();
   appearance = savegame.Read8();
   profession = savegame.Read8();
   mapLevel = savegame.Read8();

   strength = savegame.Read8();
   dexterity = savegame.Read8();
   intelligence = savegame.Read8();
   vitality = savegame.Read8();

   // read image
   imageXRes = savegame.Read16();
   imageYRes = savegame.Read16();

   unsigned int max = imageXRes * imageYRes;

   imageRGBA.clear();
   imageRGBA.resize(max);

   for (unsigned int i = 0; i < max; i++)
      imageRGBA[i] = savegame.Read32();
}

void SavegameInfo::Save(Savegame& savegame)
{
   // savegame infos
   savegame.Write8(gameType == Base::gameUw1 ? 0 : 1);

   savegame.WriteString(title);
   savegame.WriteString(gamePrefix);

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
   savegame.WriteString(playerName);

   savegame.Write8(static_cast<Uint8>(gender));
   savegame.Write8(static_cast<Uint8>(appearance));
   savegame.Write8(static_cast<Uint8>(profession));
   savegame.Write8(static_cast<Uint8>(mapLevel));

   savegame.Write8(static_cast<Uint8>(strength));
   savegame.Write8(static_cast<Uint8>(dexterity));
   savegame.Write8(static_cast<Uint8>(intelligence));
   savegame.Write8(static_cast<Uint8>(vitality));

   // write image
   savegame.Write16(static_cast<Uint16>(imageXRes));
   savegame.Write16(static_cast<Uint16>(imageYRes));

   unsigned int max = imageXRes * imageYRes;
   for (unsigned int i = 0; i < max; i++)
      savegame.Write32(imageRGBA[i]);
}


// Savegame methods

Savegame::Savegame(const std::string& strFilename, const SavegameInfo& savegameInfo)
   :m_isSaving(true),
   m_saveVersion(s_currentVersion),
   m_info(savegameInfo)
{
   // use highest gz compression ratio
   SDL_RWopsPtr rwops = MakeRWopsPtr(SDL_RWFromGzFile(strFilename.c_str(), "wb9"));
   if (rwops.get() == NULL)
      throw Base::FileSystemException(c_savegameNotFound, strFilename, errno);

   this->Base::File::operator=(Base::File(rwops));

   // write header
   BeginSection("header");

   Write32(m_saveVersion);

   m_info.Save(*this);
   EndSection();
}

Savegame::Savegame(const std::string& strFilename)
   :m_isSaving(false),
   m_saveVersion(s_currentVersion)
{
   SDL_RWopsPtr rwops = MakeRWopsPtr(SDL_RWFromGzFile(strFilename.c_str(), "rb"));
   if (rwops.get() == NULL)
      throw Base::FileSystemException(c_savegameNotFound, strFilename, errno);

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

void SavegamesManager::GetSavegameInfo(unsigned int index, SavegameInfo& info)
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
Savegame SavegamesManager::LoadSavegame(unsigned int index, bool storeImage)
{
   UaAssert(index < m_savegamesList.size());

   std::string savegameFilename(GetSavegameFilename(index));

   Savegame sg(savegameFilename);

   if (storeImage)
   {
      const SavegameInfo& info = sg.GetSavegameInfo();
      SetSaveScreenshot(info.imageXRes, info.imageYRes, info.imageRGBA);
   }

   return sg;
}

/// The savegame is constructed with the given SavegameInfo.
/// \param info savegame info to store in savegame
/// \param index index of savegame slot to overwrite; if the default
///                parameter -1 is used, a new slot is used.
Savegame SavegamesManager::SaveSavegame(SavegameInfo info, unsigned int index)
{
   UaAssert(m_savegameFolder.size() > 0);

   std::string savegameFilename;

   // search new slot on default parameter
   if (index == unsigned(-1))
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

   info.gamePrefix = m_gamePrefix;

   info.imageXRes = m_imageXRes;
   info.imageYRes = m_imageYRes;
   info.imageRGBA = m_imageSavegame;

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

   info.title = "Quicksave Savegame";
   info.gamePrefix = m_gamePrefix;

   info.imageXRes = m_imageXRes;
   info.imageYRes = m_imageYRes;
   info.imageRGBA = m_imageSavegame;

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
