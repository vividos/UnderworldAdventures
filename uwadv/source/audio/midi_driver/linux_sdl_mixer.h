/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Tels

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
/*! \file sdl_mixer.h

   universal midi driver; plays midis via the SDL_mixer audio library.
   available at http://www.libsdl.org/projects/SDL_mixer/

   works probably only on linux, needs the Timidity patches from
   http://www.libsdl.org/projects/SDL_mixer/timidity/timidity.tar.gz
   untar into /usr/local/lib/timidity

*/

// include guard
#ifndef __sdl_mixer_h_
#define __sdl_mixer_h_

// needed includes
#include "SDL_mixer.h"
#include "../midi.hpp"


// classes

//! SDL_mixer midi driver
class sdl_mixer_driver : virtual public ua_midi_driver
{
public:
   //! ctor
   sdl_mixer_driver();
   //! dtor
   virtual ~sdl_mixer_driver();

   //! initializes driver
   virtual bool init_driver();

   //! starts playing an event list
   virtual void start_track(XMIDIEventList *eventlist, bool repeat);
   virtual void stop_track();
// virtual bool is_playing(void)=0;

   //! returns a copyright string
   virtual const char *copyright();

protected:
   Mix_Music *mod;
};

#endif
