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
/*! \file uni_fmod.h

   universal midi fmod driver

*/

// include guard
#ifndef __uwadv_uni_fmod_h_
#define __uwadv_uni_fmod_h_

// needed includes
#include <fmod.h>
#include "../midi.hpp"


// classes

//! fmod midi driver
class uni_fmod_driver : virtual public ua_midi_driver
{
public:
   uni_fmod_driver();
   virtual ~uni_fmod_driver();

   //! starts playing an event list
   virtual void start_track(XMIDIEventList *eventlist, bool repeat);
   virtual void stop_track();
// virtual bool is_playing(void)=0;

   //! returns a copyright string
   virtual const char *copyright();

protected:
   //! FMOD module handle
   FMUSIC_MODULE *mod;
};

#endif
