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
/*! \file xmi2mid.cpp

   simple xmidi to midi file converter

   this program converts the first track of an XMIDI file (*.xmi) to a midi
   file (*.mid), using MT32 to GM conversion

*/

// needed includes
#include "../audio/xmidi.hpp"


int main(int argc, char* argv[])
{
   printf("xmidi to midi converter\n\n");

   if (argc<3)
   {
      printf("syntax: xmi2mid <xmi-file> <mid-file>\n");
      return 1;
   }

   int music_conversion;
   music_conversion = XMIDI_CONVERT_MT32_TO_GM;
//   music_conversion = XMIDI_CONVERT_MT32_TO_GS;
//   music_conversion = XMIDI_CONVERT_MT32_TO_GS127;
//   music_conversion = XMIDI_CONVERT_NOCONVERSION;

   // load xmi file
   XMIDI midifile(music_conversion);

   if (!midifile.Load(argv[1]))
   {
      printf("could not open file %s!\n",argv[1]);
      return 1;
   }

   XMIDIEventList *eventlist = midifile.GetEventList(0);

   if (eventlist!=NULL)
   {
      SDL_RWops *outf = SDL_RWFromFile(argv[2],"wb");

      eventlist->Write(outf);

      SDL_RWclose(outf);
      SDL_FreeRW(outf);
   }
   else
      printf("could not create event list!\n");

   return 0;
}
