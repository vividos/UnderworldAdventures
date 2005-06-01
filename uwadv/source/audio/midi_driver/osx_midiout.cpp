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
/*! \file osx_midiout.cpp

   \brief Mac OS X midi driver implementation

*/

// only compile for Mac OS X
#ifdef __MACH__

#include "common.hpp"
#include "osx_midiout.h"
#include "../xmidi.hpp"
#include "atomic.h"
#include <iostream>

// constants
enum
{
   kThreadCommandExit      = 0xffffffff,
   kThreadCommandReady     = 0,
   kThreadCommandPlay,
   kThreadCommandStop,
   kThreadCommandInit,
   kThreadCommandInitFailed

};

// class static constants
const unsigned short ua_osx_midiout::centre_value   = 0x200;
const unsigned char  ua_osx_midiout::fine_value     = centre_value & 0x7F;
const unsigned char  ua_osx_midiout::coarse_value   = centre_value >> 7;
const unsigned short ua_osx_midiout::combined_value = (coarse_value << 8) | fine_value;

#define DO_SMP_TEST     0

#define ua_false  0ULL
#define ua_true   1ULL

ua_osx_midiout::ua_osx_midiout() : music_device( NULL ), output( NULL )
{
   ua_fetch_and_store( &playing, ua_false );
   ua_fetch_and_store( &s_playing, ua_false );
   ua_fetch_and_store( &is_available, ua_false );
}

ua_osx_midiout::~ua_osx_midiout()
{
   if ( !is_available ) return;

   while ( thread_com != kThreadCommandReady ) usleep( 1000 );

   ua_fetch_and_store( &thread_com, kThreadCommandExit );

   int count = 0;
   while ( count < 100 )
   {
      // when thread terminates cleanly, it'll set this to NULL
      if ( play_thread != NULL )
         usleep( 1000 );
      else
         break;

      count++;
   }

   // if it still didn't terminate, just kill it
   // use fetch & store to atomically unset thread id variable prior to cancel
   if ( ( count == 100 ) && ( is_available ) )
      pthread_cancel( ( pthread_t ) ua_fetch_and_store( &play_thread, NULL ) );

   ua_fetch_and_store( &is_available, ua_false );
}

bool ua_osx_midiout::init_driver()
{
   init_device();
   return ( thread_com != kThreadCommandInitFailed );
}

void ua_osx_midiout::init_device()
{
   // open the thread
   ua_fetch_and_store( &thread_com, kThreadCommandInit );

   // create the thread itself
   if ( pthread_create( &play_thread, NULL, thread_start, this ) == -1 )
   {
      // failed to create thread
      ua_fetch_and_store( &thread_com, kThreadCommandInitFailed );
   }
   else
   {
      while ( thread_com == kThreadCommandInit ) 
         usleep( 1000 );
   }
}

void * ua_osx_midiout::thread_start( void * data )
{
   ua_osx_midiout * ptr = static_cast<ua_osx_midiout *>(data);
   ptr->thread_main();
   return ( NULL );
}

void ua_osx_midiout::thread_main()
{
   thread_data = NULL;
   ua_fetch_and_store( &playing, ua_false );
   ua_fetch_and_store( &s_playing, ua_false );

   OSStatus err;
   AudioUnitConnection connection;

   // open the music device
   music_device = ( AudioUnit ) OpenDefaultComponent( kAudioUnitComponentType,
                                                      kAudioUnitSubType_MusicDevice );
   if ( music_device == NULL )
   {
      std::cerr << "Unable to open music device" << std::endl;
      ua_fetch_and_store( &thread_com, kThreadCommandInitFailed );
      return;
   }

   // open the output unit
   output = ( AudioUnit ) OpenDefaultComponent( kAudioUnitComponentType,
                                                kAudioUnitSubType_Output );
   if ( output == NULL )
   {
      std::cerr << "Unable to open output device" << std::endl;
      ua_fetch_and_store( &thread_com, kThreadCommandInitFailed );

      // don't leave the music device open...
      CloseComponent( music_device );

      return;
   }

   // connect the units
   connection.sourceAudioUnit = music_device;
   connection.sourceOutputNumber = 0;
   connection.destInputNumber = 0;

   err = AudioUnitSetProperty( output, kAudioUnitProperty_MakeConnection,
                               kAudioUnitScope_Input, 0, ( void * ) &connection,
                               sizeof( AudioUnitConnection ) );

   // initialize the units
   AudioUnitInitialize( music_device );
   AudioUnitInitialize( output );

   // start the output
   AudioOutputUnitStart( output );

   ua_fetch_and_store( &is_available, ua_true );

   // raise our thread priority
   // FIXME fiddly, will code this in a bit

   ua_fetch_and_store( &thread_com, kThreadCommandReady );
   ua_fetch_and_store( &sfx_com, kThreadCommandReady );

   thread_play();

   ua_fetch_and_store( &is_available, ua_false );

   AudioOutputUnitStop( output );

   CloseComponent( output );
   CloseComponent( music_device );

   ua_fetch_and_store( &play_thread, NULL );
}

void ua_osx_midiout::thread_play()
{
   int               repeat      = false;
   Uint32            aim         = 0;
   Sint32            diff        = 0;
   Uint32            last_tick   = 0;
   XMIDIEventList *  evntlist    = NULL;
   midi_event *      event       = NULL;
   NoteStack         notes_on;
   midi_event *      note        = NULL;

   //
   // Xmidi Looping
   //

   // The for loop event
   midi_event *      loop_event[ XMIDI_MAX_FOR_LOOP_COUNT ];

   // The number of times left that we can loop
   int               loop_count[ XMIDI_MAX_FOR_LOOP_COUNT ];

   // The level of the loop we're currently in
   int               loop_num    = -1;

   int               s_track     = 0;
   Uint32            s_aim       = 0;
   Sint32            s_diff      = 0;
   Uint32            s_last_tick = 0;
   NoteStack         s_notes_on;
   XMIDIEventList *  s_evntlist  = NULL;
   midi_event *      s_event     = NULL;

   // play while there isn't a message waiting
   while ( 1 )
   {
      if ( ( thread_com == kThreadCommandExit ) && ( !playing ) && ( !s_playing ) )
         break;

      if ( thread_com == kThreadCommandStop )
      {
         ua_fetch_and_store( &playing, ua_false );
         ua_fetch_and_store( &thread_com, kThreadCommandReady );

         // stop playing any active notes
         while ( note = notes_on.Pop() )
            MusicDeviceMIDIEvent( music_device, note->status, note->data[ 0 ], 0, 0 );

         // clean up
         //for ( int i = 0; i < 16; i++ ) reset_channel( i );

         AudioUnitReset( music_device, kAudioUnitScope_Input, NULL );

         if ( evntlist )
            evntlist->DecrementCounter();

         evntlist = NULL;
         event = NULL;

         // If stop was requested, we are ready to receive another song
         loop_num = -1;

         osxInitClock();
         last_tick = 0;
      }

      // Handle any note-off events here
      while ( note = notes_on.PopTime( osxGetRealTime() ) )
         MusicDeviceMIDIEvent( music_device, note->status, note->data[ 0 ], 0, 0 );

      while ( note = s_notes_on.PopTime( osxGetRealTime() ) )
         MusicDeviceMIDIEvent( music_device, note->status, note->data[ 0 ], 0, 0 );

      while ( ( event ) && ( thread_com != kThreadCommandStop ) )
      {
         aim = ( event->time - last_tick ) * 50;
         diff = aim - osxGetTime();

         if ( diff > 0 )
            break;

         last_tick = event->time;
         osxAddOffset( aim );

         // XMIDI For Loop
         if ( ( ( event->status >> 4 ) == MIDI_STATUS_CONTROLLER ) &&
              ( event->data[ 0 ] == XMIDI_CONTROLLER_FOR_LOOP ) )
         {
            if ( loop_num < XMIDI_MAX_FOR_LOOP_COUNT )
               loop_num++;

            loop_count[ loop_num ] = event->data[ 1 ];
            loop_event[ loop_num ] = event;
         }
         // XMIDI Next/Break
         else if ( ( ( event->status >> 4 ) == MIDI_STATUS_CONTROLLER ) &&
                   ( event->data[ 0 ] == XMIDI_CONTROLLER_NEXT_BREAK ) )
         {
            if ( loop_num != -1 )
            {
               if ( event->data[ 1 ] < 64 )
                  loop_num--;
            }

            event = NULL;
         }
         // Not SysEx
         else if ( event->status < 0xF0 )
         {
            int type = event->status >> 4;

            if ( ( ( type != MIDI_STATUS_NOTE_ON ) || ( event->data[ 1 ] ) ) &&
                 ( type != MIDI_STATUS_NOTE_OFF ) )
            {
               if ( type == MIDI_STATUS_NOTE_ON )
               {
                  notes_on.Remove( event );
                  notes_on.Push( event, event->duration * 50 + osxGetStart() );
               }

               MusicDeviceMIDIEvent( music_device, event->status, event->data[ 0 ], 
                                     event->data[ 1 ], 0 );
            }
         }
         // SysEx
         /*else
         {
            MusicDeviceSysEx( music_device, event->buffer, event->len );
         }*/

         if ( event )
            event = event->next;

         if ( ( !event ) || ( thread_com != kThreadCommandReady ) )
         {
            bool clean = !repeat || ( thread_com != kThreadCommandReady ) || ( last_tick == 0 );

            if ( clean )
            {
               ua_fetch_and_store( &playing, ua_false );
               if ( thread_com == kThreadCommandStop )
                  ua_fetch_and_store( &thread_com, kThreadCommandReady );

               // turn off any active notes
               while ( note = notes_on.Pop() )
                  MusicDeviceMIDIEvent( music_device, note->status, note->data[ 0 ], 0, 0 );

               // clean up
               //for ( int i = 0; i < 16; i++ ) reset_channel( i );

               AudioUnitReset( music_device, kAudioUnitScope_Input, NULL );

               if ( evntlist )
                  evntlist->DecrementCounter();
               evntlist = NULL;
               event = NULL;

               loop_num = -1;
               osxInitClock();
            }

            last_tick = 0;

            if ( evntlist )
            {
               if ( loop_num == -1 )
               {
                  event = evntlist->events;
               }
               else
               {
                  event = loop_event[ loop_num ]->next;
                  last_tick = loop_event[ loop_num ]->time;

                  if ( loop_count[ loop_num ] )
                     if ( !--loop_count[ loop_num ] )
                        loop_num--;
               }
            }
         }
      }

      // Got issued a music play command
      // set up the music playing routine
      if ( thread_com == kThreadCommandPlay )
      {
         // Lift notes here
         while ( note = notes_on.Pop() )
            MusicDeviceMIDIEvent( music_device, note->status, note->data[ 0 ], 0, 0 );

         //for ( int i = 0; i < 16; i++ ) reset_channel( i );

         AudioUnitReset( music_device, kAudioUnitScope_Input, NULL );

         if ( evntlist )
            evntlist->DecrementCounter();
         evntlist = NULL;
         event = NULL;

         ua_fetch_and_store( &playing, ua_false );

         // make sure that data exists
         while ( !thread_data )
            usleep( 1000 );

         evntlist = thread_data->list;
         repeat = thread_data->repeat;

         ua_fetch_and_store( &thread_data, NULL );
         ua_fetch_and_store( &thread_com, kThreadCommandReady );

         if ( evntlist )
            event = evntlist->events;
         else
            event = NULL;

         last_tick = 0;
         osxInitClock();

         // Reset XMIDI Looping
         loop_num = -1;

         ua_fetch_and_store( &playing, ua_true );
      }

      if ( s_event )
      {
         s_aim = ( s_event->time - s_last_tick ) * 50;
         s_diff = s_aim - osxGetSFXTime();
      }
      else
      {
         s_diff = 1;
      }

      if ( s_diff <= 0 )
      {
         s_last_tick = s_event->time;
         osxAddSFXOffset( s_aim );

         // Not SysEx
         if ( ( s_event->status >> 4 ) != MIDI_STATUS_SYSEX )
         {
            int type = s_event->status >> 4;

            if ( ( ( type != MIDI_STATUS_NOTE_ON ) || ( s_event->data[ 1 ] ) ) &&
                 ( type != MIDI_STATUS_NOTE_OFF ) )
            {
               if ( type == MIDI_STATUS_NOTE_ON )
               {
                  s_notes_on.Remove( s_event );
                  s_notes_on.Push( s_event, s_event->duration * 50 + osxGetSFXStart() );
               }

               MusicDeviceMIDIEvent( music_device, s_event->status, s_event->data[ 0 ],
                                     s_event->data[ 1 ], 0 );
            }

            s_track |= 1 << ( s_event->status & 0x0F );
         }

         s_event = s_event->next;
      }

      if ( ( s_evntlist ) &&
           ( ( !s_event ) || 
             ( thread_com == kThreadCommandExit ) ||
             ( sfx_com != kThreadCommandReady ) ) )
      {
         // lift any playing notes
         while ( note = s_notes_on.Pop() )
            MusicDeviceMIDIEvent( music_device, note->status, note->data[ 0 ], 0, 0 );

         // reset the played tracks
         for ( int i = 0; i< 16; i++ )
            if ( ( s_track >> i ) & 1 )
               reset_channel( i );

         s_evntlist->DecrementCounter();
         s_evntlist = NULL;
         s_event = NULL;
         ua_fetch_and_store( &s_playing, ua_false );

         if ( sfx_com != kThreadCommandPlay )
            ua_fetch_and_store( &sfx_com, kThreadCommandReady );
      }

      // sound effect play command
      if ( ( !s_evntlist ) && ( sfx_com == kThreadCommandPlay ) )
      {
         // lift any notes still playing
         while ( note = s_notes_on.Pop() )
            MusicDeviceMIDIEvent( music_device, note->status, note->data[ 0 ], 0, 0 );

         // make sure that the data exists
         while ( !sfx_data )
            usleep( 1000 );

         s_evntlist = sfx_data->list;

         ua_fetch_and_store( &sfx_data, NULL );
         ua_fetch_and_store( &sfx_com, kThreadCommandReady );

         if ( s_evntlist )
            s_event = s_evntlist->events;
         else
            s_event = NULL;

         s_last_tick = 0;
         osxInitSFXClock();

         ua_fetch_and_store( &s_playing, ua_true );

         // reset the track counter
         s_track = 0;
      }

      if ( event )
      {
         aim = ( event->time - last_tick ) * 50;
         diff = aim - osxGetTime();
      }
      else
      {
         diff = 6;
      }

      if ( s_event )
      {
         s_aim = ( s_event->time - s_last_tick ) * 50;
         s_diff = s_aim - osxGetSFXTime();
      }
      else
      {
         s_diff = 6;
      }

      // if there's nothing due in the next millisecond, sleep
      if ( ( diff > 5 ) && ( s_diff > 5 ) )
         usleep( 1000 );
   }

   // lift any remaining notes
   while ( note = notes_on.Pop() )
      MusicDeviceMIDIEvent( music_device, note->status, note->data[ 0 ], 0, 0 );

   while ( note = s_notes_on.Pop() )
      MusicDeviceMIDIEvent( music_device, note->status, note->data[ 0 ], 0, 0 );

   if ( evntlist )
      evntlist->DecrementCounter();
   evntlist = NULL;

   if ( s_evntlist )
      s_evntlist->DecrementCounter();
   s_evntlist = NULL;

   //for ( int i = 0; i < 16; i++ ) reset_channel( i );
   AudioUnitReset( music_device, kAudioUnitScope_Input, NULL );
}

void ua_osx_midiout::reset_channel( int i )
{
   // Pitch wheel
   MusicDeviceMIDIEvent( music_device, i, MIDI_STATUS_PITCH_WHEEL, combined_value, 0 );

   // All controllers off
   MusicDeviceMIDIEvent( music_device, i, MIDI_STATUS_CONTROLLER, 121, 0 );

   // All notes off
   MusicDeviceMIDIEvent( music_device, i, MIDI_STATUS_CONTROLLER, 123, 0 );

   // Bank Select
   MusicDeviceMIDIEvent( music_device, i, MIDI_STATUS_PROG_CHANGE, 0, 0 );
   MusicDeviceMIDIEvent( music_device, i, MIDI_STATUS_CONTROLLER, 0, 0 );
   MusicDeviceMIDIEvent( music_device, i, MIDI_STATUS_CONTROLLER, 32, 0 );

   // Modulation Wheel
   MusicDeviceMIDIEvent( music_device, i, MIDI_STATUS_CONTROLLER, 1, coarse_value );
   MusicDeviceMIDIEvent( music_device, i, MIDI_STATUS_CONTROLLER, 33, fine_value );

   // Volume
   MusicDeviceMIDIEvent( music_device, i, MIDI_STATUS_CONTROLLER, 7, coarse_value );
   MusicDeviceMIDIEvent( music_device, i, MIDI_STATUS_CONTROLLER, 39, fine_value );

   // Pan
   MusicDeviceMIDIEvent( music_device, i, MIDI_STATUS_CONTROLLER, 8, coarse_value );
   MusicDeviceMIDIEvent( music_device, i, MIDI_STATUS_CONTROLLER, 40, fine_value );

   // Balance
   MusicDeviceMIDIEvent( music_device, i, MIDI_STATUS_CONTROLLER, 10, coarse_value );
   MusicDeviceMIDIEvent( music_device, i, MIDI_STATUS_CONTROLLER, 42, fine_value );

   // Effects (Reverb)
   MusicDeviceMIDIEvent( music_device, i, MIDI_STATUS_CONTROLLER, 91, 0 );

   // Chrous
   MusicDeviceMIDIEvent( music_device, i, MIDI_STATUS_CONTROLLER, 93, 0 );
}

void ua_osx_midiout::start_track( XMIDIEventList *xmidi, bool repeat )
{
   if ( !is_available )
      init_device();

   if ( !is_available )
      return;

   while ( thread_com != kThreadCommandReady )
      usleep( 1000 );

   xmidi->IncrementCounter();
   data.list = xmidi;
   data.repeat = repeat;

   ua_fetch_and_store( &thread_data, &data );
   ua_fetch_and_store( &thread_com, kThreadCommandPlay );
}

void ua_osx_midiout::start_sfx( XMIDIEventList *xmidi )
{
   if ( !is_available )
      init_device();

   if ( !is_available )
      return;

   while ( sfx_com != kThreadCommandReady )
      usleep( 1000 );

   xmidi->IncrementCounter();
   sdata.list = xmidi;

   ua_fetch_and_store( &sfx_data, &sdata );
   ua_fetch_and_store( &sfx_com, kThreadCommandPlay );
}

void ua_osx_midiout::stop_track()
{
   if ( !is_available )
      return;

   if ( !playing )
      return;

   while ( thread_com != kThreadCommandReady )
      usleep( 1000 );

   ua_fetch_and_store( &thread_com, kThreadCommandStop );

   while ( thread_com != kThreadCommandReady )
      usleep( 1000 );
}

void ua_osx_midiout::stop_sfx()
{
   if ( !is_available )
      return;

   if ( !playing )
      return;

   while ( sfx_com != kThreadCommandReady )
      usleep( 1000 );

   ua_fetch_and_store( &sfx_com, kThreadCommandStop );
}

bool ua_osx_midiout::is_playing()
{
   return ( playing != ua_false );
}

const char * ua_osx_midiout::copyright()
{
   return ( "Mac OS X Midiout Midi Player for Underworld Adventures" );
}

#endif   /* __MACH__ */
