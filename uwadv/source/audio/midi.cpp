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

   midi playback class implementation

*/

// needed includes
#include "common.hpp"
#include <cstdio>
#include "midi.hpp"

// driver includes
#ifdef WIN32
# include "midi_driver/win_midiout.h"
#endif


// ua_midi_player methods

bool ua_midi_player::init_driver()
{
   midi_driver = new Windows_MidiOut;
   init=true;
   return true;
}

void ua_midi_player::start_track(const char *filename, int num, bool repeat)
{
   if (!init)
      init_driver();

   if (filename==NULL || midi_driver==NULL)
      return;

   int music_conversion;
   music_conversion = XMIDI_CONVERT_MT32_TO_GM;
//   music_conversion = XMIDI_CONVERT_MT32_TO_GS;
//   music_conversion = XMIDI_CONVERT_MT32_TO_GS127;
//   music_conversion = XMIDI_CONVERT_NOCONVERSION;

   // load xmi file
   XMIDI midifile(music_conversion);

   if (!midifile.Load(filename))
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
