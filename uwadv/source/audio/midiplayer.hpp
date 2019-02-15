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
/// \file midiplayer.hpp
/// \brief midi playback class and driver
//
#pragma once

#include <memory>

class MidiDriver;
class XMidiFile;

namespace Base
{
   class Settings;
}

namespace Audio
{
   /// \brief Midi file player
   /// \details Plays back midi stored in .xmi and .mid files.
   class MidiPlayer
   {
   public:
      /// ctor
      MidiPlayer(const Base::Settings& settings);
      /// dtor
      ~MidiPlayer();

      /// starts playing midi file, with track number
      void PlayFile(Base::SDL_RWopsPtr rwops, bool repeat = false);

      /// stops playing
      void Stop();

   protected:
      /// midi driver to be used
      std::unique_ptr<MidiDriver> m_midiDriver;

      /// .xmi file to be played back
      std::unique_ptr<XMidiFile> m_xmidiFile;
   };

} // namespace Audio
