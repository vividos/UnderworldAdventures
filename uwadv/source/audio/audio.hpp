/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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

*/
/*! \defgroup audio Audio Components

   The audio subsystem takes care of all tasks that have to do with playing
   back the sound track, sound effects (such as steps when walking) and the
   cutscene speech.

   The audio subsystem can be controlled via the ua_audio_manager class that
   has functions to do these tasks. Sound track pieces are played back using
   a playlist loaded at initialisation; they can be faded out, e.g. when a
   screen ends. Sound effects are referenced by the enum ua_audio_sfx_type.
   Cutscene speech is played back from uw's .voc files; they are internally
   resampled to 22050 Hz because they are in an unusual format.

   The Audio Components Module depends on the Base Components Module.
*/
//@{

// include guard
#ifndef uwadv_audio_hpp_
#define uwadv_audio_hpp_

// needed includes


// forward references
class ua_files_manager;
class ua_settings;
class ua_midi_player;
//! forward definition for the SDL_Mixer data structure
typedef struct _Mix_Music Mix_Music;


// enums

//! all uw1 music tracks
/*! The enum value can be used in a call to ua_audio_manager::start_music()
    and reflects the position in the music playlist, not the actual filenames
    in the sound folder */
enum ua_music_track_uw1
{
   ua_music_uw1_introduction=0,
   ua_music_uw1_dark_abyss,
   ua_music_uw1_descent,
   ua_music_uw1_wanderer,
   ua_music_uw1_battlefield,
   ua_music_uw1_combat,
   ua_music_uw1_injured,
   ua_music_uw1_armed,
   ua_music_uw1_victory,
   ua_music_uw1_death,
   ua_music_uw1_fleeing,
   ua_music_uw1_maps_legends
};


//! enumeration of all sound effects
/*! \todo add all needed audio sfx types */
enum ua_audio_sfx_type
{
   ua_sfx_steps=0,
};


// classes

//! audio interface class
/*! The audio manager lets the user start and stop playing music tracks, sound
    effects and *.voc sounds in the background. music tracks can be repeated
    indefinitely.

    To use the class, just call init() at program start and then call the play
    functions to play music, sfx or *.voc files.

    - play_sound(soundname) plays the file %uw-path%/sound/{soundname}.voc
    - play_sfx() takes an enum defined below
    - start_music() takes a playlist index; playlists are loaded from
                    %uadata%/{game-prefix}/audio/music.m3u.

    The playlist is in m3u format, can contain comment lines starting with #
    and playlist entries can also contain the placeholders %uw-path%, %uahome%
    and %uadata%.

    Be sure to only use the ua_audio_manager object only once, since it uses
    the audio-part of SDL and SDL_mixer to play back audio.
*/
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
//@}
