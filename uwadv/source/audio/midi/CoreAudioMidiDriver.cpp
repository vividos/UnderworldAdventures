/*
Copyright (C) 2003  The Pentagram Team

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

#include "pent_include.h"
#include "CoreAudioMidiDriver.h"

#ifdef USE_CORE_AUDIO_MIDI

#include <pthread.h>
#include <sched.h>


// Enable the following switch to make ScummVM try to use native MIDI hardware
// on your computer for MIDI output. This is currently quite hackish, in 
// particular you have no way to specify which device is used (it just always
// uses the first output device it can find), nor is there a switch to
// force it to use the soft synth instead of the MIDI HW.
//#define ENABLE_HACKISH_NATIVE_MIDI_SUPPORT 1


const MidiDriver::MidiDriverDesc CoreAudioMidiDriver::desc = 
		MidiDriver::MidiDriverDesc ("CoreAudio", createInstance);

CoreAudioMidiDriver::CoreAudioMidiDriver() : 
	au_MusicDevice(0), au_output(0),
	mClient(0), mOutPort(0), mDest(0)
{
	OSStatus err;
	err = MIDIClientCreate(CFSTR("Pentagram MIDI Driver for OS X"), NULL, NULL, &mClient);

}

CoreAudioMidiDriver::~CoreAudioMidiDriver()
{
	if (mClient)
		MIDIClientDispose(mClient);
	mClient = 0;
}

int CoreAudioMidiDriver::open()
{
	if (au_output || mDest)
		return 1;

	OSStatus err = noErr;

	mOutPort = 0;
#ifdef ENABLE_HACKISH_NATIVE_MIDI_SUPPORT
	int dests = MIDIGetNumberOfDestinations();
	if (dests > 0 && mClient) {
		mDest = MIDIGetDestination(0);
		err = MIDIOutputPortCreate( mClient, 
									CFSTR("scummvm_output_port"), 
									&mOutPort);
	}
#endif

	if (err != noErr || !mOutPort) {
		AudioUnitConnection auconnect;
		ComponentDescription compdesc;
		Component compid;
	
		// Open the Music Device
		compdesc.componentType = kAudioUnitComponentType;
		compdesc.componentSubType = kAudioUnitSubType_MusicDevice;
		compdesc.componentManufacturer = kAudioUnitID_DLSSynth;
		compdesc.componentFlags = 0;
		compdesc.componentFlagsMask = 0;
		compid = FindNextComponent(NULL, &compdesc);
		au_MusicDevice = static_cast<AudioUnit>(OpenComponent(compid));
	
		// open the output unit
		au_output = static_cast<AudioUnit>(OpenDefaultComponent(kAudioUnitComponentType, kAudioUnitSubType_Output));
	
		// connect the units
		auconnect.sourceAudioUnit = au_MusicDevice;
		auconnect.sourceOutputNumber = 0;
		auconnect.destInputNumber = 0;
		err =
			AudioUnitSetProperty(au_output, kAudioUnitProperty_MakeConnection, kAudioUnitScope_Input, 0,
													 static_cast<void*>(&auconnect), sizeof(AudioUnitConnection));
	
		// initialize the units
		AudioUnitInitialize(au_MusicDevice);
		AudioUnitInitialize(au_output);
	
		// start the output
		AudioOutputUnitStart(au_output);
	}

	return 0;
}

void CoreAudioMidiDriver::close()
{
	if (mOutPort && mDest) {
		MIDIPortDispose(mOutPort);
		mOutPort = 0;
		mDest = 0;
	} else {
		// Stop the output
		AudioOutputUnitStop(au_output);
	
		// Cleanup
		CloseComponent(au_output);
		au_output = 0;
		CloseComponent(au_MusicDevice);
		au_MusicDevice = 0;
	}
}

void CoreAudioMidiDriver::send(uint32 message)
{
	uint8 status_byte = (message & 0x000000FF);
	uint8 first_byte = (message & 0x0000FF00) >> 8;
	uint8 second_byte = (message & 0x00FF0000) >> 16;

	if (mOutPort && mDest) {
		MIDIPacketList packetList;
		MIDIPacket *packet = &packetList.packet[0];
		
		packetList.numPackets = 1;
	
		packet->timeStamp = 0;
		packet->length = 3;
		packet->data[0] = status_byte;
		packet->data[1] = first_byte;
		packet->data[2] = second_byte;
	
		MIDISend(mOutPort, mDest, &packetList);
	} else {
		assert(au_output != NULL);
		assert(au_MusicDevice != NULL);
		MusicDeviceMIDIEvent(au_MusicDevice, status_byte, first_byte, second_byte, 0);
	}
}

void CoreAudioMidiDriver::send_sysex (uint8 status, const uint8 *msg, uint16 length)
{
	uint8 buf[384];

	if (mOutPort && mDest) {
		MIDIPacketList *packetList = (MIDIPacketList *)buf;
		MIDIPacket *packet = packetList->packet;

		assert(sizeof(buf) >= sizeof(UInt32) + sizeof(MIDITimeStamp) + sizeof(UInt16) + length + 2);
		
		packetList->numPackets = 1;

		packet->timeStamp = 0;
		
		// Add SysEx frame
		packet->length = length + 2;
		packet->data[0] = status;
		memcpy(packet->data + 1, msg, length);
		packet->data[length + 1] = 0xF7;
	
		MIDISend(mOutPort, mDest, packetList);
	} else {
		assert(sizeof(buf) >= (size_t)length + 2);
		assert(au_output != NULL);
		assert(au_MusicDevice != NULL);

		// Add SysEx frame
		buf[0] = status;
		memcpy(buf+1, msg, length);
		buf[length+1] = 0xF7;

		MusicDeviceSysEx(au_MusicDevice, buf, length + 2);
	}
}

void CoreAudioMidiDriver::increaseThreadPriority()
{
	pthread_t self;
	int policy;
	struct sched_param param;
	
	self = pthread_self();
	pthread_getschedparam(self, &policy, &param);
	param.sched_priority = sched_get_priority_max(policy);
	pthread_setschedparam(self, policy, &param);
}

void CoreAudioMidiDriver::yield()
{
	sched_yield();
}

#endif //MACOSX
