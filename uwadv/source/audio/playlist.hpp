//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2006,2019 Michael Fink
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
/// \file playlist.hpp
/// \brief music playlist
//
#pragma once

#include <string>
#include <vector>

namespace Base
{
   class Settings;
}

namespace Audio
{
   /// \brief playlist class
   class Playlist
   {
   public:
      /// ctor; empty playlist
      Playlist() {}
      /// ctor; loads playlist
      Playlist(const Base::Settings& settings, Base::SDL_RWopsPtr rwops);

      /// returns number of tracks in playlist
      unsigned int GetCount() const { return m_playlist.size(); }

      /// returns track per number
      std::string GetPlaylistTrack(unsigned int trackIndex) const;

   private:
      /// playlist
      std::vector<std::string> m_playlist;
   };

} // namespace Audio
