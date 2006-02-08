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
/*! \file playlist.cpp

   \brief music playlist implementation

*/

// needed includes
#include "audio.hpp"
#include "playlist.hpp"
#include "settings.hpp"
#include "textfile.hpp"
#include <SDL_RWops.h>

namespace Detail
{

//! loads playlist
/*! Loads the music soundtrack playlist. The file can
    be stored in the uadata resource file or in the equivalent path. The file
    can contain, among the placeholders recognized by
    ua_files_manager::replace_system_vars(), the placeholder %uw-path%
    that specifies the current uw path. \todo fix description
*/
class PlaylistLoader: public Base::NonCopyable
{
public:
   //! ctor
   /*! SDL_RWops ptr is automatically closed */
   PlaylistLoader(const Base::Settings& settings, SDL_RWops* rwops, std::vector<std::string> &vecPlaylist)
      : m_playlistFile(rwops),
        m_vecPlaylist(vecPlaylist),
        m_strUwPath(settings.GetString(Base::settingUnderworldPath)),
        m_strUaDataPath(settings.GetString(Base::settingUadataPath))
   {
   }

   //! loads playlist
   void Load();

private:
   //! text file to load playlist from
   Base::TextFile m_playlistFile;

   //! ref to playlist to load
   std::vector<std::string>& m_vecPlaylist;

   //! path to current underworld game
   std::string m_strUwPath;

   //! path to uadata files
   std::string m_strUaDataPath;
};

//! \todo trim spaces in lines
void PlaylistLoader::Load()
{
   long lFileLength = m_playlistFile.FileLength();

   std::string strLine;
   while (m_playlistFile.Tell() < lFileLength)
   {
      m_playlistFile.ReadLine(strLine);

      // remove comment lines
      std::string::size_type pos = strLine.find_first_of('#');
      if (pos != std::string::npos)
         strLine.erase(pos);

      // \todo trim spaces in lines
      if (strLine.size()==0)
         continue;

      // replace keywords

      // replace %uadata%
      while( std::string::npos != (pos = strLine.find("%uadata%") ) )
         strLine.replace(pos, 8, m_strUaDataPath.c_str());

      // replace %uw-path%
      while( std::string::npos != (pos = strLine.find("%uw-path%") ) )
         strLine.replace(pos, 9, m_strUwPath.c_str());

      m_vecPlaylist.push_back(strLine);
   }
}

} // namespace Detail

using Audio::Playlist;

// Playlist methods

/*! SDL_RWops ptr is automatically closed */
Playlist::Playlist(const Base::Settings& settings, SDL_RWops* rwops)
{
   Detail::PlaylistLoader loader(settings, rwops, m_vecPlaylist);
   loader.Load();
}

std::string Playlist::GetPlaylistTrack(unsigned int uiTrack) const
{
   UaAssert(uiTrack < m_vecPlaylist.size());
   return m_vecPlaylist[uiTrack];
}
