/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

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
/*! \file audio.cpp

   \brief audio interface implementation

   audio interface implementation does an the interfacing with the audio part
   of SDL and the ua_midi_player class.

*/

// needed includes
#include "common.hpp"
#include "audio.hpp"
#include "midi.hpp"
#include "sdl_mixer.h"
#include <string>


// tables

//! table with all midi file names
const char *ua_audio_allmidis[] =
{
   "uw01.xmi", "uw02.xmi", "uw03.xmi", "uw04.xmi", "uw05.xmi",
   "uw06.xmi", "uw07.xmi", "uw10.xmi", "uw11.xmi", "uw12.xmi",
   "uw13.xmi", "uw15.xmi"
};

//! table with all midi file names, different versions (for awe cards?)
const char *ua_audio_allmidis_awe[] =
{
   "aw01.xmi", "aw02.xmi", "aw03.xmi", "aw04.xmi", "aw05.xmi",
   "aw06.xmi", "aw07.xmi", "aw10.xmi", "aw11.xmi", "aw12.xmi",
   "aw13.xmi", "aw15.xmi"
};


// classes

//! audio implementation class
class ua_audio_impl: public ua_audio_interface
{
public:
   //! ctor
   ua_audio_impl(){}

   //! initializes audio
   void init(ua_settings &settings)
   {
      // init mixer
      SDL_Init(SDL_INIT_AUDIO);
      Mix_OpenAudio(12000, MIX_DEFAULT_FORMAT, 2, 4096);
      Mix_ChannelFinished(mixer_channel_finished);

      midipl.init_player(settings);
      midipl.init_driver();
      uw1path = settings.uw1_path;
   }

   //! plays a sound; stops when finished
   void play_sound(const char *soundname)
   {
      // construct filename
      std::string vocname(uw1path);
      vocname.append("sound/");
      vocname.append(soundname);
      vocname.append(".voc");

      // start playing
      Mix_Chunk* mc;
      if (mc = Mix_LoadWAV(vocname.c_str()))
          Mix_PlayChannel(-1, mc, 0);
   }

   //! starts music playback
   void start_music(unsigned int music, bool repeat)
   {
      if (music>=SDL_TABLESIZE(ua_audio_allmidis)) return;

      std::string midiname(uw1path);
      midiname.append("sound/");
      midiname.append(ua_audio_allmidis[music]);
      midipl.start_track(midiname.c_str(),0,repeat);
   }

   //! stops music playback
   void stop_music()
   {
      midipl.stop_track();
   }

   //! dtor
   virtual ~ua_audio_impl()
   {
      Mix_CloseAudio();
      SDL_QuitSubSystem(SDL_INIT_AUDIO);
   }

protected:
   //! frees audio chunk when channel stops playing (callback function)
   static void mixer_channel_finished(int channel)
   {
      Mix_Chunk* mc;
      if (mc = Mix_GetChunk(channel))
          Mix_FreeChunk(mc);
   }

protected:
   //! midi player
   ua_midi_player midipl;

   //! path to underworld files
   std::string uw1path;
};

ua_audio_interface *ua_audio_interface::get_audio_interface()
{
   // creates a new audio object
   return new ua_audio_impl;
}
