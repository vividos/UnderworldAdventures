/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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
/*! \file midi_sdl_mixer.cpp

   \brief SDL_mixer midi driver implementation

*/

// needed includes
#include "common.hpp"
#include "midi_sdl_mixer.hpp"


// ua_sdl_mixer_midi_driver methods

ua_sdl_mixer_midi_driver::ua_sdl_mixer_midi_driver()
{
   mod = NULL;
}

ua_sdl_mixer_midi_driver::~ua_sdl_mixer_midi_driver()
{
   // clean up
   stop_track();
}

bool ua_sdl_mixer_midi_driver::init_driver()
{
   // we don't need to call Mix_OpenAudio() here, since the audio class
   // already did it
   return true;
}

/*! Starts playing back the midi track using SDL_mixer. Since there is no way
    to load midi data into SDL_mixer, we have to temporarily write out the
    file to disk. It can then loaded by a call to Mix_LoadMUS().
*/
void ua_sdl_mixer_midi_driver::start_track(XMIDIEventList* eventlist,
   bool repeat)
{
   stop_track();

   // convert XMIDI event list to midi file, into temp. file
   SDL_RWops* midifile = SDL_RWFromFile("uwadv.mid","wb");
   eventlist->Write(midifile);
   SDL_RWclose(midifile);

   // load the midi file
   mod = Mix_LoadMUS("uwadv.mid");

   // we can remove the file here again since SDL_mixer has loaded the file
   // or at least has an open handle to that file
   remove("uwadv.mid");

   if (mod == NULL)
      return;

   // play it
   Mix_PlayMusic(mod, repeat? -1 : 1);
}

void ua_sdl_mixer_midi_driver::stop_track()
{
   if (mod!=NULL)
   {
      // stop track and clean up
      Mix_HaltMusic();
      Mix_FreeMusic(mod);
      mod=NULL;
   }
}

const char* ua_sdl_mixer_midi_driver::copyright()
{
   return "SDL_mixer midi driver for Underworld Adventures";
}
