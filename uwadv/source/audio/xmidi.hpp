/*
   XMIDI driver to load and process xmi files
   Copyright (C) 2000  Ryan Nunn

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
/*! \file xmidi.hpp

   \brief XMIDI loading and processing

   functionaliy for loading and processing XMIDI (*.xmi) files that are used
   in many Ultima games.

*/

// include guard
#ifndef __uwadv_xmidi_hpp_
#define __uwadv_xmidi_hpp_

// needed includes
#include <SDL_types.h>
#include <SDL_rwops.h>


// constants

// Conversion types for Midi files
#define XMIDI_CONVERT_NOCONVERSION      0
#define XMIDI_CONVERT_MT32_TO_GM        1
#define XMIDI_CONVERT_MT32_TO_GS        2
#define XMIDI_CONVERT_MT32_TO_GS127     3
#define XMIDI_CONVERT_GS127_TO_GS       4

// Midi Status Bytes
#define MIDI_STATUS_NOTE_OFF     0x8
#define MIDI_STATUS_NOTE_ON      0x9
#define MIDI_STATUS_AFTERTOUCH   0xA
#define MIDI_STATUS_CONTROLLER   0xB
#define MIDI_STATUS_PROG_CHANGE  0xC
#define MIDI_STATUS_PRESSURE     0xD
#define MIDI_STATUS_PITCH_WHEEL  0xE
#define MIDI_STATUS_SYSEX        0xF

// XMIDI Controllers
#define XMIDI_CONTROLLER_FOR_LOOP     116
#define XMIDI_CONTROLLER_NEXT_BREAK   117

// Maximum number of for loops we'll allow (used by win_midiout)
#define XMIDI_MAX_FOR_LOOP_COUNT   128


// structs and classes

template <class T> class GammaTable;

struct midi_event
{
   int            time;
   unsigned char  status;

   unsigned char  data[2];

   Uint32         len;        // Length of SysEx Data
   unsigned char  *buffer;    // SysEx Data
   int            duration;   // Duration of note (120 Hz)
   midi_event     *next_note; // The next note on the stack
   Uint32         note_time;  // Time note stops playing (6000th of second)
   midi_event     *next;
};

class NoteStack
{
   midi_event     *notes;     // Top of the stack
   int            polyphony;
   int            max_polyphony;
public:

   NoteStack() : notes(0), polyphony(0), max_polyphony(0) { }

   //! Pops the top of the stack if its off_time is <= time (6000th of second)
   inline midi_event *PopTime(Uint32 time)
   {
      if (notes && notes->note_time <= time)
      {
         midi_event *note = notes;
         notes = note->next_note;
         note->next_note = 0;
         polyphony--;
         return note;
      }
      return 0;
   }

   //! Pops the top of the stack
   inline midi_event *Pop()
   {
      if (notes)
      {
         midi_event *note = notes;
         notes = note->next_note;
         note->next_note = 0;
         polyphony--;
         return note;
      }
      return 0;
   }

   //! Pops the top of the stack
   inline midi_event *Remove(midi_event *event)
   {
      midi_event *prev = 0;
      midi_event *note = notes;
      while (note)
      {
         if (note == event)
         {
            if (prev) prev->next_note = note->next_note;
            else notes = note->next_note;
            note->next_note = 0;
            polyphony--;
            return note;
         }
         prev = note;
         note = note->next_note;
      }
      return 0;
   }

   //! Finds the note that has same pitch and channel, and pops it
   inline midi_event *FindAndPop(midi_event *event)
   {
      midi_event *prev = 0;
      midi_event *note = notes;
      while (note) {

         if ((note->status & 0xF) == (event->status & 0xF) &&
            note->data[0] == event->data[0])
         {
            if (prev) prev->next_note = note->next_note;
            else notes = note->next_note;
            note->next_note = 0;
            polyphony--;
            return note;
         }
         prev = note;
         note = note->next_note;
      }
      return 0;
   }

   //! Pushes a note onto the top of the stack
   inline void Push(midi_event *event)
   {
      event->next_note = notes;
      notes = event;
      polyphony++;
      if (max_polyphony < polyphony) max_polyphony = polyphony;
   }

   inline void Push(midi_event *event, Uint32 time)
   {
      event->note_time = time;
      event->next_note = 0;

      polyphony++;
      if (max_polyphony < polyphony) max_polyphony = polyphony;

      if (!notes || time <= notes->note_time)
      {
         event->next_note = notes;
         notes = event;
      }
      else
      {
         midi_event *prev = notes;
         while (prev)
         {
            midi_event *note = prev->next_note;

            if (!note || time <= note->note_time)
            {
               event->next_note = note;
               prev->next_note = event;
               return;
            }
            prev = note;
         }
      }
   }

   inline int GetPolyphony(){ return polyphony; }
   inline int GetMaxPolyphony(){ return max_polyphony; }
};


class XMIDIEventList
{
public:
   midi_event *events;

   // Write methods

   //! writes out a MIDI file
   int Write(SDL_RWops *dest);

   //! Write a Conventional Variable Length Quantity
   int PutVLQ(SDL_RWops *dest, Uint32 value);

   //! Converts an event list to a MTrk
   Uint32 ConvertListToMTrk(SDL_RWops *dest);


   //! Increments the counter
   void IncrementCounter(){ counter++; }

   //! Decrement the counter and delete the event list, if possible
   void DecrementCounter();

private:
   int counter;
   static void DeleteEventList(midi_event *list);
};


class XMIDI
{
public:
   XMIDI(int pconvert);
   ~XMIDI();

   //! loads midi file
   bool Load(const char *filename, int reverb=-1, int chorus=-1);

   int number_of_tracks(){ return num_tracks; }

   //! External Event list functions
   XMIDIEventList *GetEventList(Uint32 track);

   // Not yet implimented
   // int apply_patch (int track, SDL_RWops *source);

private:
   XMIDI(); // No default constructor

   struct first_state {            // Status,   Data[0]
      midi_event      *patch[16];  // 0xC
      midi_event      *bank[16];   // 0xB,      0
      midi_event      *pan[16];    // 0xB,      7
      midi_event      *vol[16];    // 0xB,      10
   };

   //! List manipulation
   void CreateNewEvent(int time);

   // Variable length quantity
   int GetVLQ(SDL_RWops *source, Uint32 &quant);
   int GetVLQ2(SDL_RWops *source, Uint32 &quant);

   void AdjustTimings(Uint32 ppqn);   // This is used by Midi's ONLY!
   void ApplyFirstState(first_state &fs, int chan_mask);

   int ConvertNote(const int time, const unsigned char status, SDL_RWops *source, const int size);
   int ConvertEvent(const int time, const unsigned char status, SDL_RWops *source, const int size, first_state& fs);
   int ConvertSystemMessage (const int time, const unsigned char status, SDL_RWops *source);

   int ConvertFiletoList(SDL_RWops *source, const bool is_xmi, first_state& fs);

   int ExtractTracksFromXmi(SDL_RWops *source);
   int ExtractTracksFromMid(SDL_RWops *source, const Uint32 ppqn, const int num_tracks, const bool type1);
   
   int ExtractTracks(SDL_RWops *source);

protected:
   Uint16 num_tracks;

private:
   XMIDIEventList **events;

   midi_event *list;
   midi_event *current;
   midi_event *notes_on;
   
   const static char mt32asgm[128];
   const static char mt32asgs[256];
   bool bank127[16];
   int convert_type;
   
   bool do_reverb;
   bool do_chorus;
   int chorus_value;
   int reverb_value;

   //! Midi Volume Curve Modification
   static GammaTable<unsigned char> VolumeCurve;
};

#endif
