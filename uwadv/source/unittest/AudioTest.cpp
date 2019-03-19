//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2006,2019 Michael Fink
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
/// \file AudioTest.cpp
/// \brief audio classes test
//
#include "pch.hpp"
#include "Audio.hpp"
#include "MidiPlayer.hpp"
#include "Settings.hpp"
#include <SDL.h>
#include <SDL_mixer.h>
#include "File.hpp"
#include "ResourceManager.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
   /// \brief Audio tests
   /// Note that this test is normally not run; comment in the test
   /// suite function to run the test.
   TEST_CLASS(AudioTest)
   {
      /// auto-init class for SDL audio subsystem and SDL_mixer
      class SDLMixerIniter
      {
      public:
         /// ctor; inits audio
         SDLMixerIniter()
         {
            SDL_Init(SDL_INIT_AUDIO);
            Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096);
         }
         /// ctor; uninits audio
         ~SDLMixerIniter()
         {
            Mix_CloseAudio();
            SDL_QuitSubSystem(SDL_INIT_AUDIO);
         }
      };

      /// Tests audio manager music track playback, uw1
      TEST_METHOD(TestAudioManagerUw1)
      {
         Base::Settings settings = GetTestSettings();

         settings.SetValue(Base::settingAudioEnabled, true);
         settings.SetValue(Base::settingGamePrefix, std::string("uw1"));
         settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw1Path));

         Base::ResourceManager resourceManager{ settings };

         Audio::AudioManager audioMgr(settings, resourceManager);
         audioMgr.StartMusicTrack(0, true);

         SDL_Delay(20 * 1000);

         audioMgr.StopMusic();
      }

      /// Tests audio manager music track playback, uw2
      TEST_METHOD(TestAudioManagerUw2)
      {
         Base::Settings settings = GetTestSettings();

         settings.SetValue(Base::settingGamePrefix, std::string("uw2"));
         settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw2Path));

         Base::ResourceManager resourceManager{ settings };

         Audio::AudioManager audioMgr(settings, resourceManager);
         audioMgr.StartMusicTrack(1, true);

         SDL_Delay(30 * 1000);

         audioMgr.StopMusic();
      }

      /// Tests midi player .xmi playback, uw1
      TEST_METHOD(TestMidiPlayerUw1)
      {
         SDLMixerIniter mixerIniter;

         Base::Settings& settings = GetTestSettings();
         Base::ResourceManager resourceManager{ settings };

         const char* midisUw1[] =
         {
            "UW01.XMI",
            "UW02.XMI",
            "UW03.XMI",
            "UW04.XMI",
            "UW05.XMI",
            "UW06.XMI",
            "UW07.XMI",
            "UW10.XMI",
            "UW11.XMI",
            "UW12.XMI",
            "UW13.XMI",
            "UW15.XMI"
         };

         Audio::MidiPlayer player(settings);

         for (unsigned int i = 0; i < SDL_TABLESIZE(midisUw1); i++)
         {
            std::string filename = std::string("/sound/") + midisUw1[i];

            Base::SDL_RWopsPtr rwops = resourceManager.GetUnderworldFile(Base::resourceGameUw1, filename);

            player.PlayFile(rwops, false);
            player.Stop();
         }
      }

      /// Tests midi player .xmi playback, uw2
      TEST_METHOD(TestMidiPlayerUw2)
      {
         SDLMixerIniter mixerIniter;

         Base::Settings& settings = GetTestSettings();
         Base::ResourceManager resourceManager{ settings };

         const char* midisUw2[] =
         {
            "UWA01.XMI",
            "UWA02.XMI",
            "UWA03.XMI",
            "UWA04.XMI",
            "UWA05.XMI",
            "UWA06.XMI",
            "UWA07.XMI",
            "UWA10.XMI",
            "UWA11.XMI",
            "UWA12.XMI",
            "UWA13.XMI",
            "UWA14.XMI",
            "UWA15.XMI",
            "UWA16.XMI",
            "UWA17.XMI",
            "UWA30.XMI",
            "UWA31.XMI"
         };

         Audio::MidiPlayer player(settings);

         for (unsigned int i = 0; i < SDL_TABLESIZE(midisUw2); i++)
         {
            std::string filename = std::string("/sound/") + midisUw2[i];

            Base::SDL_RWopsPtr rwops = resourceManager.GetUnderworldFile(Base::resourceGameUw2, filename);

            player.PlayFile(rwops, false);
            player.Stop();
         }
      }

      /// Tests playing back sound effects
      TEST_METHOD(TestPlaySoundUw1)
      {
         Base::Settings settings = GetTestSettings();

         settings.SetValue(Base::settingAudioEnabled, true);

         settings.SetValue(Base::settingGamePrefix, std::string("uw1"));
         settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw1Path));

         Base::ResourceManager resourceManager{ settings };

         Audio::AudioManager audioMgr(settings, resourceManager);
         audioMgr.PlaySound("26");
         SDL_Delay(2 * 1000);
         audioMgr.PlaySound("27");
         SDL_Delay(7 * 1000);
         audioMgr.PlaySound("28");
         SDL_Delay(4 * 1000);
      }

      /// Tests playing back sound effects, uw2
      TEST_METHOD(TestPlaySoundUw2)
      {
         Base::Settings settings = GetTestSettings();

         settings.SetValue(Base::settingAudioEnabled, true);

         settings.SetValue(Base::settingGamePrefix, std::string("uw2"));
         settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw2Path));

         Base::ResourceManager resourceManager{ settings };

         Audio::AudioManager audioMgr(settings, resourceManager);
         audioMgr.PlaySound("BSP00");

         SDL_Delay(16 * 1000);
      }
   };
} // namespace UnitTest
