/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2005 Jim Dovey

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
/*! \file osx_midiout.h

   \brief midi driver for Mac OS X

*/
//! \ingroup audio

//@{

#ifndef ua_osx_midiout_h_
#define ua_osx_midiout_h_

#ifdef __MACH__

#include "../midi.hpp"

#include <SDL/SDL.h>
#include <AudioUnit/AudioUnit.h>
#include <pthread.h>

//! Mac OS X midi driver
/*! CoreAudio midi driver; parses MIDI events & plays through the
    OS X CoreAudio routines.

    Implementation is based on 'win_midiout' files.
*/
class ua_osx_midiout : virtual public ua_midi_driver
{
public:
   virtual void start_track( XMIDIEventList *, bool repeat );
   virtual void start_sfx( XMIDIEventList * );
   virtual void stop_track( void );
   virtual void stop_sfx( void );
   virtual bool is_playing( void );
   virtual const char *copyright( void );

   ua_osx_midiout();
   virtual ~ua_osx_midiout();

   //! initializes driver
   virtual bool init_driver();

private:

   struct mid_data
   {
      XMIDIEventList *  list;
      bool              repeat;
   };

   static const unsigned short   centre_value;
   static const unsigned char    fine_value;
   static const unsigned char    coarse_value;
   static const unsigned short   combined_value;

   AudioUnit   music_device;
   AudioUnit   output;

   pthread_t   play_thread;

   // thread communications - all 32-bit values for use with atomic routines
   Uint32      is_available;
   Uint32      playing;
   Uint32      s_playing;
   Uint32      thread_com;
   Uint32      sfx_com;

   mid_data *  thread_data;
   mid_data *  sfx_data;

   mid_data    data;
   mid_data    sdata;

   static void* thread_start( void * data );

   void init_device();
   void thread_main();
   void thread_play();
   void reset_channel( int i );

   // Clock - supposedly microseconds, but Win32 GetTickCount() returns milliseconds,
   //  and so does SDL_GetTicks() - so I'll just use SDL_GetTicks() here
   // If time is really critical I can always use the mach nanosecond clock.
   Uint32      start;
   Uint32      sfx_start;

   inline void osxInitClock()
   { start = SDL_GetTicks() * 6; }

   inline void osxAddOffset( unsigned long offset )
   { start += offset; }

   inline unsigned long osxGetTime()
   { return ( ( SDL_GetTicks() * 6 ) - start ); }

   inline unsigned long osxGetStart()
   { return start; }

   inline unsigned long osxGetRealTime()
   { return ( SDL_GetTicks() * 6 ); }

   inline void osxInitSFXClock()
   { sfx_start = SDL_GetTicks() * 6; }

   inline void osxAddSFXOffset( unsigned long offset )
   { sfx_start += offset; }

   inline unsigned long osxGetSFXTime()
   { return ( ( SDL_GetTicks() * 6 ) - start ); }

   inline unsigned long osxGetSFXStart()
   { return sfx_start; }
};

#endif   /* __MACH__ */

#endif

//@}
