/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Tels

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

*/
/*! \file linux_sdl_mixer.cpp

   linux midi driver; plays midis via the SDL_mixer audio library.
   available at http://www.libsdl.org/projects/SDL_mixer/

   works probably only on linux, needs the Timidity patches from
   http://www.libsdl.org/projects/SDL_mixer/timidity/timidity.tar.gz
   untar into /usr/local/lib/timidity

   midi driver implementation; to play a midi file, the driver
   extracts a midi file from the XMIDI event list and loads and plays it via
   the SDL_mixer calls.

*/

// needed includes
#include "common.hpp"
#include "linux_sdl_mixer.h"


// ua_sdl_mixer_driver methods

ua_sdl_mixer_driver::ua_sdl_mixer_driver()
{
   mod = NULL;
}

ua_sdl_mixer_driver::~ua_sdl_mixer_driver()
{
   // clean up
   stop_track();
//   Mix_CloseAudio();
}

bool ua_sdl_mixer_driver::init_driver()
{
   // we don't need to call Mix_OpenAudio() here, since the audio class
   // already did it
/*
   // 44 Khz, , 2 channels (stereo), chunksize 
   if ( Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) != 0)
      {
      // this might be a critical error...
      printf("Mix_OpenAudio: %s\n", Mix_GetError());
      } 
*/
   return true;
}

void ua_sdl_mixer_driver::start_track(XMIDIEventList *eventlist, bool repeat)
{
   stop_track();

   // find out midi file length
   int length = eventlist->Write(NULL);

   // convert XMIDI event list to midi file, into temp. file
   SDL_RWops *midifile = SDL_RWFromFile("uwadv.mid","wb");
   eventlist->Write(midifile);
   SDL_RWclose(midifile);

   // load the midi file
   mod = Mix_LoadMUS("uwadv.mid");

   remove("uwadv.mid");

   if (mod == NULL)
      return;

   // play it
   Mix_PlayMusic(mod, repeat? -1 : 1);
}

void ua_sdl_mixer_driver::stop_track()
{
   if (mod!=NULL)
   {
      // stop track and clean up
      Mix_HaltMusic();
      Mix_FreeMusic(mod);
      mod=NULL;
   }
}

const char *ua_sdl_mixer_driver::copyright()
{
   return "SDL_mixer midi driver for Underworld Adventures";
}
