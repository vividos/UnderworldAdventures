/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

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

   \brief midi playback class and driver

   ua_midi_driver is a base class for a generic midi driver, so that the midi
   player only has to access one interface.

   ua_midi_player checks for available midi driver, extracts XMIDI data (via
   the XMIDI class) and passes the event list to the apropriate driver.

*/
//! \ingroup audio

//@{

// include guard
#ifndef uwadv_midi_hpp_
#define uwadv_midi_hpp_

// needed includes
#include "xmidi.hpp"
#include "settings.hpp"


// classes

//! abstract midi driver class
class ua_midi_driver
{
public:
   //! ctor
   ua_midi_driver(){}
   //! dtor
   virtual ~ua_midi_driver(){}

   //! initializes driver; returns true if driver can be used
   virtual bool init_driver()=0;

   //! starts playing an event list
   virtual void start_track(XMIDIEventList *eventlist, bool repeat)=0;

   //! stops track
   virtual void stop_track()=0;

   //! returns a copyright string
   virtual const char *copyright()=0;
};


//! midi player class
class ua_midi_player
{
public:
   //! ctor
   ua_midi_player():init(false), midi_driver(NULL)
   {
   }

   //! dtor
   ~ua_midi_player()
   {
      delete midi_driver;
      midi_driver = NULL;
   }

   //! initializes player
   void init_player(ua_settings &settings);

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

   //! midi device for the win32 midi driver
   int win32_midi_device;

   // music conversion type
   int music_conversion;

   //! chorus value; -1 means off
   int chorus_value;

   //! reverb value; -1 means off
   int reverb_value;
};


#endif
//@}
