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
/*! \file audio.cpp

   \brief audio interface implementation

   audio manager implementation; does the audio music and sound playback using
   the audio part of SDL and the ua_midi_player class.

*/

// needed includes
#include "common.hpp"
#include "audio.hpp"
#include "settings.hpp"
#include "files.hpp"
#include "midi.hpp"
#include "SDL_mixer.h"
#include <string>


// external functions

//! resamples voc file recorded at 12000 Hz to (approximately) 22050 Hz
extern void ua_audio_resample_voc(Mix_Chunk* mc);


// ua_audio_manager methods

ua_audio_manager::ua_audio_manager()
:midipl(NULL), curtrack(NULL)
{
}

/*! Shuts down the SDL and SDL_mixer audio subsystem and deletes the midi
    player instance.
*/
ua_audio_manager::~ua_audio_manager()
{
   stop_sound();
   stop_music();

   Mix_CloseAudio();
   SDL_QuitSubSystem(SDL_INIT_AUDIO);

   delete midipl;
   midipl = NULL;
}

/*! Initializes the audio manager using SDL and SDL_mixer. Creates the midi
    player and loads the sound track playlist.

    \param settings settings object
    \param filesmgr files manager object
*/
void ua_audio_manager::init(ua_settings& settings, ua_files_manager& filesmgr)
{
   ua_trace("init audio subsystem ... ");

   if (!settings.get_bool(ua_setting_audio_enabled))
   {
      ua_trace("disabled by settings\n\n");
      return;
   }

   // init mixer
   SDL_Init(SDL_INIT_AUDIO);
   Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096);
   Mix_ChannelFinished(mixer_channel_finished);

   ua_trace(" %s\n", Mix_GetError());

   // create midi player
   midipl = new ua_midi_player;
   midipl->init_player(settings);
   midipl->init_driver();

   // get uw path for future use
   uw_path = settings.get_string(ua_setting_uw_path);

   // do playlist pathname
   std::string playlist_name(settings.get_string(ua_setting_game_prefix));
   playlist_name.append("/audio/music.m3u");

   load_playlist(settings,filesmgr,playlist_name.c_str());

   // print out SDL_mixer version
   const SDL_version* mixer_ver = Mix_Linked_Version();
   ua_trace("using SDL_mixer version %u.%u.%u\n\n",
      mixer_ver->major, mixer_ver->minor, mixer_ver->patch);
}

/*! Plays back a cutscene sound stored in a .voc file. The file resides in
    the "sound" folder of uw. The audio samples are resampled to 22050 Hz
    then. The soundname parameter is used to determine the filename:
    %uwpath%/sound/{soundname}.voc

    \param soundname base name of the sound file to play back
*/
void ua_audio_manager::play_sound(const char* soundname)
{
   // construct filename
   std::string vocname(uw_path);
   vocname.append("sound/");
   vocname.append(soundname);
   vocname.append(".voc");

   // start playing
   Mix_Chunk* mc = Mix_LoadWAV(vocname.c_str());
   if (mc)
   {
      // note: Mix_LoadWAV() doesn't resample the waveform to the specified
      // mixer samplerate, so we must do it by ourselves
      ua_audio_resample_voc(mc);

      Mix_PlayChannel(-1, mc, 0);
   }
}

/*! stops a cutscene sound that is currently playing back. */
void ua_audio_manager::stop_sound()
{
   Mix_HaltChannel(-1);
}

/*! Plays back a sound effect specified. The playback stops when the sfx is
    finished.

    \param sfx sound effect type to play back

    \todo implement
*/
void ua_audio_manager::play_sfx(ua_audio_sfx_type sfx)
{
}

/*! Starts playing back a sound track from the music playlist. Midi files with
    extensions .mid or .xmi are played back using the appropriate midi driver
    through ua_midi_player. Other file types are tried to load via SDL_mixer,
    with the function Mix_LoadMUS(), so all music types that the SDL_mixer.dll
    supports can be played back. The distributed SDL_mixer.dll can only play
    back Ogg Vorbis and some tracker formats. mp3 files are not supported,
    since SMPEG is not linked it (it's an ancient format anyway).

    \param music the position in music playlist of the track to play back
    \param repeat indicates if track should be repeated when it has stopped
*/
void ua_audio_manager::start_music(unsigned int music, bool repeat)
{
   if (music>=music_playlist.size()) return;

   std::string trackname = music_playlist[music];

   ua_trace("audio: playing back %s",trackname.c_str());

   // find extension
   std::string ext;
   std::string::size_type pos = trackname.find_last_of('.');

   if (pos != std::string::npos)
      ext.assign(trackname.substr(pos));

   // make extension lowercase
   ua_str_lowercase(ext);

   // check for midi tracks
   if (ext.find(".xmi") != std::string::npos ||
       ext.find(".mid") != std::string::npos)
   {
      // start midi player
      midipl->start_track(trackname.c_str(),0,repeat);
   }
   else
   {
      if (curtrack)
         Mix_FreeMusic(curtrack);

      // start music track via SDL_mixer
      curtrack = Mix_LoadMUS(trackname.c_str());
      if (curtrack)
         Mix_PlayMusic(curtrack, repeat ? -1 : 0);
      else
         ua_trace(" (%s)",Mix_GetError());
   }
   ua_trace("\n");
}

/*! Fades out the currently playing track using the time specified.
    The method currently only fades out tracks playing back using SDL_mixer.
    Other midi drivers are not supported.

    \param time time to fade out music to null volume
*/
void ua_audio_manager::fadeout_music(double time)
{
   Mix_FadeOutMusic(static_cast<int>(time*1000.0));
}

/*! Stops the current music track, either played back by SDL_mixer or the
    current midi driver.
*/
void ua_audio_manager::stop_music()
{
   if (curtrack)
   {
      Mix_FreeMusic(curtrack);
      curtrack = NULL;
   }

   if (midipl)
      midipl->stop_track();
}

/*! Loads the music soundtrack playlist from the given filename. The file can
    be stored in the uadata resource file or in the equivalent path. The file
    can contain, among the placeholders recognized by
    ua_files_manager::replace_system_vars(), the placeholder %uw-path%
    that specifies the current uw path.

    \param settings settings object
    \param filesmgr files manager object
    \param filename playlist filename

    \todo maybe use ua_cfgfile to load playlist?
*/
void ua_audio_manager::load_playlist(ua_settings& settings,
   ua_files_manager& filesmgr, const char* filename)
{
   SDL_RWops* m3u = filesmgr.get_uadata_file(filename);
   if (m3u==NULL) return;

   // load playlist into buffer
   std::vector<char> buffer;
   unsigned int len=0;
   {
      SDL_RWseek(m3u,0,SEEK_END);
      len = SDL_RWtell(m3u);
      SDL_RWseek(m3u,0,SEEK_SET);

      buffer.resize(len+1,0);

      SDL_RWread(m3u,&buffer[0],len,1);
      buffer[len]=0;
   }
   SDL_RWclose(m3u);

   // convert to string
   std::string playlist(&buffer[0]);
   playlist.append("\n"); // end guard

   std::string::size_type pos;

   while( std::string::npos != (pos = playlist.find('\n')) )
   {
      // extract one line
      std::string line;
      line.assign(playlist.c_str(),pos);
      playlist.erase(0,pos+1);

      // get rid of comments and newlines
      pos = line.find_first_of("#\n\r");
      if (pos != std::string::npos)
         line.erase(pos);

      // check for empty line
      if (line.empty()) continue;

      filesmgr.replace_system_vars(line);

      // additionally replace %uw-path%
      pos = line.find("%uw-path%");
      if (pos != std::string::npos)
         line.replace(pos,9,uw_path);

      // add to playlist
      music_playlist.push_back(line);
   }
}

/*! Callback function to get notified when a digital audio channel has
    finished playing back; frees audio chunk played back.

    \param channel channel number that stops playback
*/
void ua_audio_manager::mixer_channel_finished(int channel)
{
   Mix_Chunk* mc = Mix_GetChunk(channel);
   if (mc != NULL)
      Mix_FreeChunk(mc);
}
