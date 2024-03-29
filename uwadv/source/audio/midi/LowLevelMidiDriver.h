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

#ifndef LOWLEVELMIDIDRIVER_H_INCLUDED
#define LOWLEVELMIDIDRIVER_H_INCLUDED

#include "MidiDriver.h"
#include "XMidiSequenceHandler.h"

class XMidiEventList;
class XMidiSequence;

#include <atomic>
#include <queue>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>

//! Specifies the max number of simultaneous playing sequences supported
//! \note Only 2 simultaneous playing sequences required for Ultima 8
#define LLMD_NUM_SEQ	4

//! An Abstract implementation of MidiDriver for Simple Low Level support of Midi playback
//!
//! \note An implementation of LowLevelMidiDriver needs to implement the open(), close()
//!  and send() functions. Implementing increaseThreadPriority() is optional, however it
//!  is strongly recommended that it is implemented. If it's not implemented, the main
//!  Pentagram thread MAY use too much CPU time and cause timing problems.
//!  Similar, implemeting yield() is a good idea too.
class	LowLevelMidiDriver : public MidiDriver, private XMidiSequenceHandler
{
public:
	LowLevelMidiDriver();
	virtual ~LowLevelMidiDriver();

	// MidiDriver Implementation
	virtual int			initMidiDriver(uint32 sample_rate, bool stereo);
	virtual void		destroyMidiDriver();
	virtual int			maxSequences();
	virtual void		setGlobalVolume(int vol);

	virtual void		startSequence(int seq_num, XMidiEventList *list, bool repeat, int vol, int branch = -1);
	virtual void		finishSequence(int seq_num);
	virtual void		pauseSequence(int seq_num);
	virtual void		unpauseSequence(int seq_num);
	virtual void		setSequenceVolume(int seq_num, int vol);
	virtual void		setSequenceSpeed(int seq_num, int speed);
	virtual bool		isSequencePlaying(int seq_num);
	virtual uint32		getSequenceCallbackData(int seq_num);

	virtual void		produceSamples(sint16 *samples, uint32 bytes);

	virtual void		loadTimbreLibrary(IDataSource*, TimbreLibraryType type);

	static bool			precacheTimbresOnStartup;
	static bool			precacheTimbresOnPlay;

protected:

	// Will be wanted by software drivers
	uint32			sample_rate;
	bool			stereo;

	//! Open the Midi Device
	//! \return 0 on sucess. Non zero on failure.
	virtual int			open()=0;

	//! Close the Midi Device
	virtual void		close()=0;

	//! Send a message to the Midi Device
	virtual void		send(uint32 message)=0;

	//! Send a SysEX message to the Midi Device
	//
	// Note that this is slightly different to the API used in ScummVM.
	// The 0xF0 status isn't assumed, and the final 0xF7 also isn't assumed, and is the
	// final byte of the msg buffer. length includes the final byte. The reason for the
	// differences is because the midi specifications can have SysEx messages that does
	// start with 0xF0 and don't end with 0xF7. Chances are though they will never be
	// encountered.
	virtual void		send_sysex(uint8 status, const uint8 *msg, uint16 length) {
	}

	//! Increate the Thread Priority of the Play (current) thread
	virtual void		increaseThreadPriority() { }

	//! Allows LowLevelMidiDrivers to produce samples
	virtual void		lowLevelProduceSamples(sint16 *samples, uint32 num_samples) {
	}

	//! Yield execution of the current thread
	virtual void		yield() { SDL_Delay(1); }

private:
	enum Messages {
		LLMD_MSG_PLAY = 1,
		LLMD_MSG_FINISH = 2,
		LLMD_MSG_PAUSE = 3,
		LLMD_MSG_SET_VOLUME = 4,
		LLMD_MSG_SET_SPEED = 5,
		LLMD_MSG_PRECACHE_TIMBRES = 6,
		// These are only used by thread
		LLMD_MSG_THREAD_INIT = -1,
		LLMD_MSG_THREAD_INIT_FAILED = -2,
		LLMD_MSG_THREAD_EXIT = -3
	};
	struct ComMessage {

		ComMessage(Messages T, int seq) : type(T), sequence(seq) { }
		~ComMessage() {}

		ComMessage(const ComMessage &other)
		{
			type = other.type;
			sequence = other.sequence;
			std::memcpy (&data, &other.data, sizeof(data));
		}

		ComMessage & operator = (const ComMessage &other)
		{
			if (this == &other)
				return *this;
			type = other.type;
			sequence = other.sequence;
			std::memcpy (&data, &other.data, sizeof(data));
			return  *this;
		}

		Messages		type;
		int				sequence;
		union
		{
			struct {
				XMidiEventList	*list;
				bool			repeat;
				int				volume;
				int				branch;
			} play;

			struct {
				bool			paused;
			} pause;

			struct {
				int				level;
			} volume;

			struct {
				int				percentage;
			} speed;

			struct {
				int				code;
			} init_failed;

			struct {
				XMidiEventList	*list;
			} precache;

		} data;
	};

	bool					uploading_timbres;	// Set in 'uploading' timbres mode

	// Communications
	std::queue<ComMessage>	messages;
	SDL_mutex				*mutex;
	SDL_mutex				*cbmutex;
	SDL_cond                *cond;
	sint32					peekComMessageType();
	void					sendComMessage(ComMessage& message);
	void					waitTillNoComMessages();
	void					lockComMessage();
	void					unlockComMessage();

	// State
	bool					playing[LLMD_NUM_SEQ];			// Only set by thread
	sint32					callback_data[LLMD_NUM_SEQ];	// Only set by thread

	// Shared Data
	int						global_volume;
	uint32					xmidi_clock;					// Xmidi clock, returned by getTickCount
	int						chan_locks[16];					// Which seq a chan has been locked by
	int						chan_map[LLMD_NUM_SEQ][16];		// Maps from locked logical chan to phyiscal
	XMidiSequence			*sequences[LLMD_NUM_SEQ];
	int						next_sysex;						// Time we can next send sysex at (is SDL_GetTick() value)

	// Software Synth only Data
	uint32					total_seconds;					// xmidi_clock = total_seconds*6000
	uint32					samples_this_second;			//		+ samples_this_second*6000/sample_rate;
	uint32					samples_per_iteration;

	// Thread Based Only Data
	SDL_Thread				*thread;

	// Timbre Banks
	struct MT32Timbre {
		uint32		time_uploaded;
		int			index;
		bool		protect;
		uint8		timbre[246];
	};
	struct MT32Patch {
		sint8		timbre_bank;			// 0-3	(group A, group B, Memory, Rhythm)
		sint8		timbre_num;				// 0-63
		uint8		key_shift;				// 0-48
		uint8		fine_tune;				// 0-100 (-50 - +50)
		uint8		bender_range;			// 0-24
		uint8		assign_mode;			// 0-3 (POLY1, POLY2, POLY3, POLY4)
		uint8		reverb_switch;			// 0-1 (off,on)
		uint8		dummy;
	};
	static const MT32Patch	mt32_patch_template;
	struct MT32Rhythm {
		uint8		timbre;					// 0-94 (M1-M64,R1-30,OFF)
		uint8		output_level;			// 0-100
		uint8		panpot;					// 0-14 (L-R)
		uint8		reverb_switch;			// 0-1 (off,on)
	};
	struct MT32RhythmSpec {
		int note;
		MT32Rhythm rhythm;
	};


	MT32Patch				**mt32_patch_banks[128];			// 128 banks, of 128 Patches
	MT32Timbre				**mt32_timbre_banks[128];			// 128 banks, of 128 Timbres
	MT32Rhythm				*mt32_rhythm_bank[128];				// 1 bank of rhythm
	int						mt32_timbre_used[64][2];
	int						mt32_bank_sel[LLMD_NUM_SEQ][16];
	int						mt32_patch_bank_sel[128];

	void					loadXMidiTimbreLibrary(IDataSource *ds);
	void					extractTimbreLibrary(XMidiEventList *eventlist);
    void					uploadTimbre(int bank, int timbre);
	void					setPatchBank(int bank, int patch);
	void					loadRhythm(const MT32Rhythm &rhythm, int note);
	void					loadRhythmTemp(int temp);
	void					sendMT32SystemMessage(uint32 address_base, uint16 address_offset, uint32 len, const void *data);

	// Shared Methods

	//! Play all sequences, handle communications requests
	//! /return true if terminating
	bool					playSequences();

	// Thread Methods
	int						initThreadedSynth();
	void					destroyThreadedSynth();
	static int				threadMain_Static(void *data);
	int						threadMain();
	// Thread flag -- set to true when ready to quit
	std::atomic<bool>		quit_thread;

	// Software methods
	int						initSoftwareSynth();
	void					destroySoftwareSynth();

	// XMidiSequenceHandler implementation
	virtual void			sequenceSendEvent(uint16 sequence_id, uint32 message);
	virtual void			sequenceSendSysEx(uint16 sequence_id, uint8 status, const uint8 *msg, uint16 length);
	virtual uint32			getTickCount(uint16 sequence_id);
	virtual void			handleCallbackTrigger(uint16 sequence_id, uint8 data);

	int						protectChannel(uint16 sequence_id, int chan, bool protect);
	int						lockChannel(uint16 sequence_id, int chan, bool lock);

	int						unlockAndUnprotectChannel(uint16 sequence_id);

	//! Mute all phyisical channels
	void					muteAllChannels();
};

#endif //LOWLEVELMIDIDRIVER_H_INCLUDED
