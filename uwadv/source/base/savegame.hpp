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
/*! \file savegame.hpp

   \brief savegame and savegames manager classes

*/

// include guard
#ifndef uwadv_base_savegame_hpp_
#define uwadv_base_savegame_hpp_

// needed includes
#include "settings.hpp"
#include "file.hpp"
#include <vector>

namespace Base
{

class Settings;
class Savegame;

// structs

//! Savegame info
/*! Saves infos about a savegame that can be shown in the savegames screen. The
    infos are written into the first section of the savegame and can quickly be
    extracted, e.g. for savegame scanning.
*/
struct SavegameInfo
{
public:
   //! ctor
   SavegameInfo();

   //! loads savegame infos from savegame
   void Load(Savegame& savegame);

   //! saves savegame infos to savegame
   void Save(Savegame& savegame);

public:
   //! game type
   Base::EUwGameType gameType;

   //! savegame title
   std::string strTitle;

   //! name of game prefix
   std::string strGamePrefix;

   //! date when savegame was saved
   struct SaveDate
   {
      //! ctor
      SaveDate()
         : uiYear(0), uiMonth(0), uiDay(0), uiHours(0), uiMinutes(0), uiSeconds(0){}

      Uint16 uiYear;    //!< year
      Uint8  uiMonth;   //!< month
      Uint8  uiDay;     //!< day

      Uint8  uiHours;   //!< hours
      Uint8  uiMinutes; //!< minutes
      Uint8  uiSeconds; //!< seconds

   } saveDate;

   // player infos
   std::string strPlayerName; //!< player name
   unsigned int uiGender;     //!< player gender (0=male, 1=female)
   unsigned int uiAppearance; //!< player appearance (0..4)
   unsigned int uiProfession; //!< player profession (0..7)
   unsigned int uiMapLevel;   //!< current map level

   // basic player stats
   unsigned int uiStrength;   //!< strength
   unsigned int uiDexterity;  //!< dexterity
   unsigned int uiIntelligence; //!< intelligence
   unsigned int uiVitality;   //!< vitality

   // savegame preview screenshot

   unsigned int uiImageXRes;  //!< screenshot x res
   unsigned int uiImageYRes;  //!< screenshot y res

   //! screenshot bytes in RGBA format
   std::vector<Uint32> vecImageRGBA;
};


// classes

//! Savegame class
/*! Represents a savegame on disk and can be opened for loading or saving.
    The SavegameInfo class contains infos about the savegame.

    A savegame contains several sections that are started with BeginSection()
    and ended with EndSection(). Values can be read and written using the
    methods provided by Base::File. The savegame is automatically clsed when
    the object is destroyed.

    A savegame carries a version number that lets the user decide which fields
    or parts of a savegame have to be loaded. This way newer game versions can
    load older savegames.
*/
class Savegame: public Base::File
{
public:
   //! ctor; opens a savegame for saving
   Savegame(const std::string& strFilename, const SavegameInfo& savegameInfo);

   //! ctor; opens a savegame for loading
   Savegame(const std::string& strFilename);

   // savegame loading functions

   //! returns version of savegame to load/save
   Uint32 GetVersion() const { return m_uiSaveVersion; }

   //! reads string from savegame
   void ReadString(std::string& strText);

   // savegame saving functions

   //! writes string to savegame
   void WriteString(const std::string& strText);

   // common functions

   //! starts new section to load/save
   void BeginSection(const std::string& strSectionName);

   //! ends current section
   void EndSection(){}

   //! returns savegame info
   SavegameInfo& GetSavegameInfo(){ return m_info; }

protected:
   //! current savegame version
   static const Uint32 s_uiCurrentVersion;

   //! true when currently saving
   bool m_bSaving;

   //! savegame version
   Uint32 m_uiSaveVersion;

   //! savegame info
   SavegameInfo m_info;
};


//! Savegames manager
/*! Manages all savegames stored in the game's savegame folder. It also
    supports a special type of savegame called the quicksave savegame.

    The game prefix of the currently running game is needed to determine if
    there are savegames available, and if there's a quicksave savegame for
    this prefix. If no prefix is set, all savegames are listed.

    The savegame naming scheme is "uasaveXXXXX.uas", where XXXXX is a decimal
    number. Quicksave savegames get the name "quicksave_{prefix}.uas"
*/
class SavegamesManager
{
public:
   //! ctor
   SavegamesManager(const Settings& settings);

   //! sets new game prefix
   void SetNewGamePrefix(const std::string& strNewGamePrefix);

   //! rescans for existing savegames
   void Rescan();

   //! returns number of available savegames
   unsigned int GetSavegamesCount() const { return m_vecSavegames.size(); }

   //! returns savegame infos
   void GetSavegameInfo(unsigned int uiIndex, SavegameInfo& info);

   //! returns filename of savegame file
   std::string GetSavegameFilename(unsigned int uiIndex) const
   {
      UaAssert(uiIndex < m_vecSavegames.size());
      return m_vecSavegames[uiIndex];
   }

   //! opens savegame for loading
   Savegame LoadSavegame(unsigned int uiIndex, bool bStoreImage=false);

   //! opens savegame for saving
   Savegame SaveSavegame(SavegameInfo info, unsigned int uiIndex = unsigned(-1));

   //! returns true when a quicksave savegame is available
   bool IsQuicksaveAvail() const;

   //! returns quicksave savegame for loading
   Savegame LoadQuicksaveSavegame() const
   {
      UaAssert(true == IsQuicksaveAvail());
      std::string strQuicksaveName = GetQuicksaveFilename();
      return Savegame(strQuicksaveName);
   }

   //! returns quicksave savegame for saving
   Savegame SaveQuicksaveSavegame(SavegameInfo info);

   //! sets screenshot for next savegame to be saved
   void SetSaveScreenshot(unsigned int uiXRes, unsigned int uiYRes,
      const std::vector<Uint32>& vecImageRGBA)
   {
      m_uiImageXRes = uiXRes;
      m_uiImageYRes = uiYRes;
      m_vecImageSavegame = vecImageRGBA;
   }

protected:
   //! returns filename of quicksave savegame
   std::string GetQuicksaveFilename() const;

protected:
   //! savegame folder name
   std::string m_strSavegameFolder;

   //! game prefix of currently running game
   std::string m_strGamePrefix;

   //! list of all current savegames
   std::vector<std::string> m_vecSavegames;

   // savegame image resolution
   unsigned int m_uiImageXRes;   //!< image x res
   unsigned int m_uiImageYRes;   //!< image y res

   //! savegame image in RGBA format
   std::vector<Uint32> m_vecImageSavegame;
};

} // namespace Base

#endif
