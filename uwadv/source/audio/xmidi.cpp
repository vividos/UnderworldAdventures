/*
   XMIDI driver to load and process xmi files
   Copyright (C) 2000  Ryan Nunn
   Copyright (C) 2002  Michael Fink

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
/*! \file xmidi.cpp

   XMIDI loading and processing implementation

   2002-04-08: mfink: made XMIDI use SDL_RWops instead of DataSource
                      other minor changes to get the stuff to work

*/

// needed includes
#include "xmidi.hpp"
#include <cstring>
#include <cstdlib>
#include <cmath>

#ifndef _MSC_VER
using namespace std;
#endif


// endian helper macros

# define ua_endian_convert16(x) ( (((x)&0x00ff)<<8) | (((x)&0xff00)>>8) )
# define ua_endian_convert32(x) ( ua_endian_convert16(((x)&0xffff0000)>>16) | \
   ua_endian_convert16((x)&0x0000ffff)<<16 )


// SDL_RWops helper functions

inline Uint8 SDL_RWread8(SDL_RWops *rwops)
{
   Uint8 val;
   SDL_RWread(rwops,&val,1,1);
   return val;
}

inline Uint16 SDL_RWread16(SDL_RWops *rwops)
{
   Uint16 val;
   SDL_RWread(rwops,&val,2,1);
#ifdef SDL_BIG_ENDIAN
   val = ua_endian_convert16(val)
#endif
   return val;
}

inline Uint32 SDL_RWread32(SDL_RWops *rwops)
{
   Uint32 val;
   SDL_RWread(rwops,&val,4,1);
#ifdef SDL_BIG_ENDIAN
   val = ua_endian_convert32(val)
#endif
   return val;
}

inline void SDL_RWwrite8(SDL_RWops *rwops, Uint8 val)
{
   SDL_RWwrite(rwops,&val,1,1);
}

inline void SDL_RWwrite16(SDL_RWops *rwops, Uint16 val)
{
#ifdef SDL_BIG_ENDIAN
   val = ua_endian_convert16(val)
#endif
   SDL_RWwrite(rwops,&val,2,1);
}

inline void SDL_RWwrite32(SDL_RWops *rwops, Uint32 val)
{
#ifdef SDL_BIG_ENDIAN
   val = ua_endian_convert32(val)
#endif
   SDL_RWwrite(rwops,&val,4,1);
}

//! gamma table template class
template <class T> class GammaTable
{
private:
   unsigned int size;
   float        sizef;
   T            *table;
   float        gamma;

   // ctor
   GammaTable(){ }

public:
   inline const float & get_gamma ()
   {   return gamma; }

   inline void set_gamma (float g)
   {
      if (g < 0.001f) g = 0.001f;
      if (g == gamma) return;
      gamma = g;

      for (unsigned int i = 0; i < size; i++)
         table[i] = (T) (pow (i / sizef, 1 / gamma) * sizef);
   }

   GammaTable (unsigned int s, float g = 1) : sizef(-1), gamma(-1)
   {
      sizef += size = s>2?s:2;
      table = new T [size];
      set_gamma(g);
   }

   ~GammaTable () { delete [] table; }

   inline const T & operator [] (const T &i) const
   { return table[i]; }
};


// Here's a bit of joy: WIN32 isn't SMP safe if we use operator new and delete.
// On the other hand, nothing else is thread-safe if we use malloc()/free().
// So, we wrap the implementations and use malloc()/calloc()/free() for WIN32, and
// the C++ thread-safe allocator for other platforms.

template<class T>
inline T* Malloc(size_t num=1)
{
#ifdef WIN32
   return static_cast<T*>(malloc(num));
#else
   return static_cast<T*>(::operator new(num));
#endif
}

template<class T>
inline T* Calloc(size_t num=1,size_t sz=0)
{
   if(!sz)
      sz=sizeof(T);
#ifdef WIN32
   return static_cast<T*>(calloc(num,sz));
#else
   size_t   total=sz*num;
   T *tmp=Malloc<T>(total);
   std::memset(tmp,0,total);
   return tmp;
#endif
}

inline void   Free(void *ptr)
{
#ifdef WIN32
   free(ptr);
#else
   ::operator delete(ptr);
#endif
}

// This is used to correct incorrect patch, vol and pan changes in midi files
// The bias is just a value to used to work out if a vol and pan belong with a 
// patch change. This is to ensure that the default state of a midi file is with
// the tracks centred, unless the first patch change says otherwise.
#define PATCH_VOL_PAN_BIAS   5


/*! This is a default set of patches to convert from MT32 to GM
    The index is the MT32 Patch nubmer and the value is the GM Patch
    This is only suitable for music that doesn'tdo timbre changes
    XMIDIs that contain Timbre changes will not convert properly */
const char XMIDI::mt32asgm[128] =
{
   0,   // 0    Piano 1
   1,   // 1    Piano 2
   2,   // 2    Piano 3 (synth)
   4,   // 3    EPiano 1
   4,   // 4    EPiano 2
   5,   // 5    EPiano 3
   5,   // 6    EPiano 4
   3,   // 7    Honkytonk
   16,  // 8    Organ 1
   17,  // 9    Organ 2
   18,  // 10   Organ 3
   16,  // 11   Organ 4
   19,  // 12   Pipe Organ 1
   19,  // 13   Pipe Organ 2
   19,  // 14   Pipe Organ 3
   21,  // 15   Accordion
   6,   // 16   Harpsichord 1
   6,   // 17   Harpsichord 2
   6,   // 18   Harpsichord 3
   7,   // 19   Clavinet 1
   7,   // 20   Clavinet 2
   7,   // 21   Clavinet 3
   8,   // 22   Celesta 1
   8,   // 23   Celesta 2
   62,  // 24   Synthbrass 1 (62)
   63,  // 25   Synthbrass 2 (63)
   62,  // 26   Synthbrass 3 Bank 8
   63,  // 27   Synthbrass 4 Bank 8
   38,  // 28   Synthbass 1
   39,  // 29   Synthbass 2
   38,  // 30   Synthbass 3 Bank 8
   39,  // 31   Synthbass 4 Bank 8
   88,  // 32   Fantasy
   90,  // 33   Harmonic Pan - No equiv closest is polysynth(90) :(
   52,  // 34   Choral ?? Currently set to SynthVox(54). Should it be ChoirAhhs(52)???
   92,  // 35   Glass
   97,  // 36   Soundtrack
   99,  // 37   Atmosphere
   14,  // 38   Warmbell, sounds kind of like crystal(98) perhaps Tubular Bells(14) would be better. It is!
   54,  // 39   FunnyVox, sounds alot like Bagpipe(109) and Shania(111)
   98,  // 40   EchoBell, no real equiv, sounds like Crystal(98)
   96,  // 41   IceRain
   68,  // 42   Oboe 2001, no equiv, just patching it to normal oboe(68)
   95,  // 43   EchoPans, no equiv, setting to SweepPad
   81,  // 44   DoctorSolo Bank 8
   87,  // 45   SchoolDaze, no real equiv
   112, // 46   Bell Singer
   80,  // 47   SquareWave
   48,  // 48   Strings 1
   48,  // 49   Strings 2 - should be 49
   44,  // 50   Strings 3 (Synth) - Experimental set to Tremollo Strings - should be 50
   45,  // 51   Pizzicato Strings
   40,  // 52   Violin 1
   40,  // 53   Violin 2 ? Viola
   42,  // 54   Cello 1
   42,  // 55   Cello 2
   43,  // 56   Contrabass
   46,  // 57   Harp 1
   46,  // 58   Harp 2
   24,  // 59   Guitar 1 (Nylon)
   25,  // 60   Guitar 2 (Steel)
   26,  // 61   Elec Guitar 1
   27,  // 62   Elec Guitar 2
   104, // 63   Sitar
   32,  // 64   Acou Bass 1
   32,  // 65   Acou Bass 2
   33,  // 66   Elec Bass 1
   34,  // 67   Elec Bass 2
   36,  // 68   Slap Bass 1
   37,  // 69   Slap Bass 2
   35,  // 70   Fretless Bass 1
   35,  // 71   Fretless Bass 2
   73,  // 72   Flute 1
   73,  // 73   Flute 2
   72,  // 74   Piccolo 1
   72,  // 75   Piccolo 2
   74,  // 76   Recorder
   75,  // 77   Pan Pipes
   64,  // 78   Sax 1
   65,  // 79   Sax 2
   66,  // 80   Sax 3
   67,  // 81   Sax 4
   71,  // 82   Clarinet 1
   71,  // 83   Clarinet 2
   68,  // 84   Oboe
   69,  // 85   English Horn (Cor Anglais)
   70,  // 86   Bassoon
   22,  // 87   Harmonica
   56,  // 88   Trumpet 1
   56,  // 89   Trumpet 2
   57,  // 90   Trombone 1
   57,  // 91   Trombone 2
   60,  // 92   French Horn 1
   60,  // 93   French Horn 2
   58,  // 94   Tuba   
   61,  // 95   Brass Section 1
   61,  // 96   Brass Section 2
   11,  // 97   Vibes 1
   11,  // 98   Vibes 2
   99,  // 99   Syn Mallet Bank 1
   112, // 100  WindBell no real equiv Set to TinkleBell(112)
   9,   // 101  Glockenspiel
   14,  // 102  Tubular Bells
   13,  // 103  Xylophone
   12,  // 104  Marimba
   107, // 105  Koto
   111, // 106  Sho?? set to Shanai(111)
   77,  // 107  Shakauhachi
   78,  // 108  Whistle 1
   78,  // 109  Whistle 2
   76,  // 110  Bottle Blow
   76,  // 111  Breathpipe no real equiv set to bottle blow(76)
   47,  // 112  Timpani
   117, // 113  Melodic Tom
   116, // 114  Deap Snare no equiv, set to Taiko(116)
   118, // 115  Electric Perc 1
   118, // 116  Electric Perc 2
   116, // 117  Taiko
   115, // 118  Taiko Rim, no real equiv, set to Woodblock(115)
   119, // 119  Cymbal, no real equiv, set to reverse cymbal(119)
   115, // 120  Castanets, no real equiv, in GM set to Woodblock(115)
   112, // 121  Triangle, no real equiv, set to TinkleBell(112)
   55,  // 122  Orchestral Hit
   124, // 123  Telephone
   123, // 124  BirdTweet
   94,  // 125  Big Notes Pad no equiv, set to halo pad (94)
   98,  // 126  Water Bell set to Crystal Pad(98)
   121  // 127  Jungle Tune set to Breath Noise
};

/*! Same as above, except include patch changes
    so GS instruments can be used */
const char XMIDI::mt32asgs[256] =
{
   0, 0,   // 0    Piano 1
   1, 0,   // 1    Piano 2
   2, 0,   // 2    Piano 3 (synth)
   4, 0,   // 3    EPiano 1
   4, 0,   // 4    EPiano 2
   5, 0,   // 5    EPiano 3
   5, 0,   // 6    EPiano 4
   3, 0,   // 7    Honkytonk
   16, 0,  // 8    Organ 1
   17, 0,  // 9    Organ 2
   18, 0,  // 10   Organ 3
   16, 0,  // 11   Organ 4
   19, 0,  // 12   Pipe Organ 1
   19, 0,  // 13   Pipe Organ 2
   19, 0,  // 14   Pipe Organ 3
   21, 0,  // 15   Accordion
   6, 0,   // 16   Harpsichord 1
   6, 0,   // 17   Harpsichord 2
   6, 0,   // 18   Harpsichord 3
   7, 0,   // 19   Clavinet 1
   7, 0,   // 20   Clavinet 2
   7, 0,   // 21   Clavinet 3
   8, 0,   // 22   Celesta 1
   8, 0,   // 23   Celesta 2
   62, 0,  // 24   Synthbrass 1 (62)
   63, 0,  // 25   Synthbrass 2 (63)
   62, 0,  // 26   Synthbrass 3 Bank 8
   63, 0,  // 27   Synthbrass 4 Bank 8
   38, 0,  // 28   Synthbass 1
   39, 0,  // 29   Synthbass 2
   38, 0,  // 30   Synthbass 3 Bank 8
   39, 0,  // 31   Synthbass 4 Bank 8
   88, 0,  // 32   Fantasy
   90, 0,  // 33   Harmonic Pan - No equiv closest is polysynth(90) :(
   52, 0,  // 34   Choral ?? Currently set to SynthVox(54). Should it be ChoirAhhs(52)???
   92, 0,  // 35   Glass
   97, 0,  // 36   Soundtrack
   99, 0,  // 37   Atmosphere
   14, 0,  // 38   Warmbell, sounds kind of like crystal(98) perhaps Tubular Bells(14) would be better. It is!
   54, 0,  // 39   FunnyVox, sounds alot like Bagpipe(109) and Shania(111)
   98, 0,  // 40   EchoBell, no real equiv, sounds like Crystal(98)
   96, 0,  // 41   IceRain
   68, 0,  // 42   Oboe 2001, no equiv, just patching it to normal oboe(68)
   95, 0,  // 43   EchoPans, no equiv, setting to SweepPad
   81, 0,  // 44   DoctorSolo Bank 8
   87, 0,  // 45   SchoolDaze, no real equiv
   112, 0, // 46   Bell Singer
   80, 0,  // 47   SquareWave
   48, 0,  // 48   Strings 1
   48, 0,  // 49   Strings 2 - should be 49
   44, 0,  // 50   Strings 3 (Synth) - Experimental set to Tremollo Strings - should be 50
   45, 0,  // 51   Pizzicato Strings
   40, 0,  // 52   Violin 1
   40, 0,  // 53   Violin 2 ? Viola
   42, 0,  // 54   Cello 1
   42, 0,  // 55   Cello 2
   43, 0,  // 56   Contrabass
   46, 0,  // 57   Harp 1
   46, 0,  // 58   Harp 2
   24, 0,  // 59   Guitar 1 (Nylon)
   25, 0,  // 60   Guitar 2 (Steel)
   26, 0,  // 61   Elec Guitar 1
   27, 0,  // 62   Elec Guitar 2
   104, 0, // 63   Sitar
   32, 0,  // 64   Acou Bass 1
   32, 0,  // 65   Acou Bass 2
   33, 0,  // 66   Elec Bass 1
   34, 0,  // 67   Elec Bass 2
   36, 0,  // 68   Slap Bass 1
   37, 0,  // 69   Slap Bass 2
   35, 0,  // 70   Fretless Bass 1
   35, 0,  // 71   Fretless Bass 2
   73, 0,  // 72   Flute 1
   73, 0,  // 73   Flute 2
   72, 0,  // 74   Piccolo 1
   72, 0,  // 75   Piccolo 2
   74, 0,  // 76   Recorder
   75, 0,  // 77   Pan Pipes
   64, 0,  // 78   Sax 1
   65, 0,  // 79   Sax 2
   66, 0,  // 80   Sax 3
   67, 0,  // 81   Sax 4
   71, 0,  // 82   Clarinet 1
   71, 0,  // 83   Clarinet 2
   68, 0,  // 84   Oboe
   69, 0,  // 85   English Horn (Cor Anglais)
   70, 0,  // 86   Bassoon
   22, 0,  // 87   Harmonica
   56, 0,  // 88   Trumpet 1
   56, 0,  // 89   Trumpet 2
   57, 0,  // 90   Trombone 1
   57, 0,  // 91   Trombone 2
   60, 0,  // 92   French Horn 1
   60, 0,  // 93   French Horn 2
   58, 0,  // 94   Tuba   
   61, 0,  // 95   Brass Section 1
   61, 0,  // 96   Brass Section 2
   11, 0,  // 97   Vibes 1
   11, 0,  // 98   Vibes 2
   99, 0,  // 99   Syn Mallet Bank 1
   112, 0, // 100  WindBell no real equiv Set to TinkleBell(112)
   9, 0,   // 101  Glockenspiel
   14, 0,  // 102  Tubular Bells
   13, 0,  // 103  Xylophone
   12, 0,  // 104  Marimba
   107, 0, // 105  Koto
   111, 0, // 106  Sho?? set to Shanai(111)
   77, 0,  // 107  Shakauhachi
   78, 0,  // 108  Whistle 1
   78, 0,  // 109  Whistle 2
   76, 0,  // 110  Bottle Blow
   76, 0,  // 111  Breathpipe no real equiv set to bottle blow(76)
   47, 0,  // 112  Timpani
   117, 0, // 113  Melodic Tom
   116, 0, // 114  Deap Snare no equiv, set to Taiko(116)
   118, 0, // 115  Electric Perc 1
   118, 0, // 116  Electric Perc 2
   116, 0, // 117  Taiko
   115, 0, // 118  Taiko Rim, no real equiv, set to Woodblock(115)
   119, 0, // 119  Cymbal, no real equiv, set to reverse cymbal(119)
   115, 0, // 120  Castanets, no real equiv, in GM set to Woodblock(115)
   112, 0, // 121  Triangle, no real equiv, set to TinkleBell(112)
   55, 0,  // 122  Orchestral Hit
   124, 0, // 123  Telephone
   123, 0, // 124  BirdTweet
   94, 0,  // 125  Big Notes Pad no equiv, set to halo pad (94)
   98, 0,  // 126  Water Bell set to Crystal Pad(98)
   121, 0  // 127  Jungle Tune set to Breath Noise
};


// XMIDI static variables

GammaTable<unsigned char> XMIDI::VolumeCurve(128);


// XMIDI methods

XMIDI::XMIDI(int pconvert) :
   events(NULL), convert_type(pconvert), do_reverb(false), do_chorus(false)
{
   memset(bank127,0,sizeof(bank127));
}

XMIDI::~XMIDI()
{
   if (events)
   {
      for (int i=0; i < num_tracks; i++) {
         events[i]->DecrementCounter();
         events[i] = NULL;
      }
      //delete [] events;
      Free(events);
   }
}

bool XMIDI::Load(const char *filename)
{
   SDL_RWops *file = SDL_RWFromFile(filename,"rb");
   if (file==NULL)
      return false;

   ExtractTracks(file);

   SDL_RWclose(file);

   return true;
}

XMIDIEventList *XMIDI::GetEventList(Uint32 track)
{
   if (!events || track >= num_tracks)
      return NULL;

   return events[track];
}

//! Sets current to the new event and updates list
void XMIDI::CreateNewEvent(int time)
{
   if (!list)
   {
      list = current = Calloc<midi_event>(); //new midi_event;
      if (time > 0)
         current->time = time;
      return;
   }

   if (time < 0 || list->time > time)
   {
      midi_event *event = Calloc<midi_event>(); //new midi_event;
      event->next = list;
      list = current = event;
      return;
   }

   if (!current || current->time > time)
      current = list;

   while (current->next)
   {
      if (current->next->time > time)
      {
         midi_event *event = Calloc<midi_event>(); //new midi_event;

         event->next = current->next;
         current->next = event;
         current = event;
         current->time = time;
         return;
      }

      current = current->next;
   }

   current->next = Calloc<midi_event>(); //new midi_event;
   current = current->next;
   current->time = time;
}

//! Get a Conventional Variable Length Quantity
int XMIDI::GetVLQ (SDL_RWops *source, Uint32 &quant)
{
   int i;
   quant=0;
   Uint8 data=0;

   for (i = 0; i < 4; i++)
   {
      data = SDL_RWread8(source);
      quant <<= 7;
      quant |= data & 0x7F;

      if (!(data & 0x80))
      {
         i++;
         break;
      }
   }
   return i;
}

//! Get a XMIDI Variable Length Quantity
int XMIDI::GetVLQ2 (SDL_RWops *source, Uint32 &quant)
{
   int i;
   quant=0;
   Uint8 data=0;

   for (i = 0; i < 4; i++)
   {
      data = SDL_RWread8(source);
      if (data & 0x80)
      {
         SDL_RWseek(source,-1,SEEK_CUR);
         break;
      }
      quant += data;
   }
   return i;
}

/*! Well, this is just a modified version of what that method used to do. This
    is a massive optimization. Speed up should be quite impressive */
void XMIDI::ApplyFirstState(first_state &fs, int chan_mask)
{
   for (Uint8 channel = 0; channel < 16; channel++)
   {
      midi_event *patch = fs.patch[channel];
      midi_event *vol = fs.vol[channel];
      midi_event *pan = fs.pan[channel];
      midi_event *bank = fs.bank[channel];
      midi_event *reverb = NULL;
      midi_event *chorus = NULL;
      midi_event *temp;

      // Got no patch change, return and don't try fixing it
      if (!patch || !(chan_mask & 1 << channel)) continue;
#if 0
      std::cout << "Channel: " << channel+1 << std::endl;
      std::cout << "Patch: " << (unsigned int) patch->data[0] << " @ " << patch->time << std::endl;
      if (bank) std::cout << " Bank: " << (unsigned int) bank->data[1] << " @ " << bank->time << std::endl;
      if (vol) std::cout << "  Vol: " << (unsigned int) vol->data[1] << " @ " << vol->time << std::endl;
      if (pan) std::cout << "  Pan: " << ((signed int) pan->data[1])-64 << " @ " << pan->time << std::endl;
      std::cout << std::endl;
#endif

      // Copy Patch Change Event
      temp = patch;
      patch = Calloc<midi_event>(); //new midi_event;
      patch->time = temp->time;
      patch->status = Uint8(channel|(MIDI_STATUS_PROG_CHANGE << 4));
      patch->data[0] = temp->data[0];

      // Copy Volume
      if (vol && (vol->time > patch->time+PATCH_VOL_PAN_BIAS ||
         vol->time < patch->time-PATCH_VOL_PAN_BIAS))
            vol = NULL;

      temp = vol;
      vol = Calloc<midi_event>(); //new midi_event;
      vol->status = Uint8(channel|(MIDI_STATUS_CONTROLLER << 4));
      vol->data[0] = 7;

      if (!temp)
      {
         if (convert_type) vol->data[1] = VolumeCurve[90];
         else vol->data[1] = 90;
      }
      else
         vol->data[1] = temp->data[1];


      // Copy Bank
      if (bank && (bank->time > patch->time+PATCH_VOL_PAN_BIAS ||
         bank->time < patch->time-PATCH_VOL_PAN_BIAS))
            bank = NULL;

      temp = bank;

      bank = Calloc<midi_event>(); //new midi_event;
      bank->status = Uint8(channel|(MIDI_STATUS_CONTROLLER << 4));

      if (!temp)
         bank->data[1] = 0;
      else
         bank->data[1] = temp->data[1];

      // Copy Pan
      if (pan && (pan->time > patch->time+PATCH_VOL_PAN_BIAS ||
         pan->time < patch->time-PATCH_VOL_PAN_BIAS))
            pan = NULL;

      temp = pan;
      pan = Calloc<midi_event>(); //new midi_event;
      pan->status = Uint8(channel|(MIDI_STATUS_CONTROLLER << 4));
      pan->data[0] = 10;

      if (!temp)
         pan->data[1] = 64;
      else
         pan->data[1] = temp->data[1];

      if (do_reverb)
      {
         reverb = Calloc<midi_event>(); //new midi_event;
         reverb->status = Uint8(channel|(MIDI_STATUS_CONTROLLER << 4));
         reverb->data[0] = 91;
         reverb->data[1] = reverb_value;
      }

      if (do_chorus)
      {
         chorus = Calloc<midi_event>(); //new midi_event;
         chorus->status = Uint8(channel|(MIDI_STATUS_CONTROLLER << 4));
         chorus->data[0] = 93;
         chorus->data[1] = chorus_value;
      }

      vol->time = 0;
      pan->time = 0;
      patch->time = 0;
      bank->time = 0;

      if (do_reverb && do_chorus) reverb->next = chorus;
      else if (do_reverb) reverb->next = bank;
      if (do_chorus) chorus->next = bank;
      bank->next = vol;
      vol->next = pan;
      pan->next = patch;

      patch->next = list;
      if (do_reverb) list = reverb;
      else if (do_chorus) list = chorus;
      else list = bank;
   }
}

#ifndef SDL_HAS_64BIT_TYPE
# error need 64 bit type in XMIDI::AdjustTimings()
#endif

/*! It converts the midi's to use 120 Hz timing, and also calcs the duration of
    the notes. It also strips the tempo events, and adds it's own

    This is used by Midi's ONLY! It will do nothing with Xmidi
*/
void XMIDI::AdjustTimings(Uint32 ppqn)
{
   Uint32      tempo = 500000;
   Uint32      time_prev = 0;
   Uint32      hs_rem = 0;
   Uint32      hs     = 0;

   ppqn *= 10000;

   // Virtual playing
   NoteStack notes;

   for (midi_event   *event = list; event; event = event->next)
   {
      // Note 64 bit int is required because multiplication by tempo can
      // require 52 bits in some circumstances

      Uint64 aim = event->time - time_prev;
      aim *= tempo;

      hs_rem += Uint32(aim%ppqn);
      hs += Uint32(aim/ppqn);
      hs += hs_rem/ppqn;
      hs_rem %= ppqn;

      time_prev = event->time;
      event->time = (hs*6)/5 + (6*hs_rem)/(5*ppqn);

      // Note on and note off handling
      if (event->status <= 0x9F)
      {
         // Add if it's a note on and remove if it's a note off
         if ((event->status>>4) == MIDI_STATUS_NOTE_ON && event->data[1])
            notes.Push(event);
         else {
            midi_event *prev = notes.FindAndPop(event);
            if (prev) prev->duration = event->time - prev->time;
         }
      }
      else if (event->status == 0xFF && event->data[0] == 0x51)
      {
         tempo = (event->buffer[0] << 16) +
            (event->buffer[1] << 8) +
            event->buffer[2];

         event->buffer[0] = 0x07;
         event->buffer[1] = 0xA1;
         event->buffer[2] = 0x20;
      }
   }

   //std::cout << "Max Polyphony: " << notes.GetMaxPolyphony() << std::endl;
   unsigned char tempo_buf[5] = { 0x51, 0x03, 0x07, 0xA1, 0x20 };

   SDL_RWops *tbuf = SDL_RWFromMem(tempo_buf,5);
   current = list;
   ConvertSystemMessage(0, 0xFF, tbuf);

   SDL_RWclose(tbuf);
}

/*! Source is at the first data byte
    size 1 is single data byte (ConvertEvent Only)
    size 2 is dual data byte
    size 3 is XMI Note on (ConvertNote only)
    Returns bytes converted

    ConvertNote is used for Note On's and Note offs
    ConvertSystemMessage is used for SysEx events and Meta events
    ConvertEvent is used for everything else
*/
int XMIDI::ConvertEvent (const int time, const unsigned char status, SDL_RWops *source, const int size, first_state &fs)
{
   Uint32 delta=0;
   int data;

   data = SDL_RWread8(source);

   // Bank changes are handled here
   if ((status >> 4) == 0xB && data == 0)
   {
      data = SDL_RWread8(source);

      bank127[status&0xF] = false;

      if (convert_type == XMIDI_CONVERT_MT32_TO_GM ||
          convert_type == XMIDI_CONVERT_MT32_TO_GS ||
          convert_type == XMIDI_CONVERT_MT32_TO_GS127)
         return 2;

      CreateNewEvent(time);
      current->status = status;
      current->data[0] = 0;
      current->data[1] = data;

      // Set the bank
      if (!fs.bank[status&0xF] || fs.bank[status&0xF]->time > time)
         fs.bank[status&0xF] = current;

      if (convert_type == XMIDI_CONVERT_GS127_TO_GS && data == 127)
         bank127[status&0xF] = true;

      return 2;
   }

   // Handling for patch change mt32 conversion, probably should go elsewhere
   if ((status >> 4) == 0xC && (status&0xF) != 9 && convert_type != XMIDI_CONVERT_NOCONVERSION)
   {
      if (convert_type == XMIDI_CONVERT_MT32_TO_GM)
      {
         data = mt32asgm[data];
      }
      else if ((convert_type == XMIDI_CONVERT_GS127_TO_GS && bank127[status&0xF]) ||
            convert_type == XMIDI_CONVERT_MT32_TO_GS)
      {
         CreateNewEvent (time);
         current->status = 0xB0 | (status&0xF);
         current->data[0] = 0;
         current->data[1] = mt32asgs[data*2+1];

         data = mt32asgs[data*2];

         // Set the bank
         if (!fs.bank[status&0xF] || fs.bank[status&0xF]->time > time)
            fs.bank[status&0xF] = current;
      }
      else if (convert_type == XMIDI_CONVERT_MT32_TO_GS127)
      {
         CreateNewEvent (time);
         current->status = 0xB0 | (status&0xF);
         current->data[0] = 0;
         current->data[1] = 127;

         // Set the bank
         if (!fs.bank[status&0xF] || fs.bank[status&0xF]->time > time)
            fs.bank[status&0xF] = current;
      }
   }// Disable patch changes on Track 10 is doing a conversion
   else if ((status >> 4) == 0xC && (status&0xF) == 9 && convert_type != XMIDI_CONVERT_NOCONVERSION)
   {
      return size;
   }

   CreateNewEvent (time);
   current->status = status;

   current->data[0] = data;

   // Check for patch change, and update fs if req
   if ((status >> 4) == 0xC)
   {
      if (!fs.patch[status&0xF] || fs.patch[status&0xF]->time > time)
         fs.patch[status&0xF] = current;
   }
   // Controllers
   else if ((status >> 4) == 0xB)
   {
      // Volume
      if (current->data[0] == 7)
      {
         if (!fs.vol[status&0xF] || fs.vol[status&0xF]->time > time)
            fs.vol[status&0xF] = current;
      }
      // Pan
      else if (current->data[0] == 10)
      {
         if (!fs.pan[status&0xF] || fs.pan[status&0xF]->time > time)
            fs.pan[status&0xF] = current;
      }
   }

   if (size == 1)
      return 1;

   current->data[1] = SDL_RWread8(source);

   // Volume modify the volume controller, only if converting
   if (convert_type && (current->status >> 4) == MIDI_STATUS_CONTROLLER && current->data[0] == 7)
      current->data[1] = VolumeCurve[current->data[1]];

   return 2;
}

int XMIDI::ConvertNote (const int time, const unsigned char status, SDL_RWops *source, const int size)
{
   Uint32 delta = 0;
   int data;

   data = SDL_RWread8(source);

   CreateNewEvent (time);
   current->status = status;

   current->data[0] = data;
   current->data[1] = SDL_RWread8(source);

   // Volume modify the note on's, only if converting
   if (convert_type && (current->status >> 4) == MIDI_STATUS_NOTE_ON && current->data[1])
      current->data[1] = VolumeCurve[current->data[1]];

   if (size == 2)
      return 2;

   // XMI Note On handling

   // Get the duration
   int i = GetVLQ(source, delta);

   // Set the duration
   current->duration = delta;

   // This is an optimization
   midi_event *prev = current;

   // Create a note off
   CreateNewEvent(time+delta);

   current->status = status;
   current->data[0] = data;
   current->data[1] = 0;

   // Optimization
   current = prev;

   return i + 2;
}

//! Simple routine to convert system messages
int XMIDI::ConvertSystemMessage (const int time, const unsigned char status, SDL_RWops *source)
{
   int i=0;

   CreateNewEvent (time);
   current->status = status;

   // Handling of Meta events
   if (status == 0xFF)
   {
      current->data[0] = SDL_RWread8(source);
      i++;
   }

   i += GetVLQ (source, current->len);

   if (!current->len)
   {
      current->buffer = NULL;
      return i;
   }
   
   current->buffer = Malloc<unsigned char>(current->len);

   SDL_RWread(source,current->buffer,1,current->len);

   return i+current->len;
}

//! XMIDI and Midi to List. Returns bit mask of channels used
int XMIDI::ConvertFiletoList(SDL_RWops *source, const bool is_xmi, first_state &fs)
{
   int      time = 0; // 120th of a second
   Uint32   data;
   int      end = 0;
   Uint32   status = 0;
   int      play_size = 2;
   int      file_size;
   int      retval = 0;

   // find out file length
   {
      int curpos = SDL_RWtell(source);
      SDL_RWseek(source,0,SEEK_END);
      file_size = SDL_RWtell(source);
      SDL_RWseek(source,curpos,SEEK_SET);
   }

   if (is_xmi) play_size = 3;

   while (!end && SDL_RWtell(source) < file_size)
   {
      if (!is_xmi)
      {
         GetVLQ(source, data);
         time += data;

         data = SDL_RWread8(source);

         if (data >= 0x80)
         {
            status = data;
         }
         else
            SDL_RWseek(source,-1,SEEK_CUR);
      }
      else
      {
         GetVLQ2(source, data);
         time += data;

         status = SDL_RWread8(source);
      }

      switch (status >> 4)
      {
         case MIDI_STATUS_NOTE_ON:
         retval |= 1 << (status & 0xF);
         ConvertNote (time, status, source, play_size);
         break;

         case MIDI_STATUS_NOTE_OFF:
         ConvertNote (time, status, source, 2);
         break;

         // 2 byte data
         case MIDI_STATUS_AFTERTOUCH:
         case MIDI_STATUS_CONTROLLER:
         case MIDI_STATUS_PITCH_WHEEL:
         ConvertEvent (time, status, source, 2, fs);
         break;

         // 1 byte data
         case MIDI_STATUS_PROG_CHANGE:
         case MIDI_STATUS_PRESSURE:
         ConvertEvent (time, status, source, 1, fs);
         break;

         case MIDI_STATUS_SYSEX:
         if (status == 0xFF)
         {
            int pos = SDL_RWtell(source);
            Uint32 data = SDL_RWread8(source);

            if (data == 0x2F)                  // End, of track
               end = 1;
            else if (data == 0x51 && is_xmi)   // XMIDI doesn't use tempo
            {
               GetVLQ (source, data);
               SDL_RWseek(source,data,SEEK_CUR);
               break;
            }

            SDL_RWseek(source,pos,SEEK_SET);
         }
         ConvertSystemMessage (time, status, source);
         break;

         default:
         break;
      }

   }

   return retval;
}

/*! Assumes correct xmidi */
int XMIDI::ExtractTracksFromXmi(SDL_RWops *source)
{
   int    num = 0;
   Uint32 len = 0;
   char   buf[32];
   int    file_size;

   first_state fs;

   // find out file length
   {
      int curpos = SDL_RWtell(source);
      SDL_RWseek(source,0,SEEK_END);
      file_size = SDL_RWtell(source);
      SDL_RWseek(source,curpos,SEEK_SET);
   }

   while (SDL_RWtell(source) < file_size && num != num_tracks)
   {
      // Read first 4 bytes of name
      SDL_RWread(source,buf,1,4);

      len = SDL_RWread32(source);
      len = ua_endian_convert32(len); // caution! macro arg is evaluated twice

      // Skip the FORM entries
      if (!memcmp(buf,"FORM",4))
      {
         SDL_RWseek(source,4,SEEK_CUR);
         SDL_RWread(source,buf,1,4);

         len = SDL_RWread32(source);
         len = ua_endian_convert32(len);
      }

      if (memcmp(buf,"EVNT",4))
      {
         SDL_RWseek(source,((len+1)&~1),SEEK_CUR);
         continue;
      }

      list = NULL;
      memset(&fs, 0, sizeof(fs));

      int begin = SDL_RWtell(source);

      // Convert it
      int chan_mask = ConvertFiletoList(source, true, fs);

      // Apply the first state
      ApplyFirstState(fs, chan_mask);

      // Add tempo
      unsigned char tempo_buf[5] = { 0x51, 0x03, 0x07, 0xA1, 0x20 };

      SDL_RWops *tbuf = SDL_RWFromMem(tempo_buf,5);
      current = list;
      ConvertSystemMessage(0, 0xFF, tbuf);

      SDL_RWclose(tbuf);

      // Set the list
      events[num]->events = list;

      // Increment Counter
      num++;

      // go to start of next track
      SDL_RWseek(source,(begin + ((len+1)&~1)),SEEK_SET);
   }

   // Return how many were converted
   return num;
}

int XMIDI::ExtractTracksFromMid (SDL_RWops *source, const Uint32 ppqn, const int num_tracks, const bool type1)
{
   int    num = 0;
   Uint32 len = 0;
   char   buf[32];
   int    chan_mask = 0;
   int    file_size;

   first_state fs;
   memset(&fs, 0, sizeof(fs));

   // find out file length
   {
      int curpos = SDL_RWtell(source);
      SDL_RWseek(source,0,SEEK_END);
      file_size = SDL_RWtell(source);
      SDL_RWseek(source,curpos,SEEK_SET);
   }

   list = NULL;

   while (SDL_RWtell(source) < file_size && num != num_tracks)
   {
      // Read first 4 bytes of name
      SDL_RWread(source,buf,1,4);
      len = SDL_RWread32(source);
      len = ua_endian_convert32(len);

      if (memcmp(buf,"MTrk",4))
      {
         SDL_RWseek(source,len,SEEK_CUR);
         continue;
      }

      int begin = SDL_RWtell(source);

      // Convert it
      chan_mask |= ConvertFiletoList(source, false, fs);

      if (!type1)
      {
         ApplyFirstState(fs, chan_mask);
         AdjustTimings(ppqn);
         events[num]->events = list;
         list = NULL;
         memset(&fs, 0, sizeof(fs));
         chan_mask = 0;
      }

      // Increment Counter
      num++;
      SDL_RWseek(source,(begin+len),SEEK_SET);
   }

   if (type1)
   {
      ApplyFirstState(fs, chan_mask);
      AdjustTimings(ppqn);
      events[0]->events = list;
      return num == num_tracks ? 1 : 0;
   }

   // Return how many were converted
   return num;
}

//! \todo get do_reverb, reverb_value, do_chorus, chorus_value from somewhere
int XMIDI::ExtractTracks(SDL_RWops *source)
{
   Uint32 i = 0;
   int    start;
   Uint32 len;
   Uint32 chunk_len;
   int    count;
   char   buf[32];

   do_reverb = true;
   reverb_value = 64;
   do_chorus = true;
   chorus_value = 16;
   VolumeCurve.set_gamma(1.0);

   // Read first 4 bytes of header
   SDL_RWread(source,buf,1,4);

   // Could be XMIDI
   if (!memcmp (buf, "FORM", 4))
   {
      // Read length of chunk
      len = SDL_RWread32(source);
      len = ua_endian_convert32(len);

      start = SDL_RWtell(source);

      // Read 4 bytes of type
      SDL_RWread(source,buf,1,4);

      if (!memcmp (buf, "XMID", 4))
      {
         // XDIRless XMIDI, we can handle them here.
//         std::cerr << "Warning: XMIDI doesn't have XDIR" << std::endl;
         num_tracks = 1;
      }
      else if (memcmp (buf, "XDIR", 4))
      {
         // Not an XMIDI that we recognise
//         std::cerr << "Not a recognised XMID" << std::endl;
         return 0;
      }
      else
      {
         // Seems Valid
         num_tracks = 0;

         for (i = 4; i < len; i++)
         {
            // Read 4 bytes of type
            SDL_RWread(source,buf,1,4);

            // Read length of chunk
            chunk_len = SDL_RWread32(source);
            chunk_len = ua_endian_convert32(chunk_len);

            // Add eight bytes
            i+=8;

            if (memcmp (buf, "INFO", 4))
            {
               // Must allign
               SDL_RWseek(source,((chunk_len+1)&~1),SEEK_CUR);
               i+= (chunk_len+1)&~1;
               continue;
            }

            // Must be at least 2 bytes long
            if (chunk_len < 2)
               break;

            num_tracks = SDL_RWread16(source);
            break;
         }

         // Didn't get to fill the header
         if (num_tracks == 0)
         {
//            std::cerr << "Not a valid XMID" << std::endl;
            return 0;
         }

         // Ok now to start part 2
         // Goto the right place
         SDL_RWseek(source,(start+((len+1)&~1)),SEEK_SET);

         // Read 4 bytes of type
         SDL_RWread(source,buf,1,4);

         // Not an XMID
         if (memcmp (buf, "CAT ", 4))
         {
//            std::cerr << "Not a recognised XMID (" << buf[0] << buf[1] << buf[2] << buf[3] << ") should be (CAT )" << std::endl;
            return 0;
         }

         // Now read length of this track
         len = SDL_RWread32(source);
         len = ua_endian_convert32(len);

         // Read 4 bytes of type
         SDL_RWread(source,buf,1,4);

         // Not an XMID
         if (memcmp(buf, "XMID", 4))
         {
//            std::cerr << "Not a recognised XMID (" << buf[0] << buf[1] << buf[2] << buf[3] << ") should be (XMID)" << std::endl;
            return 0;
         }
      }

      // Ok it's an XMID, so pass it to the ExtractCode

      events = Calloc<XMIDIEventList*>(num_tracks); //new midi_event *[info.tracks];

      for (i = 0; i < num_tracks; i++)
         events[i] = Calloc<XMIDIEventList>();

      count = ExtractTracksFromXmi(source);

      if (count != num_tracks)
      {
//         std::cerr << "Error: unable to extract all (" << num_tracks << ") tracks specified from XMIDI. Only ("<< count << ")" << std::endl;

         int i = 0;
         for (i = 0; i < num_tracks; i++)
         {
            events[i]->DecrementCounter();
            events[i] = NULL;
         }

         //delete [] events;
         Free(events);

         return 0;
      }

      return 1;

   }
   else if (!memcmp (buf, "MThd", 4))
   {
      // Definately a Midi

      // Simple read length of header
      len = SDL_RWread32(source);
      len = ua_endian_convert32(len);

      if (len < 6)
      {
//         std::cerr << "Not a valid MIDI" << std::endl;
         return 0;
      }

      int type = SDL_RWread16(source);
      type = ua_endian_convert16(type);

      num_tracks = SDL_RWread16(source);
      num_tracks = ua_endian_convert16(num_tracks);

      int actual_num = num_tracks;

      // Type 1 only has 1 track, even though it says it has more
      if (type == 1) num_tracks = 1;

      events = Calloc<XMIDIEventList*>(num_tracks); //new midi_event *[info.tracks];

      Uint32 ppqn = SDL_RWread16(source);
      ppqn = ua_endian_convert16(ppqn);

      for (i = 0; i < num_tracks; i++)
         events[i] = Calloc<XMIDIEventList>();

      count = ExtractTracksFromMid(source, ppqn, actual_num, type == 1);

      if (count != num_tracks)
      {
//         std::cerr << "Error: unable to extract all (" << num_tracks << ") tracks specified from MIDI. Only ("<< count << ")" << std::endl;

         for (i = 0; i < num_tracks; i++) {
            events[i]->DecrementCounter();
            events[i] = NULL;
         }

         Free (events);

         return 0;
      }

      return 1;

   }
   else if (!memcmp (buf, "RIFF", 4))
   {
      // A RIFF Midi, just pass the source back to this function at the start of the midi file

      // Read len
      len = SDL_RWread32(source);

      // Read 4 bytes of type
      SDL_RWread(source,buf,1,4);

      if (memcmp (buf, "RMID", 4))
      {
         // Not an RMID
//         std::cerr << "Invalid RMID" << std::endl;
         return 0;
      }

      // Is a RMID

      for (i = 4; i < len; i++)
      {
         // Read 4 bytes of type
         SDL_RWread(source,buf,1,4);

         chunk_len = SDL_RWread32(source);

         i+=8;

         if (memcmp (buf, "data", 4))
         {   
            // Must allign
            SDL_RWseek(source,((chunk_len+1)&~1),SEEK_CUR);

            i+= (chunk_len+1)&~1;
            continue;
         }
         
         return ExtractTracks(source);
      }

//      std::cerr << "Failed to find midi data in RIFF Midi" << std::endl;
      return 0;
   }

   return 0;
}


// XMIDIEventList methods

void XMIDIEventList::DecrementCounter()
{
   if (--counter < 0)
   {
      DeleteEventList(events);
      Free(this);
   }
}

void XMIDIEventList::DeleteEventList (midi_event *mlist)
{
   midi_event *event;
   midi_event *next;

   next = mlist;
   event = mlist;

   while ((event = next))
   {
      next = event->next;
      // We only do this with sysex
      if ((event->status>>4) == 0xF && event->buffer)
         Free (event->buffer);
      Free (event);
   }
}

int XMIDIEventList::Write(SDL_RWops *dest)
{
   int len = 0;

   if (!events)
   {
//      cerr << "No midi data in loaded." << endl;
      return 0;
   }

   // This is so if using buffer datasource, the caller can know how big to make the buffer
   if (!dest)
   {
      // Header is 14 bytes long and add the rest as well
      len = ConvertListToMTrk(NULL);
      return len+14;
   }

   SDL_RWwrite8(dest,'M');
   SDL_RWwrite8(dest,'T');
   SDL_RWwrite8(dest,'h');
   SDL_RWwrite8(dest,'d');

   SDL_RWwrite32(dest,ua_endian_convert32(6)); // track length
   SDL_RWwrite16(dest,ua_endian_convert16(0)); // type
   SDL_RWwrite16(dest,ua_endian_convert16(1)); // number of tracks
   SDL_RWwrite16(dest,ua_endian_convert16(60)); // the PPQN

   len = ConvertListToMTrk(dest);

   return len+14;
}

int XMIDIEventList::PutVLQ(SDL_RWops *dest, Uint32 value)
{
   int buffer;
   int i = 1;
   buffer = value & 0x7F;
   while (value >>= 7)
   {
      buffer <<= 8;
      buffer |= ((value & 0x7F) | 0x80);
      i++;
   }

   if (!dest) return i;
   for (int j = 0; j < i; j++)
   {
      SDL_RWwrite8(dest,buffer & 0xFF);
      buffer >>= 8;
   }

   return i;
}

/*! Returns bytes of the array; buf can be NULL */
Uint32 XMIDIEventList::ConvertListToMTrk(SDL_RWops *dest)
{
   int time = 0;
   int lasttime = 0;
   midi_event *event;
   Uint32 delta;
   unsigned char last_status = 0;
   Uint32 i = 8;
   Uint32 j;
   Uint32 size_pos=0;

   if (dest)
   {
      SDL_RWwrite8(dest,'M');
      SDL_RWwrite8(dest,'T');
      SDL_RWwrite8(dest,'r');
      SDL_RWwrite8(dest,'k');

      size_pos = SDL_RWtell(dest);
      SDL_RWseek(dest,4,SEEK_CUR); // skip size, will be written later
   }

   for (event = events; event; event=event->next)
   {
      // We don't write the end of stream marker here, we'll do it later
      if (event->status == 0xFF && event->data[0] == 0x2f)
      {
         lasttime = event->time;
         continue;
      }

      delta = (event->time - time);
      time = event->time;

      i += PutVLQ(dest, delta);

      if ((event->status != last_status) || (event->status >= 0xF0))
      {
         if (dest) SDL_RWwrite8(dest,event->status);
         i++;
      }

      last_status = event->status;

      switch (event->status >> 4)
      {
      // 2 bytes data
      // Note off, Note on, Aftertouch, Controller and Pitch Wheel
      case 0x8: case 0x9: case 0xA: case 0xB: case 0xE:
         if (dest)
         {
            SDL_RWwrite8(dest,event->data[0]);
            SDL_RWwrite8(dest,event->data[1]);
         }
         i += 2;
         break;

      // 1 bytes data
      // Program Change and Channel Pressure
      case 0xC: case 0xD:
         if (dest) SDL_RWwrite8(dest,event->data[0]);
         i++;
         break;

      // Variable length
      // SysEx
      case 0xF:
         if (event->status == 0xFF)
         {
            if (dest) SDL_RWwrite8(dest,event->data[0]);
            i++;
         }

         i += PutVLQ(dest, event->len);

         if (event->len)
         {
            for (j = 0; j < event->len; j++)
            {
               if (dest) SDL_RWwrite8(dest,event->buffer[j]); 
               i++;
            }
         }
         break;

      // Never occur
      default:
//         cerr << "Not supposed to see this" << endl;
         break;
      }
   }

   // Write out end of stream marker
   if (lasttime > time)
      i += PutVLQ(dest, lasttime-time);
   else
      i += PutVLQ(dest, 0);

   if (dest)
   {
      SDL_RWwrite8(dest,0xFF);
      SDL_RWwrite8(dest,0x2F);
   }

   i += PutVLQ(dest,0)+2;

   if (dest)
   {
      Uint32 cur_pos = SDL_RWtell(dest);
      SDL_RWseek(dest,size_pos,SEEK_SET);

      Uint32 length = i-8;
      SDL_RWwrite32(dest,ua_endian_convert32(length));

      SDL_RWseek(dest,cur_pos,SEEK_SET);
   }
   return i;
}
