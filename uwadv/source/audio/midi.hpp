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

   midi playback class and driver

*/

// include guard
#ifndef __uwadv_midi_hpp_
#define __uwadv_midi_hpp_

// needed includes
#include "xmidi.hpp"


// classes

//! abstract midi driver class
class ua_midi_driver
{
public:
   ua_midi_driver(){}
   virtual ~ua_midi_driver(){}

   //! starts playing an event list
   virtual void start_track(XMIDIEventList *eventlist, bool repeat)=0;
   virtual void stop_track()=0;
// virtual bool is_playing(void)=0;

   //! returns a copyright string
   virtual const char *copyright()=0;
};


//! midi player class
class ua_midi_player
{
public:
   //! ctor
   ua_midi_player()
   {
      midi_driver = NULL; init=false;
   }

   //! dtor
   ~ua_midi_player()
   {
      delete midi_driver;
      midi_driver = NULL;
   }

   //! initializes appropriate midi driver
   bool init_driver();

   //! starts playing midi file, with track number
   void start_track(const char *filename, int num=0, bool repeat=false);

   //! stops playing
   void stop_track();

protected:
   //! indicates if drivers were init'ed
   bool init;

   //! midi driver to use
   ua_midi_driver *midi_driver;
};

#endif
