//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2019 Underworld Adventures Team
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
/// \file xmi2mid.cpp
/// \brief simple xmidi to midi file converter
/// this program converts the first track of an XMIDI file (*.xmi) to a midi
/// file (*.mid), using MT32 to GM conversion
//
#include "../../audio/midi/XMidiFile.h"
#include "../../audio/midi/XMidiEventList.h"
#include "../../audio/IODataSource.hpp"

int main(int argc, char* argv[])
{
   printf("xmi2mid - XMidi to MIDI converter\n\n");

   if (argc < 3)
   {
      printf("syntax: xmi2mid <xmi-file> <mid-file>\n");
      return 1;
   }

   int music_conversion;
   music_conversion = XMIDIFILE_CONVERT_MT32_TO_GM;
   //music_conversion = XMIDIFILE_CONVERT_MT32_TO_GS;
   //music_conversion = XMIDIFILE_CONVERT_MT32_TO_GS127;
   //music_conversion = XMIDIFILE_CONVERT_NOCONVERSION;

   // load xmi file
   Detail::InputDataSource inputSource(Base::MakeRWopsPtr(SDL_RWFromFile(argv[1], "rb")));

   XMidiFile midifile(&inputSource, music_conversion);

   XMidiEventList* eventlist = midifile.GetEventList(0);

   if (eventlist == NULL)
   {
      printf("could not create event list!\n");
      return 1;
   }

   Base::SDL_RWopsPtr rwops = Base::MakeRWopsPtr(SDL_RWFromFile(argv[2], "wb"));
   if (rwops == NULL)
   {
      printf("could not create output file %s\n", argv[2]);
      return 2;
   }

   Detail::OutputDataSource outSource(rwops);
   eventlist->write(&outSource);

   return 0;
}
