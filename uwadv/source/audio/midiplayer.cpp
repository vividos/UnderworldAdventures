//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Michael Fink
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
/// \file midiplayer.cpp
/// \brief midi playback class implementation
//
#include "audio.hpp"
#include "midiplayer.hpp"
#include "settings.hpp"
#include <SDL_endian.h>
#pragma warning(push)
#include "midi/XMidiFile.h"
#include "midi/MidiDriver.h"
#include "midi/IDataSource.h"
#pragma warning(pop)

namespace Detail
{
   /// \brief Input data source for Pentagram code
   /// provides implementation for Pentagram's IDataSource interface to input
   /// .xmi files into XMidiFile.
   /// The SDL_RWops ptr is closed automatically by this class.
   /// \todo reimplement using Base::File
   class InputDataSource : public IDataSource
   {
   public:
      /// ctor
      InputDataSource(Base::SDL_RWopsPtr rwops)
         :m_rwops(rwops)
      {
      }
      /// dtor
      virtual ~InputDataSource()
      {
      }

      /// reads 1 byte
      virtual uint8 read1() override
      {
         uint8 u;
         SDL_RWread(m_rwops.get(), &u, 1, 1);
         return u;
      }

      /// reads 2 bytes, little endian
      virtual uint16 read2() override
      {
         return SDL_ReadLE16(m_rwops.get());
      }

      /// reads 2 bytes, big endian
      virtual uint16 read2high() override
      {
         return SDL_ReadBE16(m_rwops.get());
      }

      /// not implemented
      virtual uint32 read3() override
      {
         UaAssert(false);
         return 0;
      }

      /// reads 4 bytes, little endian
      virtual uint32 read4() override
      {
         return SDL_ReadLE32(m_rwops.get());
      }

      /// reads 4 bytes, big endian
      virtual uint32 read4high() override
      {
         return SDL_ReadBE32(m_rwops.get());
      }

      /// reads memory block
      virtual sint32 read(void *str, sint32 num_bytes) override
      {
         return static_cast<sint32>(SDL_RWread(m_rwops.get(), str, 1, num_bytes));
      }

      /// seeks to file position
      virtual void seek(uint32 pos) override
      {
         SDL_RWseek(m_rwops.get(), pos, SEEK_SET);
      }

      /// skip bytes
      virtual void skip(sint32 delta) override
      {
         SDL_RWseek(m_rwops.get(), delta, SEEK_CUR);
      }

      /// returns size of file
      /// \todo optimize fetching size by storing size value
      virtual uint32 getSize() override
      {
         uint32 curPos = static_cast<uint32>(SDL_RWtell(m_rwops.get()));
         SDL_RWseek(m_rwops.get(), 0L, SEEK_END);
         uint32 u = getPos();
         seek(curPos);
         return u;
      }

      /// returns current file pos
      virtual uint32 getPos() override
      {
         return static_cast<uint32>(SDL_RWtell(m_rwops.get()));
      }

      /// returns if already at end of file
      virtual bool eof() override
      {
         return getPos() >= getSize();
      }

   private:
      /// rwops ptr
      Base::SDL_RWopsPtr m_rwops;
   };

} // namespace Detail

using Audio::MidiPlayer;

/// \todo if necessary, provide support for midi drivers that are sample
/// producers
MidiPlayer::MidiPlayer(const Base::Settings& settings)
{
   // prepare settings
   MidiDriverSettings driverSettings;
   driverSettings.m_iWin32MidiDevice = settings.GetInt(Base::settingWin32MidiDevice);

   MidiDriver* midiDriver = MidiDriver::createInstance("", 22050, true, driverSettings);
   m_midiDriver = std::unique_ptr<MidiDriver>(midiDriver);

   // we can't deal with sample producers, sorry!
   UaAssert(false == midiDriver->isSampleProducer());
}

MidiPlayer::~MidiPlayer()
{
   if (m_midiDriver.get() != NULL)
      m_midiDriver->destroyMidiDriver();
}

void MidiPlayer::PlayFile(Base::SDL_RWopsPtr rwops, bool repeat)
{
   if (m_midiDriver.get() == NULL)
      return;

   Stop();

   Detail::InputDataSource dataSource(rwops);

   XMidiFile* xmidiFile = new XMidiFile(&dataSource, XMIDIFILE_CONVERT_MT32_TO_GM);
   m_xmidiFile = std::auto_ptr<XMidiFile>(xmidiFile);

   // get event list of track 0 (uw .xmi's always only have track 0)
   XMidiEventList* xmiEventList = xmidiFile->GetEventList(0);

   int seqNumber = 0; // always play using sequence 1 (don't use overlapping midi)
   m_midiDriver->startSequence(seqNumber, xmiEventList, repeat, 255);
}

void MidiPlayer::Stop()
{
   if (m_midiDriver.get() != NULL)
      m_midiDriver.get()->finishSequence(0);

   if (m_xmidiFile.get() != NULL)
      m_xmidiFile.release();
}
