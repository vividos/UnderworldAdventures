/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2006 Michael Fink

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
/*! \file audiotest.cpp

   \brief audio classes test

*/

// needed includes
#include "unittest.hpp"
#include "audio.hpp"
#include "midiplayer.hpp"
#include "settings.hpp"
#include <SDL.h>
#include <SDL_mixer.h>
#include "file.hpp"
#include "resourcemanager.hpp"

namespace UnitTest
{

//! Audio test
/*! Note that this test is normally not run; comment in the test
    suite function to run the test.
*/
class TestAudio: public UnitTestCase
{
public:
   // generate test suite
   CPPUNIT_TEST_SUITE(TestAudio)
      //CPPUNIT_TEST(TestAudioManager)
      //CPPUNIT_TEST(TestMidiPlayer)
      //CPPUNIT_TEST(TestPlaySound)
   CPPUNIT_TEST_SUITE_END()

protected:
   void TestAudioManager();
   void TestMidiPlayer();
   void TestPlaySound();
};

// register test suite
CPPUNIT_TEST_SUITE_REGISTRATION(TestAudio)

} // namespace UnitTest

// methods

using namespace UnitTest;

//! auto-init class for SDL audio subsystem and SDL_mixer
class SDLMixerIniter
{
public:
   //! ctor; inits audio
   SDLMixerIniter()
   {
      SDL_Init(SDL_INIT_AUDIO);
      Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096);
   }
   //! ctor; uninits audio
   ~SDLMixerIniter()
   {
      Mix_CloseAudio();
      SDL_QuitSubSystem(SDL_INIT_AUDIO);
   }
};


/*! Tests audio manager music track playback */
void TestAudio::TestAudioManager()
{
   Base::Settings settings;
   Base::LoadSettings(settings);

   settings.SetValue(Base::settingAudioEnabled, true);
   settings.SetValue(Base::settingGamePrefix, std::string("uw1"));
   settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw1Path));

   {
      Audio::AudioManager audioMgr(settings);
      audioMgr.StartMusicTrack(0, true);

      SDL_Delay(20*1000);

      audioMgr.StopMusic();
   }

   settings.SetValue(Base::settingGamePrefix, std::string("uw2"));
   settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw2Path));

   {
      Audio::AudioManager audioMgr(settings);
      audioMgr.StartMusicTrack(1, true);

      SDL_Delay(30*1000);

      audioMgr.StopMusic();
   }

}

/*! Tests midi player .xmi playback */
void TestAudio::TestMidiPlayer()
{
   SDLMixerIniter mixerIniter;

   Base::Settings settings;
   Base::LoadSettings(settings);

   Base::ResourceManager resourceManager(settings);

   // load all uw1 midis
   {
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

      for(unsigned int i=0; i<SDL_TABLESIZE(midisUw1); i++)
      {
         std::string strFilename = std::string("/sound/") + midisUw1[i];

         Base::SDL_RWopsPtr rwops = resourceManager.GetUnderworldFile(Base::resourceGameUw1, strFilename);

         player.PlayFile(rwops, false);
         player.Stop();
      }
   }

   // load all uw2 midis
   {
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

      for(unsigned int i=0; i<SDL_TABLESIZE(midisUw2); i++)
      {
         std::string strFilename = std::string("/sound/") + midisUw2[i];

         Base::SDL_RWopsPtr rwops = resourceManager.GetUnderworldFile(Base::resourceGameUw2, strFilename);

         player.PlayFile(rwops, false);
         player.Stop();
      }
   }
}

/*! Tests playing back sound effects */
void TestAudio::TestPlaySound()
{
   Base::Settings settings;
   Base::LoadSettings(settings);
   settings.SetValue(Base::settingAudioEnabled, true);

   settings.SetValue(Base::settingGamePrefix, std::string("uw1"));
   settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw1Path));

   {
      Audio::AudioManager audioMgr(settings);
      audioMgr.PlaySound("26");
      SDL_Delay(2*1000);
      audioMgr.PlaySound("27");
      SDL_Delay(7*1000);
      audioMgr.PlaySound("28");
      SDL_Delay(4*1000);
   }

   settings.SetValue(Base::settingGamePrefix, std::string("uw2"));
   settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw2Path));
   {
      Audio::AudioManager audioMgr(settings);
      audioMgr.PlaySound("BSP00");

      SDL_Delay(16*1000);
   }
}
