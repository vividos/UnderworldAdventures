/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2003 Underworld Adventures Team

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
/*! \file fading.hpp

   \brief fading class

   The fading helper class ua_fading_helper helps fading in or out screens,
   e.g. when one leaves an active screen. The class manages a value in the
   range 0 to 255 that can be used in a call to glColor3ub(). The init()
   method can be called multiple times. The tick() method (should be called
   every tick) returns true if the fading action is at its end. The fading
   value stays at it's value (either 0 or 255) after finishing fading.

*/

// include guard
#ifndef uwadv_fading_hpp_
#define uwadv_fading_hpp_


// classes

//! helper class for fading in/out screens
class ua_fading_helper
{
public:
   //! ctor
   ua_fading_helper():finished(true),fade_in(false),tickrate(20.0),fadetime(){}

   //! (re)initialize fading helper
   void init(bool fade_in, double tickrate, double fadetime);

   //! does a tick and returns true if the fading action is finished
   bool tick();

   //! returns fading color value [0..255]
   Uint8 get_fade_value();

protected:
   //! indicates if a fade-in is done
   bool fade_in;
   bool finished;

   double tickrate;
   double fadetime;
   unsigned int tickcount;
};


// inline methods

inline void ua_fading_helper::init(bool my_fade_in, double my_tickrate, double my_fadetime)
{
   fade_in = my_fade_in;
   tickrate = my_tickrate;
   fadetime = my_fadetime;
   tickcount = 0;
   finished = false;
}

inline bool ua_fading_helper::tick()
{
   if (!finished) tickcount++;

   return finished = tickcount >= fadetime * tickrate;
}

inline Uint8 ua_fading_helper::get_fade_value()
{
   if (finished) return fade_in ? 255 : 0;

   return fade_in ? Uint8(255*(double(tickcount) / (fadetime * tickrate))) :
      Uint8(255-255*(double(tickcount) / (fadetime * tickrate)));
}

#endif
