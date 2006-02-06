/*
Copyright (C) 2003-2005  The Pentagram Team

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

#ifndef COREAUDIOMIDIDRIVER_H_INCLUDED
#define COREAUDIOMIDIDRIVER_H_INCLUDED

#ifdef MACOSX
#define USE_CORE_AUDIO_MIDI

#include "LowLevelMidiDriver.h"

#include <AudioUnit/AudioUnit.h>
#include <CoreMIDI/CoreMIDI.h>7

class CoreAudioMidiDriver : public LowLevelMidiDriver
{
	AudioUnit au_MusicDevice;
	AudioUnit au_output;

	MIDIClientRef	mClient;
	MIDIPortRef		mOutPort;
	MIDIEndpointRef	mDest;

	static const MidiDriverDesc	desc;
	static MidiDriver *createInstance() {
		return new CoreAudioMidiDriver();
	}

public:
	static const MidiDriverDesc* getDesc() { return &desc; }

	CoreAudioMidiDriver();
	~CoreAudioMidiDriver();

protected:
	virtual int			open();
	virtual void		close();
	virtual void		send(uint32 message);
	virtual void		send_sysex(uint8 status, const uint8 *msg, uint16 length);
	virtual void		increaseThreadPriority();
	virtual void		yield();
};

#endif //MACOSX

#endif //COREAUDIOMIDIDRIVER_H_INCLUDED
