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
/*! \file uni_fmod.cpp

   \brief universal midi fmod driver implementation

   to play a midi file, the driver extracts a midi file from the XMIDI event
   list and loads and plays it via the FMOD API calls.

*/

// needed includes
#include "common.hpp"

// only compile if FMOD support is wanted
#ifdef HAVE_FMOD_H

#include "uni_fmod.h"


// msvc win32 linker option
#pragma comment(lib, "fmodvc.lib")


// ua_uni_fmod_driver methods

ua_uni_fmod_driver::ua_uni_fmod_driver()
{
   mod = NULL;
}

ua_uni_fmod_driver::~ua_uni_fmod_driver()
{
   // clean up
   stop_track();
   FSOUND_Close();
}

bool ua_uni_fmod_driver::init_driver()
{
   // hmm, have to init sound part of FMOD to get midi to play
   FSOUND_Init(44100,2,0);
}

void ua_uni_fmod_driver::start_track(XMIDIEventList *eventlist, bool repeat)
{
   stop_track();

   // find out midi file length
   int length = eventlist->Write(NULL);

   // convert XMIDI event list to midi file, into memory buffer
   Uint8 *rawbuf = new Uint8[length];
   SDL_RWops *midibuf = SDL_RWFromMem(rawbuf,length);

   eventlist->Write(midibuf);

   // play the midi file
   mod = FMUSIC_LoadSongMemory(rawbuf,length);

   delete[] rawbuf;

   FMUSIC_PlaySong(mod);
}

void ua_uni_fmod_driver::stop_track()
{
   if (mod!=NULL)
   {
      // stop track and clean up
      FMUSIC_StopSong(mod);
      FMUSIC_FreeSong(mod);
      mod=NULL;
   }
}

const char *ua_uni_fmod_driver::copyright()
{
   return "FMOD midi driver for Underworld Adventures";
}

#endif
