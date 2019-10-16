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
/// \file Audio.hpp
/// \brief audio manager definition
//
#pragma once

#include "Base.hpp"
#include <string>
#include <memory>

namespace Base
{
   class Settings;
   class ResourceManager;
}

namespace Detail
{
   class AudioManagerData;
}

/// \brief Audio namespace
/// \details The audio subsystem takes care of all tasks that have to do with
/// playing back the sound track, sound effects (such as steps when walking)
/// and the cutscene speech.
///
/// The audio subsystem can be controlled via the AudioManager class that
/// has functions to do these tasks. Sound track pieces are played back using
/// a playlist loaded at initialisation; they can be faded out, e.g. when a
/// screen ends. Sound effects are referenced by the enum SoundEffectType.
/// Cutscene speech is played back from uw's .voc files; they are internally
/// resampled to 22050 Hz because they are in an unusual format.
///
/// The Audio module depends on the Base module.
namespace Audio
{
   class Playlist;
   class MidiPlayer;

   /// \brief all uw1 music tracks
   /// \details The enum value can be used in a call to AudioManager::StartMusicTrack()
   /// and reflects the position in the music playlist, not the actual filenames
   /// in the sound folder
   enum MusicTrackUw1
   {
      musicUw1_Introduction = 0,
      musicUw1_DarkAbyss,
      musicUw1_Descent,
      musicUw1_Wanderer,
      musicUw1_Battlefield,
      musicUw1_Combat,
      musicUw1_Injured,
      musicUw1_Armed,
      musicUw1_Victory,
      musicUw1_Death,
      musicUw1_Fleeing,
      musicUw1_MapsAndLegends
   };

   /// \brief enumeration of all sound effects from uw2
   /// \todo find out and add add remaining audio sfx types
   enum SoundEffectType
   {
      sfxStepsLeft = 0,   // SP01
      sfxStepsRight,    // SP02
      // SP03 ??
      // SP04 ??
      sfxSwimWater,     // SP05
      sfxPlayerHit,     // SP06
      sfxWeaponHit,     // SP07
      sfxWeaponHit2,    // SP08
      sfxArrow,         // SP09
      sfxWeaponMiss,    // SP10
      sfxMovingDoor,    // SP11
      // SP12 locked door?
      // SP18 some grumbling?
      sfxCrankPortcullis, // SP20
      sfxWalkWater,     // SP24 really water walking?
      // SP26 some more water walking?
      // SP27 ??
      // SP28 ??
      sfxCastSpell,     // SP29
      sfxDrinkBottle,   // SP30 really?
      sfxHaveFood,      // SP31
      sfxHaveFood2,     // SP33 really?
      // SP34
      // SP35
      // SP36
      // SP37 eating apple?
      // SP40
      // SP47
      // SP48
      sfxGuardianLaugh1,// UW00
      sfxGuardianLaugh2,// UW01
   };

   /// \brief Audio manager
   /// \details Lets the user start and stop playing music tracks, sound
   /// effects and *.voc sounds in the background. music tracks can be repeated
   /// indefinitely.
   ///
   /// To use the class, just call init() at program start and then call the play
   /// functions to play music, sfx or *.voc files.
   ///
   /// - PlaySound(soundname) plays the file %uw-path%/sound/{soundname}.voc
   /// - PlaySoundEffect() takes an enum defined below
   /// - StartMusicTrack() takes a playlist index; playlists are loaded from
   ///                 %uadata%/{game-prefix}/audio/music.m3u.
   ///
   /// The playlist is in m3u format, can contain comment lines starting with #
   /// and playlist entries can also contain the placeholders %uw-path%, %uahome%
   /// and %uadata%.
   ///
   /// Be sure to only use the AudioManager object only once, since it uses
   /// the audio-part of SDL and SDL_mixer to play back audio.
   class AudioManager
   {
   public:
      /// ctor
      AudioManager(const Base::Settings& settings, const Base::ResourceManager& resourceManager);
      /// dtor
      ~AudioManager();
      /// deleted copy ctor
      AudioManager(const AudioManager&) = delete;
      /// deleted assignment operator
      AudioManager& operator=(const AudioManager&) = delete;

      /// plays a sound file; stops when finished
      void PlaySound(const std::string& soundName);

      /// stops sound playback
      void StopSound();

      /// plays a special sound effect
      void PlaySoundEffect(SoundEffectType sfxType);

      /// starts music track playback
      void StartMusicTrack(size_t musicTrack, bool repeat);

      /// fades out currently playing music track; fadeout time in milliseconds
      void FadeoutMusic(int timeInMs);

      /// stops music track playback
      void StopMusic();

      /// returns if music is currently fading out or is stopped
      bool IsMusicFadeoutOrStopped() const;

   private:
      /// init SDL audio subsystem and SDL_mixer
      void InitAudioSubsystem();

      /// loads music playlist
      void LoadMusicPlaylist(const Base::Settings& settings, const Base::ResourceManager& resourceManager);

      /// loads timbre library from game files
      void LoadTimbreLibrary();

   private:
      /// audio manager internal data
      std::unique_ptr<Detail::AudioManagerData> m_data;
   };

} // namespace Audio
