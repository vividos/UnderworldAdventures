/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2002,2003,2004,2005,2006 Michael Fink

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
/*! \file midiplayer.cpp

   \brief midi playback class implementation

*/

// needed includes
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

//! Input data source for Pentagram code
/*! provides implementation for Pentagram's IDataSource interface to input
    .xmi files into XMidiFile.
    The SDL_RWops ptr is closed automatically by this class.
    \todo reimplement using Base::File
*/
class InputDataSource: public IDataSource
{
public:
   //! ctor
   InputDataSource(Base::SDL_RWopsPtr rwops):m_rwops(rwops){}
   //! dtor
	virtual ~InputDataSource(){}

   //! reads 1 byte
   virtual uint8 read1(){ uint8 u; SDL_RWread(m_rwops.get(), &u, 1, 1); return u; }
   //! reads 2 bytes, little endian
	virtual uint16 read2(){ return SDL_ReadLE16(m_rwops.get()); }
   //! reads 2 bytes, big endian
	virtual uint16 read2high(){ return SDL_ReadBE16(m_rwops.get()); }
   //! not implemented
	virtual uint32 read3(){ UaAssert(0); return 0; }
   //! reads 4 bytes, little endian
	virtual uint32 read4(){ return SDL_ReadLE32(m_rwops.get()); }
   //! reads 4 bytes, big endian
	virtual uint32 read4high(){ return SDL_ReadBE32(m_rwops.get()); }
   //! reads memory block
   virtual sint32 read(void *str, sint32 num_bytes)
   {
      return static_cast<sint32>(SDL_RWread(m_rwops.get(), str, 1, num_bytes));
   }
   //! seeks to file position
   virtual void seek(uint32 pos){ SDL_RWseek(m_rwops.get(), pos, SEEK_SET); }
   //! skip bytes
   virtual void skip(sint32 delta){ SDL_RWseek(m_rwops.get(), delta, SEEK_CUR); }
   //! returns size of file
   /*! \todo optimize fetching size by storing size value */
   virtual uint32 getSize()
   {
      uint32 uCurPos = SDL_RWtell(m_rwops.get());
      SDL_RWseek(m_rwops.get(), 0L, SEEK_END);
      uint32 u = getPos();
      seek(uCurPos);
      return u;
   }
   //! returns current file pos
	virtual uint32 getPos()
   {
      return static_cast<uint32>(SDL_RWtell(m_rwops.get()));
   }
   //! returns if already at end of file
   virtual bool eof()
   {
      return getPos() >= getSize();
   }

private:
   //! rwops ptr
   Base::SDL_RWopsPtr m_rwops;
};

} // namespace Detail

using Audio::MidiPlayer;

// MidiPlayer methods

/*! \todo if necessary, provide support for midi drivers that are sample
    producers
*/
MidiPlayer::MidiPlayer(const Base::Settings& settings)
{
   // prepare settings
   MidiDriverSettings driverSettings;
   driverSettings.m_iWin32MidiDevice = settings.GetInt(Base::settingWin32MidiDevice);

   MidiDriver* pDriver = MidiDriver::createInstance("", 22050, true, driverSettings);
   m_apDriver = std::auto_ptr<MidiDriver>(pDriver);

   // we can't deal with sample producers, sorry!
   UaAssert(false == pDriver->isSampleProducer());
}

MidiPlayer::~MidiPlayer()
{
   if (m_apDriver.get() != NULL)
      m_apDriver->destroyMidiDriver();
}

void MidiPlayer::PlayFile(Base::SDL_RWopsPtr rwops, bool bRepeat)
{
   if (m_apDriver.get() == NULL)
      return;

   Stop();

   Detail::InputDataSource dataSource(rwops);

   XMidiFile* xmiFile = new XMidiFile(&dataSource, XMIDIFILE_CONVERT_MT32_TO_GM);
   m_apXMidiFile = std::auto_ptr<XMidiFile>(xmiFile);

   // get event list of track 0 (uw .xmi's always only have track 0)
   XMidiEventList* xmiEventList = xmiFile->GetEventList(0);

   int iSeqNumber = 0; // always play using sequence 1 (don't use overlapping midi)
   m_apDriver->startSequence(iSeqNumber, xmiEventList, bRepeat, 255); 
}

void MidiPlayer::Stop()
{
   if (m_apDriver.get() != NULL)
      m_apDriver.get()->finishSequence(0);

   if (m_apXMidiFile.get() != NULL)
      m_apXMidiFile.release();
}
