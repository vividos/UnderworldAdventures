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
/*! \file midi.hpp

   midi playback class implementation.

   ua_midi_player::init_driver() checks for available midi drivers for each
   platform, ua_midi_player::start_track() handles all the XMIDI stuff.

*/

// needed includes
#include "common.hpp"
#include <cstdio>
#include "midi.hpp"

// driver includes
#ifdef WIN32
# include "midi_driver/win_midiout.h"
#endif

#ifdef HAVE_FMOD_H
#include "midi_driver/uni_fmod.h"
#endif

#ifdef HAVE_SDL_MIXER
#include "midi_driver/linux_sdl_mixer.h"
#endif


// global functions

// template function to try out midi driver
template <typename T>
void ua_try_midi_driver(ua_midi_driver *&mdrv)
{
   if (mdrv!=NULL) return;

   mdrv = new T;

   if (!mdrv->init_driver())
   {
      delete mdrv;
      mdrv = NULL;
   }
}


// ua_midi_player methods

void ua_midi_player::init_player(ua_settings &settings)
{
   // todo: retrieve these values from the settings

   music_conversion = XMIDI_CONVERT_MT32_TO_GM;
//   music_conversion = XMIDI_CONVERT_MT32_TO_GS;
//   music_conversion = XMIDI_CONVERT_MT32_TO_GS127;
//   music_conversion = XMIDI_CONVERT_NOCONVERSION;

   chorus_value = 64;
   reverb_value = 16;
}

bool ua_midi_player::init_driver()
{
   // test every midi driver available
   midi_driver = NULL;

#ifdef WIN32
   ua_try_midi_driver<Windows_MidiOut>(midi_driver);
#endif

#ifdef HAVE_FMOD_H
   ua_try_midi_driver<uni_fmod_driver>(midi_driver);
#endif

#ifdef HAVE_SDL_MIXER
   ua_try_midi_driver<sdl_mixer_driver>(midi_driver);
#endif

   if (midi_driver != NULL)
      ua_trace("audio: using midi driver \"%s\"\n",midi_driver->copyright());
   else
      ua_trace("audio: no midi driver found.\n");

   init=true;
   return midi_driver != NULL;
}

void ua_midi_player::start_track(const char *filename, int num, bool repeat)
{
   if (!init || filename==NULL || midi_driver==NULL)
      return;

   // load xmi file
   XMIDI midifile(music_conversion);

   if (!midifile.Load(filename,chorus_value,reverb_value))
      return;

   XMIDIEventList *eventlist = midifile.GetEventList(num);

   // start playing through midi driver; don't repeat
   if (eventlist!=NULL)
      midi_driver->start_track(eventlist,repeat);
}

void ua_midi_player::stop_track()
{
   if (!init || midi_driver==NULL)
      return;

   midi_driver->stop_track();
}
