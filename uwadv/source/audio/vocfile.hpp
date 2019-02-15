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
/// \file vocfile.hpp
/// \brief .voc file
//
#pragma once

#include <vector>

namespace Audio
{
   /// \brief .voc file class
   /// \details Provides loading .voc files via Mix_LoadWAV_RW(). It resamples
   /// the sample data if necessary.
   class VoiceFile
   {
   public:
      /// ctor; loads .voc file
      VoiceFile(Base::SDL_RWopsPtr rwops);

      /// returns voice file data
      Base::SDL_RWopsPtr GetFileData() const;

   private:
      /// voice file data
      std::vector<Uint8> m_fileData;
   };

} // namespace Audio
