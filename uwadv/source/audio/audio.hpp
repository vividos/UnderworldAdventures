/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

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
/*! \file audio.hpp

   \brief audio manager definition

   The audio manager lets the user start and stop playing music tracks, sound
   effects and *.voc sounds in the background. music tracks can be repeated
   indefinitely.

   To use the class, just call init() at program start and then call the play
   functions to play music, sfx or *.voc files.

   - play_sound(soundname) plays the file %uw-path%/sound/<soundname>.voc
   - play_sfx() takes an enum defined below
   - start_music() takes a playlist index; playlists are loaded from
                   %uadata%<game-prefix>/audio/music.m3u.

   The playlist is in m3u format, can contain comment lines starting with #
   and playlist entries can also contain the placeholders %uw-path%, %uahome%
   and %uadata%.

   Be sure to only use the ua_audio_manager object only once, since it uses
   the audio-part of SDL and SDL_mixer to play back audio.

*/

// include guard
#ifndef uwadv_audio_hpp_
#define uwadv_audio_hpp_

// needed includes


// forward references
class ua_files_manager;
class ua_settings;
class ua_midi_player;
typedef struct _Mix_Music Mix_Music;


// enums

//! enumeration of all sound effects
enum ua_audio_sfx_type
{
   ua_sfx_steps=0,
   // TODO add more
};


// classes

//! audio interface class
class ua_audio_manager
{
public:
   //! ctor
   ua_audio_manager();
   //! dtor
   ~ua_audio_manager();

   //! initializes audio
   void init(ua_settings& settings, ua_files_manager& filesmgr);

   //! plays a sound; stops when finished
   void play_sound(const char* soundname);

   //! stops sound playback
   void stop_sound();

   //! plays a special effect sound
   void play_sfx(ua_audio_sfx_type sfx);

   //! starts music playback
   void start_music(unsigned int music, bool repeat);

   //! fades out currently playing music track; fadeout time in seconds
   void fadeout_music(double time);

   //! stops music playback
   void stop_music();

protected:
   //! loads music playlist
   void load_playlist(ua_settings& settings, ua_files_manager& filesmgr,
      const char* filename);

   //! frees audio chunk when channel stops playing (callback function)
   static void ua_audio_manager::mixer_channel_finished(int channel);

protected:

   //! midi player
   ua_midi_player* midipl;

   //! playlist with all music files
   std::vector<std::string> music_playlist;

   //! path to uw base folder
   std::string uw_path;

   //! current music track
   Mix_Music* curtrack;
};

#endif
