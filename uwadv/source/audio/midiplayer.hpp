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
/*! \file midiplayer.hpp

   \brief midi playback class and driver

*/

// include guard
#ifndef uwadv_audio_midiplayer_hpp_
#define uwadv_audio_midiplayer_hpp_

// needed includes
#include <memory> // for std::auto_ptr

class MidiDriver;
class XMidiFile;

namespace Base
{
   class Settings;
}

namespace Audio
{

//! Midi file player
/*! Plays back midi stored in .xmi and .mid files. */
class MidiPlayer
{
public:
   //! ctor
   MidiPlayer(const Base::Settings& settings);
   //! dtor
   ~MidiPlayer();

   //! starts playing midi file, with track number
   void PlayFile(Base::SDL_RWopsPtr rwops, bool bRepeat=false);

   //! stops playing
   void Stop();

protected:
   //! midi driver to be used
   std::auto_ptr<MidiDriver> m_apDriver;

   //! .xmi file to be played back
   std::auto_ptr<XMidiFile> m_apXMidiFile;
};

} // namespace Audio

#endif
