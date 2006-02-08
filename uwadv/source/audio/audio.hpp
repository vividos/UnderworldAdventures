/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2002,2003,2004,2005,2006 Michael Fink

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
/*! \defgroup audio Audio components

   The audio subsystem takes care of all tasks that have to do with playing
   back the sound track, sound effects (such as steps when walking) and the
   cutscene speech.

   The audio subsystem can be controlled via the AudioManager class that
   has functions to do these tasks. Sound track pieces are played back using
   a playlist loaded at initialisation; they can be faded out, e.g. when a
   screen ends. Sound effects are referenced by the enum ua_audio_sfx_type.
   Cutscene speech is played back from uw's .voc files; they are internally
   resampled to 22050 Hz because they are in an unusual format.

   The Audio module depends on the Base module.
*/

// include guard
#ifndef uwadv_audio_audio_hpp_
#define uwadv_audio_audio_hpp_

// needed includes
#include "base.hpp"
#include <string>
#include <memory> // for std::auto_ptr

namespace Base
{
   class Settings;
}

namespace Detail
{
   class AudioManagerData;
}

//! Audio namespace \ingroup audio
namespace Audio
{
class Playlist;
class MidiPlayer;


// enums

//! all uw1 music tracks
/*! The enum value can be used in a call to AudioManager::start_music()
    and reflects the position in the music playlist, not the actual filenames
    in the sound folder */
enum EMusicTrackUw1
{
   musicUw1_Introduction=0,
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


//! enumeration of all sound effects
/*! \todo find out and add add remaining audio sfx types */
enum ESoundEffectType
{
   sfxStepsLeft=0,   // SP01
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


// classes

//! Audio manager
/*! Lets the user start and stop playing music tracks, sound
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

    Be sure to only use the AudioManager object only once, since it uses
    the audio-part of SDL and SDL_mixer to play back audio.
*/
class AudioManager
{
public:
   //! ctor
   AudioManager(const Base::Settings& settings);
   //! dtor
   ~AudioManager();

   //! plays a sound file; stops when finished
   void PlaySound(const std::string& strSoundName);

   //! stops sound playback
   void StopSound();

   //! plays a special sound effect
   void PlaySoundEffect(ESoundEffectType sfxType);

   //! starts music track playback
   void StartMusicTrack(unsigned int uiMusic, bool bRepeat);

   //! fades out currently playing music track; fadeout time in milliseconds
   void FadeoutMusic(int iTimeMs);

   //! stops music track playback
   void StopMusic();

private:
   //! audio manager internal data
   std::auto_ptr<Detail::AudioManagerData> m_apData;
};

} // namespace Audio

#endif
