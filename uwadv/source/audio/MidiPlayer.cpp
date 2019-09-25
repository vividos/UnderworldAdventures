//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Underworld Adventures Team
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
/// \file MidiPlayer.cpp
/// \brief midi playback class implementation
//
#include "pch.hpp"
#include "MidiPlayer.hpp"
#include "Settings.hpp"
#pragma warning(push)
#include "midi/XMidiFile.h"
#include "midi/MidiDriver.h"
#pragma warning(pop)
#include "IODataSource.hpp"

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

void MidiPlayer::LoadTimbreLibrary(Base::SDL_RWopsPtr rwops, bool isMT) const
{
   Detail::InputDataSource dataSource(rwops);

   m_midiDriver->loadTimbreLibrary(&dataSource,
      isMT ? MidiDriver::TIMBRE_LIBRARY_XMIDI_MT : MidiDriver::TIMBRE_LIBRARY_XMIDI_AD);
}

void MidiPlayer::PlayFile(Base::SDL_RWopsPtr rwops, bool repeat)
{
   if (m_midiDriver.get() == NULL)
      return;

   Stop();

   Detail::InputDataSource dataSource(rwops);

   XMidiFile* xmidiFile = new XMidiFile(&dataSource, XMIDIFILE_CONVERT_MT32_TO_GM);
   m_xmidiFile = std::unique_ptr<XMidiFile>(xmidiFile);

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
