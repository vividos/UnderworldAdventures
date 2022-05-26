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
/// \file Playlist.cpp
/// \brief music playlist implementation
//
#include "pch.hpp"
#include "Playlist.hpp"
#include "Settings.hpp"
#include "TextFile.hpp"
#include <SDL2/SDL_rwops.h>

namespace Detail
{
   /// \brief loads playlist
   /// Loads the music soundtrack playlist. The file is normally stored in the
   /// uadata resource zip file. The file can contain, among the placeholders
   /// recognized by ResourceManager, the placeholder %uw-path% that specifies
   /// the current uw path.
   class PlaylistLoader
   {
   public:
      /// ctor
      PlaylistLoader(const Base::Settings& settings, Base::SDL_RWopsPtr rwops, std::vector<std::string> &playlist)
         : m_playlistFile(rwops),
         m_playlist(playlist),
         m_underworldPath(settings.GetString(Base::settingUnderworldPath)),
         m_uadataPath(settings.GetString(Base::settingUadataPath))
      {
      }

      /// loads playlist
      void Load();

   private:
      /// deleted copy ctor
      PlaylistLoader(const PlaylistLoader&) = delete;

      /// deleted assignment operator
      PlaylistLoader& operator=(const PlaylistLoader&) = delete;

   private:
      /// text file to load playlist from
      Base::TextFile m_playlistFile;

      /// ref to playlist to load
      std::vector<std::string>& m_playlist;

      /// path to current underworld game
      std::string m_underworldPath;

      /// path to uadata files
      std::string m_uadataPath;
   };

   void PlaylistLoader::Load()
   {
      long fileLength = m_playlistFile.FileLength();

      std::string line;
      while (m_playlistFile.Tell() < fileLength)
      {
         m_playlistFile.ReadLine(line);

         // remove comment lines
         std::string::size_type pos = line.find_first_of('#');
         if (pos != std::string::npos)
            line.erase(pos);

         Base::String::TrimStart(line);
         Base::String::TrimEnd(line);

         if (line.empty())
            continue;

         m_playlist.push_back(line);
      }
   }

} // namespace Detail

using Audio::Playlist;

/// SDL_RWops ptr is automatically closed
Playlist::Playlist(const Base::Settings& settings, Base::SDL_RWopsPtr rwops)
{
   Detail::PlaylistLoader loader(settings, rwops, m_playlist);
   loader.Load();
}

std::string Playlist::GetPlaylistTrack(size_t trackIndex) const
{
   UaAssert(trackIndex < m_playlist.size());
   return m_playlist[trackIndex];
}
