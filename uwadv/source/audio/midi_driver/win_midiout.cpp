/*
   Copyright (C) 2000, 2001, 2002  Ryan Nunn

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

   $Id$

*/
/*! \file win_midiout.cpp

   \brief windows midi driver

*/

// neded includes
#include "common.hpp"
#include "win_midiout.h"
#include "../xmidi.hpp"


// constants

#define W32MO_THREAD_COM_READY       0
#define W32MO_THREAD_COM_PLAY        1
#define W32MO_THREAD_COM_STOP        2
#define W32MO_THREAD_COM_INIT        3
#define W32MO_THREAD_COM_INIT_FAILED 4
#define W32MO_THREAD_COM_EXIT        -1


// ua_win_midiout static variables

const unsigned short ua_win_midiout::centre_value = 0x2000;
const unsigned char ua_win_midiout::fine_value = centre_value & 127;
const unsigned char ua_win_midiout::coarse_value = centre_value >> 7;
const unsigned short ua_win_midiout::combined_value = (coarse_value << 8) | fine_value;


//#define DO_SMP_TEST

#ifdef DO_SMP_TEST
#define giveinfo() std::cerr << __FILE__ << ":" << __LINE__ << std::endl; std::cerr.flush();
#else
#define giveinfo()
#endif


// ua_win_midiout methods

ua_win_midiout::ua_win_midiout() : dev_num(-1)
{
   giveinfo();
   InterlockedExchange (&playing, false);
   InterlockedExchange (&s_playing, false);
   InterlockedExchange (&is_available, false);
   giveinfo();
//   init_device();
   giveinfo();
}

ua_win_midiout::~ua_win_midiout()
{
   giveinfo();
   if (!is_available) return;

   giveinfo();
   while (thread_com != W32MO_THREAD_COM_READY) Sleep (1);
   
   giveinfo();
   InterlockedExchange (&thread_com, W32MO_THREAD_COM_EXIT);

   giveinfo();
   int count = 0;
   
   giveinfo();
   while (count < 100)
   {
      giveinfo();
      DWORD code;
      GetExitCodeThread (thread_handle, &code);
      
      giveinfo();
      // Wait 1 MS before trying again
      if (code == STILL_ACTIVE) Sleep (1);
      else break;
      giveinfo();
      
      count++;
   }

   // We waited a second and it still didn't terminate
   giveinfo();
   if (count == 100 && is_available)
      TerminateThread (thread_handle, 1);

   giveinfo();
   InterlockedExchange (&is_available, false);
   giveinfo();
}

bool ua_win_midiout::init_driver()
{
   init_device();
   return (thread_com != W32MO_THREAD_COM_INIT_FAILED);
}

void ua_win_midiout::init_device()
{
//   string s;

   // Opened, lets open the thread
   giveinfo();
   InterlockedExchange (&thread_com, W32MO_THREAD_COM_INIT);
   
   // Get Win32 Midi Device num
//   config->value("config/audio/midi/win32_device", dev_num, -1);
   dev_num = -1;

   giveinfo();
   thread_handle = (HANDLE*) CreateThread (NULL, 0, thread_start, this, 0, &thread_id);
   
   giveinfo();
   while (thread_com == W32MO_THREAD_COM_INIT) Sleep (1);

   // Set Win32 Midi Device num
//   config->set("config/audio/midi/win32_device", dev_num, true);
   
   giveinfo();
   if (thread_com == W32MO_THREAD_COM_INIT_FAILED)
      thread_com;//cerr << "Failure to initialize midi playing thread" << endl;
   giveinfo();
}

DWORD __stdcall ua_win_midiout::thread_start(void *data)
{
   giveinfo();
   ua_win_midiout *ptr=static_cast<ua_win_midiout *>(data);
   giveinfo();
   return ptr->thread_main();
}

DWORD ua_win_midiout::thread_main()
{
   int i;
   thread_data = NULL;
   giveinfo();
   InterlockedExchange (&playing, false);
   InterlockedExchange (&s_playing, false);

   giveinfo();

   // List all the midi devices.
   MIDIOUTCAPS caps;
   signed long dev_count = (signed long) midiOutGetNumDevs(); 
//   std::cout << dev_count << " Midi Devices Detected" << endl;
//   std::cout << "Listing midi devices:" << endl;

   for (i = -1; i < dev_count; i++)
   {
      midiOutGetDevCaps ((UINT) i, &caps, sizeof(caps));
//      std::cout << i << ": " << caps.szPname << endl;
   }

   if (dev_num < -1 || dev_num >= dev_count)
   {
//      std::cerr << "Warning Midi device in config is out of range." << endl;
      dev_num = -1;
   }
   midiOutGetDevCaps ((UINT) dev_num, &caps, sizeof(caps));
//   std::cout << "Using device " << dev_num << ": "<< caps.szPname << endl;

   UINT mmsys_err = midiOutOpen (&midi_port, dev_num, 0, 0, 0);

   giveinfo();
   if (mmsys_err != MMSYSERR_NOERROR)
   {
      char buf[512];

      giveinfo();
      buf[0]=0;
//      mciGetErrorStringA(mmsys_err, buf, 512);
//      cerr << "Unable to open device: " << buf << endl;
      giveinfo();
      InterlockedExchange (&thread_com, W32MO_THREAD_COM_INIT_FAILED);
      giveinfo();
      return 1;
   }
   giveinfo();
   InterlockedExchange (&is_available, true);
   
//   SetThreadPriority (thread_handle, THREAD_PRIORITY_HIGHEST);
   giveinfo();
   SetThreadPriority (thread_handle, THREAD_PRIORITY_TIME_CRITICAL);
   
   giveinfo();
   InterlockedExchange (&thread_com, W32MO_THREAD_COM_READY);
   InterlockedExchange (&sfx_com, W32MO_THREAD_COM_READY);

   giveinfo();
   thread_play();
   giveinfo();

   giveinfo();
   midiOutClose (midi_port);
   giveinfo();
   InterlockedExchange (&is_available, false);
   giveinfo();
   return 0;
}

void ua_win_midiout::thread_play ()
{
   int            repeat = false;
   Uint32         aim = 0;
   Sint32         diff = 0;
   Uint32         last_tick = 0;
   XMIDIEventList   *evntlist = NULL;
   midi_event      *event = NULL;
   NoteStack      notes_on;
   midi_event      *note = NULL;

   //
   // Xmidi Looping
   //

   // The for loop event
   midi_event   *loop_event[XMIDI_MAX_FOR_LOOP_COUNT];

   // The amount of times we have left that we can loop
   int      loop_count[XMIDI_MAX_FOR_LOOP_COUNT];

   // The level of the loop we are currently in
   int      loop_num = -1;      

   giveinfo();

   int            s_track = 0;
   Uint32         s_aim = 0;
   Sint32         s_diff = 0;
   Uint32         s_last_tick = 0;
   NoteStack      s_notes_on;
   XMIDIEventList   *s_evntlist = NULL;
   midi_event      *s_event = NULL;

   giveinfo();

   // Play while there isn't a message waiting
   while (1)
   {
      if (thread_com == W32MO_THREAD_COM_EXIT && !playing && !s_playing) break;
      
      if (thread_com == W32MO_THREAD_COM_STOP)
      {
         giveinfo();
         InterlockedExchange (&playing, FALSE);
         InterlockedExchange (&thread_com, W32MO_THREAD_COM_READY);

         // Handle note off's here
         while (note = notes_on.Pop())
            midiOutShortMsg (midi_port, note->status + (note->data[0] << 8));

         giveinfo();
          // Clean up
         for (int i = 0; i < 16; i++) reset_channel (i); 
         midiOutReset (midi_port);
         giveinfo();
         if (evntlist) evntlist->DecrementCounter();
         giveinfo();
         evntlist = NULL;
         event = NULL;
         giveinfo();

         // If stop was requested, we are ready to receive another song

         loop_num = -1;

         wmoInitClock ();
         last_tick = 0;
      }

      // Handle note off's here
      while (note = notes_on.PopTime(wmoGetRealTime()))
         midiOutShortMsg (midi_port, note->status + (note->data[0] << 8));

      while (note = s_notes_on.PopTime(wmoGetRealTime()))
         midiOutShortMsg (midi_port, note->status + (note->data[0] << 8));

      while (event && thread_com != W32MO_THREAD_COM_STOP)
      {
          aim = (event->time-last_tick)*50;
         diff = aim - wmoGetTime ();

         if (diff > 0) break;

         last_tick = event->time;
         wmoAddOffset(aim);
      
            // XMIDI For Loop
         if ((event->status >> 4) == MIDI_STATUS_CONTROLLER && event->data[0] == XMIDI_CONTROLLER_FOR_LOOP)
         {
            if (loop_num < XMIDI_MAX_FOR_LOOP_COUNT) loop_num++;

            loop_count[loop_num] = event->data[1];
            loop_event[loop_num] = event;

         }   // XMIDI Next/Break
         else if ((event->status >> 4) == MIDI_STATUS_CONTROLLER && event->data[0] == XMIDI_CONTROLLER_NEXT_BREAK)
         {
            if (loop_num != -1)
            {
               if (event->data[1] < 64)
               {
                  loop_num--;
               }
            }
            event = NULL;

         }    // Not SysEx
         else if (event->status < 0xF0)
         {
            int type = event->status >> 4;

            if ((type != MIDI_STATUS_NOTE_ON || event->data[1]) && type != MIDI_STATUS_NOTE_OFF) {
               if (type == MIDI_STATUS_NOTE_ON) {
                  notes_on.Remove(event);
                  notes_on.Push (event, event->duration * 50 + wmoGetStart());
               }

               midiOutShortMsg (midi_port, event->status + (event->data[0] << 8) + (event->data[1] << 16));
            }
         }
      
          if (event) event = event->next;
   
          if (!event || thread_com != W32MO_THREAD_COM_READY)
          {
            bool clean = !repeat || (thread_com != W32MO_THREAD_COM_READY) || last_tick == 0;

             if (clean)
             {
               InterlockedExchange (&playing, FALSE);
               if (thread_com == W32MO_THREAD_COM_STOP)
                  InterlockedExchange (&thread_com, W32MO_THREAD_COM_READY);

               // Handle note off's here
               while (note = notes_on.Pop())
                  midiOutShortMsg (midi_port, note->status + (note->data[0] << 8));

                // Clean up
               for (int i = 0; i < 16; i++) reset_channel (i); 
               midiOutReset (midi_port);
               if (evntlist) evntlist->DecrementCounter();
               evntlist = NULL;
               event = NULL;

               loop_num = -1;
               wmoInitClock ();
             }

            last_tick = 0;

            if (evntlist)
            {
                if (loop_num == -1) event = evntlist->events;
               else
               {
                  event = loop_event[loop_num]->next;
                  last_tick = loop_event[loop_num]->time;

                  if (loop_count[loop_num])
                     if (!--loop_count[loop_num])
                        loop_num--;
               }
            }
          }
      }


      // Got issued a music play command
      // set up the music playing routine
      if (thread_com == W32MO_THREAD_COM_PLAY)
      {
         // Handle note off's here
         while (note = notes_on.Pop())
            midiOutShortMsg (midi_port, note->status + (note->data[0] << 8));

         // Manual Reset since I don't trust midiOutReset()
         giveinfo();
         for (int i = 0; i < 16; i++) reset_channel (i);
         midiOutReset (midi_port);

         if (evntlist) evntlist->DecrementCounter();
         evntlist = NULL;
         event = NULL;
         InterlockedExchange (&playing, FALSE);

         // Make sure that the data exists
         giveinfo();
         while (!thread_data) Sleep(1);
         
         giveinfo();
         evntlist = thread_data->list;
         repeat = thread_data->repeat;

         giveinfo();
         InterlockedExchange ((LONG*) &thread_data, (LONG) NULL);
         giveinfo();
         InterlockedExchange (&thread_com, W32MO_THREAD_COM_READY);
         
         giveinfo();
         if (evntlist) event = evntlist->events;
         else event = 0;

         giveinfo();
         last_tick = 0;
         
         giveinfo();
         wmoInitClock ();
   
         // Reset XMIDI Looping
         loop_num = -1;

         giveinfo();
         InterlockedExchange (&playing, true);
      }

       if (s_event)
       {
          s_aim = (s_event->time-s_last_tick)*50;
         s_diff = s_aim - wmoGetSFXTime ();
       }
       else 
          s_diff = 1;
   
      if (s_diff <= 0)
      {
         s_last_tick = s_event->time;
         wmoAddSFXOffset(s_aim);
      
         // Not SysEx
         if ((s_event->status >> 4) != MIDI_STATUS_SYSEX)
         {
            int type = s_event->status >> 4;

            if ((type != MIDI_STATUS_NOTE_ON || s_event->data[1]) && type != MIDI_STATUS_NOTE_OFF) {
               if (type == MIDI_STATUS_NOTE_ON) {
                  s_notes_on.Remove(s_event);
                  s_notes_on.Push (s_event, s_event->duration * 50 + wmoGetSFXStart());
               }

               midiOutShortMsg (midi_port, s_event->status + (s_event->data[0] << 8) + (s_event->data[1] << 16));
            }
            s_track |= 1 << (s_event->status & 0xF);
         }

          s_event = s_event->next;
      }
       if (s_evntlist && (!s_event || thread_com == W32MO_THREAD_COM_EXIT || sfx_com != W32MO_THREAD_COM_READY))
      {
          // Play all the remaining note offs 
         while (note = s_notes_on.Pop())
            midiOutShortMsg (midi_port, note->status + (note->data[0] << 8));
          
          // Also reset the played tracks
         for (int i = 0; i < 16; i++) if ((s_track >> i)&1) reset_channel (i);

         s_evntlist->DecrementCounter();
         s_evntlist = NULL;
         s_event = NULL;
         InterlockedExchange (&s_playing, false);
         if (sfx_com != W32MO_THREAD_COM_PLAY) InterlockedExchange (&sfx_com, W32MO_THREAD_COM_READY);
      }

      // Got issued a sound effect play command
      // set up the sound effect playing routine
      if (!s_evntlist && sfx_com == W32MO_THREAD_COM_PLAY)
      {
         giveinfo();
//         cout << "Play sfx command" << endl;

          // Play all the remaining note offs 
         while (note = s_notes_on.Pop())
            midiOutShortMsg (midi_port, note->status + (note->data[0] << 8));

         // Make sure that the data exists
         while (!sfx_data) Sleep(1);
         
         giveinfo();
         s_evntlist = sfx_data->list;

         giveinfo();
         InterlockedExchange ((LONG*) &sfx_data, (LONG) NULL);
         InterlockedExchange (&sfx_com, W32MO_THREAD_COM_READY);
         giveinfo();
         
         if (s_evntlist) s_event = s_evntlist->events;
         else s_event = 0;

         giveinfo();
   
         s_last_tick = 0;
         
         giveinfo();
         wmoInitSFXClock ();

         giveinfo();
         InterlockedExchange (&s_playing, true);
         
         giveinfo();
         // Reset thet track counter
         s_track = 0;
      }

       if (event)
       {
          aim = (event->time-last_tick)*50;
         diff = aim - wmoGetTime ();
       }
       else 
          diff = 6;

       if (s_event)
       {
          s_aim = (s_event->time-s_last_tick)*50;
         s_diff = s_aim - wmoGetSFXTime ();
       }
       else 
          s_diff = 6;

      //std::cout << sfx_com << endl;

      if (diff > 5 && s_diff > 5) Sleep (1);
   }
   // Handle note off's here
   while (note = notes_on.Pop())
      midiOutShortMsg (midi_port, note->status + (note->data[0] << 8));

   // Play all the remaining note offs 
   while (note = s_notes_on.PopTime(wmoGetRealTime()))
      midiOutShortMsg (midi_port, note->status + (note->data[0] << 8));

   if (evntlist) evntlist->DecrementCounter();
   evntlist = NULL;
   if (s_evntlist) s_evntlist->DecrementCounter();
   s_evntlist = NULL;
   for (int i = 0; i < 16; i++) reset_channel (i); 
   midiOutReset (midi_port);
}

void ua_win_midiout::reset_channel (int i)
{
   // Pitch Wheel
   midiOutShortMsg (midi_port, i | (MIDI_STATUS_PITCH_WHEEL << 4) | (combined_value << 8));
   
   // All controllers off
   midiOutShortMsg (midi_port, i | (MIDI_STATUS_CONTROLLER << 4) | (121 << 8));

   // All notes off
   midiOutShortMsg (midi_port, i | (MIDI_STATUS_CONTROLLER << 4) | (123 << 8));

   // Bank Select
   midiOutShortMsg (midi_port, i | (MIDI_STATUS_PROG_CHANGE << 4) | (0 << 8));
   midiOutShortMsg (midi_port, i | (MIDI_STATUS_CONTROLLER << 4) | (0 << 8));
   midiOutShortMsg (midi_port, i | (MIDI_STATUS_CONTROLLER << 4) | (32 << 8));

   // Modulation Wheel
   midiOutShortMsg (midi_port, i | (MIDI_STATUS_CONTROLLER << 4) | (1 << 8) | (coarse_value << 16));
   midiOutShortMsg (midi_port, i | (MIDI_STATUS_CONTROLLER << 4) | (33 << 8) | (fine_value << 16));
   
   // Volume
   midiOutShortMsg (midi_port, i | (MIDI_STATUS_CONTROLLER << 4) | (7 << 8) | (coarse_value << 16));
   midiOutShortMsg (midi_port, i | (MIDI_STATUS_CONTROLLER << 4) | (39 << 8) | (fine_value << 16));

   // Pan
   midiOutShortMsg (midi_port, i | (MIDI_STATUS_CONTROLLER << 4) | (8 << 8) | (coarse_value << 16));
   midiOutShortMsg (midi_port, i | (MIDI_STATUS_CONTROLLER << 4) | (40 << 8) | (fine_value << 16));

   // Balance
   midiOutShortMsg (midi_port, i | (MIDI_STATUS_CONTROLLER << 4) | (10 << 8) | (coarse_value << 16));
   midiOutShortMsg (midi_port, i | (MIDI_STATUS_CONTROLLER << 4) | (42 << 8) | (fine_value << 16));

   // Effects (Reverb)
   midiOutShortMsg (midi_port, i | (MIDI_STATUS_CONTROLLER << 4) | (91 << 8));

   // Chorus
   midiOutShortMsg (midi_port, i | (MIDI_STATUS_CONTROLLER << 4) | (93 << 8));
}

void ua_win_midiout::start_track (XMIDIEventList *xmidi, bool repeat)
{
   giveinfo();
   if (!is_available)
      init_device();

   giveinfo();
   if (!is_available)
      return;
      
   giveinfo();
   while (thread_com != W32MO_THREAD_COM_READY) Sleep (1);
   
   giveinfo();
   xmidi->IncrementCounter();
   data.list = xmidi;
   data.repeat = repeat;
//   xmidi->Write("winmidi_out.mid");
   
   giveinfo();
   InterlockedExchange ((LONG*) &thread_data, (LONG) &data);
   giveinfo();
   InterlockedExchange (&thread_com, W32MO_THREAD_COM_PLAY);
   giveinfo();
}

void ua_win_midiout::start_sfx(XMIDIEventList *xmidi)
{
   giveinfo();
   if (!is_available)
      init_device();

   giveinfo();
   if (!is_available)
      return;
   
   giveinfo();
   while (sfx_com != W32MO_THREAD_COM_READY) Sleep (1);

   giveinfo();
   xmidi->IncrementCounter();
   sdata.list = xmidi;
   sdata.repeat;
   
   giveinfo();
   InterlockedExchange ((LONG*) &sfx_data, (LONG) &sdata);
   giveinfo();
   InterlockedExchange (&sfx_com, W32MO_THREAD_COM_PLAY);
   giveinfo();
}


void ua_win_midiout::stop_track(void)
{
   giveinfo();
   if (!is_available)
      return;

   giveinfo();
   if (!playing) return;

   giveinfo();
   while (thread_com != W32MO_THREAD_COM_READY) Sleep (1);
   giveinfo();
   InterlockedExchange (&thread_com, W32MO_THREAD_COM_STOP);
   giveinfo();
   while (thread_com != W32MO_THREAD_COM_READY) Sleep (1);
   giveinfo();
}

void ua_win_midiout::stop_sfx(void)
{
   giveinfo();
   if (!is_available)
      return;

   giveinfo();
   if (!s_playing) return;

   giveinfo();
   while (sfx_com != W32MO_THREAD_COM_READY) Sleep (1);
   giveinfo();
   InterlockedExchange (&sfx_com, W32MO_THREAD_COM_STOP);
   giveinfo();
}

bool ua_win_midiout::is_playing(void)
{
   giveinfo();
   return playing!=0;
}

const char *ua_win_midiout::copyright(void)
{
   giveinfo();
   return "Win32 Midiout Midi Player for Underworld Adventures";
}
