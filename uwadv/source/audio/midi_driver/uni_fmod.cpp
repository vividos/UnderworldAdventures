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

   universal midi fmod driver implementation

*/

// neded includes
#include "uni_fmod.h"

// linker options
#pragma comment(lib, "fmodvc.lib")


// uni_fmod_driver methods

uni_fmod_driver::uni_fmod_driver()
{
   mod = NULL;
   FSOUND_Init(44010,2,0);
}

uni_fmod_driver::~uni_fmod_driver()
{
   stop_track();
   FSOUND_Close();
}

void uni_fmod_driver::start_track(XMIDIEventList *eventlist, bool repeat)
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

void uni_fmod_driver::stop_track()
{
   if (mod!=NULL)
   {
      // stop track and clean up
      FMUSIC_StopSong(mod);
      FMUSIC_FreeSong(mod);
      mod=NULL;
   }
}

const char *uni_fmod_driver::copyright()
{
   return "FMOD midi driver for Underworld Adventures";
}
