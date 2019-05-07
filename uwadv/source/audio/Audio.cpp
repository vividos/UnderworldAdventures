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
/// \file Audio.cpp
/// \brief audio manager implementation
/// \details audio manager implementation; does the audio music and sound
/// playback using SDL and SDL_mixer and the midi classes borrowed from
/// Pentagram.
//
#include "pch.hpp"
#include "Audio.hpp"
#include "Playlist.hpp"
#include "Base.hpp"
#include "Settings.hpp"
#include "ResourceManager.hpp"
#include <SDL.h>
#include <SDL_mixer.h>
#include "MidiPlayer.hpp"
#include "VoiceFile.hpp"
#include "String.hpp"
#include "File.hpp"

namespace Detail
{
   /// \brief internal audio manager data
   class AudioManagerData
   {
   public:
      /// ctor
      AudioManagerData(const Base::Settings& settings, const Base::ResourceManager& resourceManager)
         :m_midiPlayer(settings),
         m_currentTrack(NULL),
         m_resourceManager(resourceManager)
      {
      }

      /// returns current music track
      Mix_Music*& GetCurrentTrack() { return m_currentTrack; }

      /// returns track data for current music track
      Base::SDL_RWopsPtr& GetCurrentMusicTrackData()
      {
         return m_currentMusicTrackData;
      }

      /// returns resource manager
      const Base::ResourceManager& GetResourceManager() const { return m_resourceManager; }

      /// returns playlist
      Audio::Playlist& GetPlaylist() { return m_playlist; }

      /// returns midi player
      Audio::MidiPlayer& GetMidiPlayer() { return m_midiPlayer; }

   private:
      /// midi player
      Audio::MidiPlayer m_midiPlayer;

      /// playlist
      Audio::Playlist m_playlist;

      /// resource manager
      const Base::ResourceManager& m_resourceManager;

      /// current music track
      Mix_Music* m_currentTrack;

      /// current music track data
      Base::SDL_RWopsPtr m_currentMusicTrackData;

      /// path to current uw game
      std::string m_underworldPath;
   };

   /// \brief frees audio chunk when channel stops playing (callback function)
   /// Callback function to get notified when a digital audio channel has
   /// finished playing back; frees audio chunk.
   /// \param iChannel channel number that stops playback
   void MixerChannelFinished(int channel)
   {
      Mix_Chunk* mc = Mix_GetChunk(channel);
      if (mc != NULL)
         Mix_FreeChunk(mc);
   }

}

using Audio::AudioManager;

/// Initializes audio subsystem using SDL and SDL_mixer. Note that game type
/// must have been set to properly load the playlist.
AudioManager::AudioManager(const Base::Settings& settings, const Base::ResourceManager& resourceManager)
   :m_data(new Detail::AudioManagerData(settings, resourceManager))
{
   UaAssert(m_data.get() != NULL);

   UaTrace("init audio subsystem ... ");

   if (!settings.GetBool(Base::settingAudioEnabled))
   {
      UaTrace("disabled\n");
      return;
   }

   // init SDL audio subsystem and SDL_mixer
   {
      SDL_Init(SDL_INIT_AUDIO);
      Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096);
      Mix_ChannelFinished(Detail::MixerChannelFinished);

      UaTrace(" %s\n", Mix_GetError());

      // print out SDL_mixer version
      const SDL_version* mixerVersion = Mix_Linked_Version();
      UaTrace("using SDL_mixer %u.%u.%u\n",
         mixerVersion->major, mixerVersion->minor, mixerVersion->patch);

      int iSampleFrequency = 0;
      Uint16 uiFormat = 0;
      int iChannels = 0;
      Mix_QuerySpec(&iSampleFrequency, &uiFormat, &iChannels);

      UaTrace("samplerate: %u Hz, %u channels, format: %s\n",
         iSampleFrequency, iChannels,
         uiFormat == AUDIO_U8 ? "U8" :
         uiFormat == AUDIO_S8 ? "S8" :
         uiFormat == AUDIO_U16LSB ? "U16LSB" :
         uiFormat == AUDIO_S16LSB ? "S16LSB" :
         uiFormat == AUDIO_U16MSB ? "U16MSB" :
         uiFormat == AUDIO_S16MSB ? "S16MSB" : "unknown");
   }

   // load playlist
   {
      std::string playlistFilename =
         settings.GetString(Base::settingGamePrefix) + "/audio/music.m3u";

      Base::SDL_RWopsPtr rwops = resourceManager.GetResourceFile(playlistFilename);
      if (rwops.get() != NULL)
         m_data->GetPlaylist() = Playlist(settings, rwops);
   }

   UaTrace("loading timbre library from file sound/uw.mt...\n");

   Base::SDL_RWopsPtr rwops = m_data->GetResourceManager().GetUnderworldFile(Base::resourceGameUw, "sound/uw.mt");
   m_data->GetMidiPlayer().LoadTimbreLibrary(rwops, true);

   UaTrace("\n");
}

/// Shuts down SDL audio subsystem and SDL_mixer.
AudioManager::~AudioManager()
{
   StopSound();
   StopMusic();

   Mix_CloseAudio();
   SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

/// Plays back a cutscene sound stored in a .voc file in the "sound" folder of
/// the current game. The audio samples are properly resampled for SDL_mixer
/// to process. The soundName parameter is used to determine the filename:
/// %uwpath%/sound/{soundName}.voc
/// \param soundName base name of the sound file to play back
void AudioManager::PlaySound(const std::string& soundName)
{
   UaAssert(m_data.get() != NULL);

   // construct filename
   std::string vocFilename = std::string("sound/") + soundName + ".voc";

   // get .voc file
   Base::SDL_RWopsPtr rwops;
   try
   {
      rwops = m_data->GetResourceManager().GetUnderworldFile(Base::resourceGameUw, vocFilename);
   }
   catch (const Base::FileSystemException&)
   {
      UaTrace("couldn't load sound file %s\n", vocFilename.c_str());
      return;
   }

   VoiceFile vocFile(rwops);

   // start playing
   Mix_Chunk* mc = Mix_LoadWAV_RW(vocFile.GetFileData().get(), true);
   if (mc == NULL)
   {
      UaTrace("couldn't load sound file %s: %s\n", vocFilename.c_str(), Mix_GetError());
      return;
   }

   Mix_PlayChannel(-1, mc, 0);
}

/// Stops a sound that is currently playing back.
void AudioManager::StopSound()
{
   Mix_HaltChannel(-1);
}

/// Plays back a sound effect. The playback stops when the sound effect is
/// finished. Note: since only uw2 has real sound effects stored in .voc files,
/// this method only plays back sound effects when uw2 path is configured
/// properly.
/// \param sfxType sound effect type to play back
/// \todo implement
void AudioManager::PlaySoundEffect(Audio::SoundEffectType sfxType)
{
   UNUSED(sfxType);
}

/// Starts playing back a sound track from the music playlist. Midi files with
/// extensions .mid or .xmi are played back using the appropriate midi driver
/// through MidiPlayer. Other file types are tried to load via SDL_mixer,
/// with the function Mix_LoadMUS(), so all music types that SDL_mixer.dll
/// supports can be played back. The distributed SDL_mixer.dll can only play
/// back Ogg Vorbis and some tracker formats. mp3 files are not supported,
/// since SMPEG is not linked it (it's an ancient format anyway).
///
/// \param musicTrack the position in music playlist of the track to play back
/// \param repeat indicates if track should be repeated when it has stopped
/// \todo use MusicTrackUw1 or a similar mapping to start music instead
void AudioManager::StartMusicTrack(unsigned int musicTrack, bool repeat)
{
   Playlist& playlist = m_data->GetPlaylist();

   if (musicTrack >= playlist.GetCount())
      return;

   std::string trackName = playlist.GetPlaylistTrack(musicTrack);

   UaTrace("audio: playing back %s", trackName.c_str());

   // find extension
   std::string extension;
   std::string::size_type pos = trackName.find_last_of('.');

   if (pos != std::string::npos)
      extension.assign(trackName.substr(pos));

   // make extension lowercase
   Base::String::Lowercase(extension);

   // check for midi tracks
   if (extension.find(".xmi") != std::string::npos ||
      extension.find(".mid") != std::string::npos)
   {
      try
      {
         Base::SDL_RWopsPtr rwops = m_data->GetResourceManager().GetFileWithPlaceholder(trackName);

         // start midi player track
         if (rwops != NULL)
            m_data->GetMidiPlayer().PlayFile(rwops, repeat);
      }
      catch (const Base::Exception&)
      {
         UaTrace("couldn't load midi file %s\n", trackName.c_str());
      }
   }
   else
   {
      Mix_Music*& mm = m_data->GetCurrentTrack();

      if (mm)
      {
         Mix_FreeMusic(mm);
         m_data->GetCurrentMusicTrackData().reset();
      }

      // start music track via SDL_mixer
      m_data->GetCurrentMusicTrackData() = m_data->GetResourceManager().GetFileWithPlaceholder(trackName);
      mm = Mix_LoadMUSType_RW(m_data->GetCurrentMusicTrackData().get(), MUS_OGG, 0);

      if (mm)
         Mix_PlayMusic(mm, repeat ? -1 : 0);
      else
         UaTrace(" (%s)", Mix_GetError());
   }
   UaTrace("\n");
}

/// Fades out the currently playing music track using the specified time.
/// The method currently only fades out tracks playing back using SDL_mixer.
/// Other midi drivers are not supported.
/// \param timeInMs time to fade out music to null volume, in milliseconds
void AudioManager::FadeoutMusic(int timeInMs)
{
   Mix_FadeOutMusic(timeInMs);
}

/// Stops the current music track, either played back by SDL_mixer or the
/// midi player.
void AudioManager::StopMusic()
{
   UaAssert(m_data.get() != NULL);

   Mix_Music*& mm = m_data->GetCurrentTrack();
   if (mm != NULL)
   {
      Mix_FreeMusic(mm);
      mm = NULL;

      m_data->GetCurrentMusicTrackData().reset();
   }

   m_data->GetMidiPlayer().Stop();
}

bool AudioManager::IsMusicFadeoutOrStopped() const
{
   return MIX_FADING_OUT == Mix_FadingMusic() ||
      m_data->GetCurrentMusicTrackData() == NULL;
}

/*
void DumpChunk(const char* soundname, Mix_Chunk* mc)
{
   std::string filename;
   filename = "d:\\uw1_";
   filename += soundname;
   filename += ".wav";

   unsigned int srclen = mc->alen / 2;
   Sint16* samples = reinterpret_cast<Sint16*>(mc->abuf);

   // prepare data bytes array for wave file
   unsigned int fileSize = 12 + 24 + 8 + srclen * sizeof(Sint16);

   unsigned int m_sampleRate = 44100;

   SDL_RWops* rwops = SDL_RWFromFile(filename.c_str(), "wb");

   // header: 12 bytes
   SDL_RWwrite(rwops, "RIFF", 4, 1);
   SDL_WriteLE32(rwops, fileSize - 8);
   SDL_RWwrite(rwops, "WAVE", 4, 1);

   // format chunk: 24 bytes
   SDL_RWwrite(rwops, "fmt ", 4, 1);
   SDL_WriteLE32(rwops, 16);
   SDL_WriteLE16(rwops, 1); // wFormatTag
   SDL_WriteLE16(rwops, 1); // wChannels
   SDL_WriteLE32(rwops, m_sampleRate); // dwSamplesPerSec
   SDL_WriteLE32(rwops, m_sampleRate * sizeof(Sint16)); // dwAvgBytesPerSec = dwSamplesPerSec * wBlockAlign
   SDL_WriteLE16(rwops, sizeof(Sint16)); // wBlockAlign = wChannels * (wBitsPerSample / 8)
   SDL_WriteLE16(rwops, sizeof(Sint16) * 8); // wBitsPerSample

   // data chunk: 8 + numsamples*sizeof(Sint16) bytes
   SDL_RWwrite(rwops, "data", 4, 1);
   unsigned int uiSize = srclen;
   SDL_WriteLE32(rwops, uiSize * sizeof(Sint16)); // size

   for (unsigned int ui = 0; ui < uiSize; ui++)
      SDL_WriteLE16(rwops, samples[ui]);

   SDL_RWclose(rwops);
}
*/
