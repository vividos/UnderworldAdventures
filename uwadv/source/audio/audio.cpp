/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

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

   audio interface implementation does an the interfacing with the audio part
   of SDL and the ua_midi_player class.

*/

// needed includes
#include "common.hpp"
#include "audio.hpp"
#include "midi.hpp"
#include "SDL_mixer.h"
#include <string>
#include <algorithm>


// classes

//! audio implementation class
class ua_audio_impl: public ua_audio_interface
{
public:
   //! ctor
   ua_audio_impl();
   //! dtor
   virtual ~ua_audio_impl();

   //! initializes audio
   void init(ua_settings &settings, ua_files_manager &filesmgr);

   //! plays a sound; stops when finished
   void play_sound(const char *soundname);

   //! stops sound playback
   void stop_sound();

   //! starts music playback
   void start_music(unsigned int music, bool repeat);

   //! stops music playback
   void stop_music();

protected:
   //! loads music playlist
   void load_playlist(ua_settings &settings, ua_files_manager &filesmgr,
      const char *filename);

   //! frees audio chunk when channel stops playing (callback function)
   static void mixer_channel_finished(int channel)
   {
      Mix_Chunk* mc = Mix_GetChunk(channel);
      if (mc)
         Mix_FreeChunk(mc);
   }

protected:
   //! midi player
   ua_midi_player midipl;

   //! playlist with all music files
   std::vector<std::string> music_playlist;

   //! path to uw base folder
   std::string uw_path;

   Mix_Music* curtrack;
};


// ua_audio_interface methods

ua_audio_interface *ua_audio_interface::get_audio_interface()
{
   // creates a new audio object
   return new ua_audio_impl;
}


// ua_audio_impl methods

ua_audio_impl::ua_audio_impl()
:curtrack(NULL)
{
}

ua_audio_impl::~ua_audio_impl()
{
   stop_sound();
   stop_music();

   Mix_CloseAudio();
   SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void ua_audio_impl::init(ua_settings &settings, ua_files_manager &filesmgr)
{
   // init mixer
   SDL_Init(SDL_INIT_AUDIO);
   Mix_OpenAudio(12000, MIX_DEFAULT_FORMAT, 2, 4096);
   Mix_ChannelFinished(mixer_channel_finished);

   midipl.init_player(settings);
   midipl.init_driver();

   load_playlist(settings,filesmgr,"uw1/audio/music.m3u");

   uw_path = settings.get_string(ua_setting_uw_path);
}

void ua_audio_impl::play_sound(const char *soundname)
{
   // construct filename
   std::string vocname(uw_path);
   vocname.append("sound/");
   vocname.append(soundname);
   vocname.append(".voc");

   // start playing
   Mix_Chunk* mc = Mix_LoadWAV(vocname.c_str());
   if (mc)
      Mix_PlayChannel(-1, mc, 0);
}

void ua_audio_impl::stop_sound()
{
   Mix_HaltChannel(-1);
}

void ua_audio_impl::start_music(unsigned int music, bool repeat)
{
   if (music>=music_playlist.size()) return;

   std::string trackname = music_playlist[music];

   // make lowercase
   std::transform(trackname.begin(),trackname.end(),trackname.begin(),::tolower);

   // check for midi tracks
   if (trackname.find(".xmi") != std::string::npos ||
       trackname.find(".mid") != std::string::npos)
   {
      // start midi player
      midipl.start_track(trackname.c_str(),0,repeat);
   }
   else
   {
      if (curtrack)
         Mix_FreeMusic(curtrack);

      // start music track via SDL_mixer
      curtrack = Mix_LoadMUS(trackname.c_str());
      if (curtrack)
         Mix_PlayMusic(curtrack, 0);
   }
}

void ua_audio_impl::stop_music()
{
   if (curtrack)
   {
      Mix_FreeMusic(curtrack);
      curtrack = NULL;
   }

   midipl.stop_track();
}

void ua_audio_impl::load_playlist(ua_settings &settings,
   ua_files_manager &filesmgr, const char *filename)
{
   SDL_RWops *m3u = filesmgr.get_uadata_file(filename);

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

      // replace %var% variables
      pos = line.find("%uw-path%");
      if (pos != std::string::npos)
         line.replace(pos,9,settings.get_string(ua_setting_uw_path));

      pos = line.find("%uadata%");
      if (pos != std::string::npos)
         line.replace(pos,8,settings.get_string(ua_setting_uadata_path));

      // add to playlist
      music_playlist.push_back(line);
   }
}
