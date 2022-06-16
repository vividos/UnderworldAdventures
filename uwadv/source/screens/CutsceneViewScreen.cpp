//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2022 Underworld Adventures Team
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
/// \file CutsceneViewScreen.cpp
/// \brief cutscene view implementation
//
#include "pch.hpp"
#include "CutsceneViewScreen.hpp"
#include "Audio.hpp"
#include "Renderer.hpp"
#include "Underworld.hpp"
#include "GameStrings.hpp"

extern "C"
{
#include "lua.h"
#include "lualib.h"
}

/// frames per second for cutscene animation
const double CutsceneViewScreen::s_animationFramesPerSecond = 5.0;

/// time needed to fade in/out text
const double CutsceneViewScreen::s_fadeTime = 0.5;

/// name of lua userdata variable containing the "this" pointer
const char* CutsceneViewScreen::s_luaThisPointerName = "self";

void CutsceneViewScreen::Init()
{
   Screen::Init();

   UaTrace("cutscene view screen started\n"
      "showing cutscene %u\n", m_cutsceneNumber);

   m_game.GetRenderer().SetupForUserInterface();

   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   // determine cutscene type
   Base::Settings& settings = m_game.GetSettings();
   {
      std::string cutsntype(settings.GetString(Base::settingCutsceneNarration));

      m_canPlayBackSound = m_canShowText = false;

      if (cutsntype.compare("both") == 0)
         m_canPlayBackSound = m_canShowText = true;

      if (cutsntype.compare("sound") == 0)
         m_canPlayBackSound = true;

      if (cutsntype.compare("subtitles") == 0)
         m_canShowText = true;

      // enable showing text when audio wasn't enabled
      if (!settings.GetBool(Base::settingAudioEnabled))
         m_canShowText = true;
   }

   m_isEnded = false;
   m_tickCount = 0;

   m_showAnimation = false;
   m_loopAnimation = false;
   m_currentFrame = 0;
   m_animationTickCount = 0.0;
   m_animationFadeState = 0;
   m_animationFadeCount = 0;

   m_isShowingText = false;
   m_stringBlock = 0x0c00;
   m_textColor = 11;
   m_textFadeState = 0;
   m_textFadeCount = 0;

   // stop audio track
   m_game.GetAudioManager().StopMusic();

   // init lua scripting

   // get a new lua state
   m_lua.Init(&m_game);

   // register C functions
   lua_register(m_lua.GetLuaState(), "cuts_do_action", cuts_do_action);

   // load lua cutscene script
   if (!m_lua.LoadScript("uw1/scripts/cutscene"))
      m_isEnded = true;

   lua_State* L = m_lua.GetLuaState();

   // set "this" pointer
   lua_pushlightuserdata(L, this);
   lua_setglobal(L, s_luaThisPointerName);

   // call "cuts_init(cutscene)"
   lua_getglobal(L, "cuts_init");
   lua_pushnumber(L, static_cast<double>(m_cutsceneNumber));
   int ret = lua_pcall(L, 1, 0, 0);
   if (ret != 0)
   {
      const char* errorText = lua_tostring(L, -1);
      UaTrace("Lua function call cuts_init(%u) ended with error code %u: %s\n",
         m_cutsceneNumber, ret, errorText);
      m_isEnded = true;
   }

   // init subtitle text
   m_bigFont.Load(m_game.GetResourceManager(), fontBig);
}

void CutsceneViewScreen::Destroy()
{
   UaTrace("cutscene view screen ended\n\n");

   // stop audio track
   m_game.GetAudioManager().StopSound();

   m_textImage.Destroy();
   m_image.Destroy();

   m_lua.Done();
}

void CutsceneViewScreen::Draw()
{
   glClear(GL_COLOR_BUFFER_BIT);

   if (m_showAnimation)
   {
      // render animation frame

      // prepare image texture
      m_cutsceneAnimation.GetFrame(m_image.GetImage(), m_currentFrame);
      m_image.Update();

      // set text color
      Uint8 light = 255;
      switch (m_animationFadeState)
      {
      case 0: // show
         light = 255;
         break;

      case 1: // fade in
         light = static_cast<Uint8>(255 * double(m_animationFadeCount) /
            (s_fadeTime*GetTickRate()));
         break;

      case 2: // fade out
         light = 255 - static_cast<Uint8>(255 * double(m_animationFadeCount) /
            (s_fadeTime*GetTickRate()));
         break;
      }
      glColor3ub(light, light, light);

      m_image.Draw();
   }

   if (m_isShowingText)
   {
      // render subtitle text

      // enable blending, in case text overlaps animation
      glEnable(GL_BLEND);

      // set text color
      Uint8 light = 255;
      switch (m_textFadeState)
      {
      case 0: // show
         light = 255;
         break;

      case 1: // fade in
         light = static_cast<Uint8>(255 * double(m_textFadeCount) /
            (s_fadeTime*GetTickRate()));
         break;

      case 2: // fade out
         light = 255 - static_cast<Uint8>(255 * double(m_textFadeCount) /
            (s_fadeTime*GetTickRate()));
         break;
      }
      glColor3ub(light, light, light);

      m_textImage.Draw();

      glDisable(GL_BLEND);
   }
}

bool CutsceneViewScreen::ProcessEvent(SDL_Event& event)
{
   bool ret = false;

   switch (event.type)
   {
   case SDL_KEYDOWN:
      // handle key presses
      switch (event.key.keysym.sym)
      {
      case SDLK_SPACE:
      case SDLK_RETURN:
      case SDLK_ESCAPE:
         UaTrace("cutscene ended by space/return/escape\n");
         m_isEnded = true;

         // initiate text and anim fadeout
         m_textFadeState = 2;
         m_textFadeCount = 0;
         m_animationFadeState = 2;
         m_animationFadeCount = 0;
         ret = true;
         break;

      default:
         break;
      }
   default:
      break;
   }

   return ret;
}

void CutsceneViewScreen::Tick()
{
   if (m_isEnded && m_animationFadeCount >= s_fadeTime * GetTickRate())
   {
      // we're finished
      m_game.RemoveScreen();
      return;
   }

   // check anim looping
   if (m_showAnimation && m_loopAnimation && !(m_animationFadeState == 1 || m_animationFadeState == 2))
   {
      // count up m_animationTickCount
      m_animationTickCount += 1.0 / GetTickRate();

      if (m_animationTickCount >= 1.0 / s_animationFramesPerSecond)
      {
         // calculate remaining time
         m_animationTickCount -= 1.0 / s_animationFramesPerSecond;

         // count up frames
         m_currentFrame++;
         if (m_currentFrame >= m_cutsceneAnimation.GetMaxFrames())
            m_currentFrame = 0;

         // check if we should stop at that frame
         if (m_stopFrame >= 0 && m_currentFrame == static_cast<unsigned int>(m_stopFrame))
         {
            // disable animation; works like "cuts_anim_stop"
            m_loopAnimation = false;
         }
      }
   }

   // check anim fading
   if ((m_showAnimation || m_isEnded) && (m_animationFadeState == 1 || m_animationFadeState == 2))
   {
      ++m_animationFadeCount;

      // end of fade reached?
      if (m_animationFadeCount >= s_fadeTime * GetTickRate())
      {
         if (m_animationFadeState == 1)
            m_animationFadeState = 0;
         else
            m_showAnimation = false;
      }
   }

   // check text fading
   if (m_isShowingText && (m_textFadeState == 1 || m_textFadeState == 2))
   {
      ++m_textFadeCount;

      // end of fade reached?
      if (m_textFadeCount >= s_fadeTime * GetTickRate())
      {
         if (m_textFadeState == 1)
            m_textFadeState = 0;
         else
            m_isShowingText = false;
      }
   }


   // calculate current ticktime
   double ticktime = double(m_tickCount) / GetTickRate();

   // call lua "cuts_tick(ticktime)" function
   if (!m_isEnded)
   {
      lua_getglobal(m_lua.GetLuaState(), "cuts_tick");
      lua_pushnumber(m_lua.GetLuaState(), ticktime);
      int ret = lua_pcall(m_lua.GetLuaState(), 1, 0, 0);
      if (ret != 0)
      {
         const char* errorText = lua_tostring(m_lua.GetLuaState(), -1);
         UaTrace("Lua function call cuts_tick(%3.2f) ended with error code %u: %s\n",
            ticktime, ret, errorText);
         m_isEnded = true;
         return;
      }
   }

   ++m_tickCount;
}

void CutsceneViewScreen::CreateTextImage(const std::string& str)
{
   unsigned int maxlen = 310;

   m_textImage.GetImage().Clear(0);

   if (m_bigFont.CalcLength(str) < maxlen)
   {
      // easy task: all text fits into one line
      m_bigFont.CreateString(m_textImage.GetImage(), str, m_textColor);
   }
   else
   {
      std::vector<IndexedImage> lines;
      std::string text{ str }, part;

      // collect all text string lines
      do
      {
         part.assign(text);

         std::string::size_type pos;

         // cut down string on ' ' boundaries, until it fits into an image
         while (m_bigFont.CalcLength(part) > maxlen)
         {
            pos = part.find_last_of(' ');
            if (pos != std::string::npos)
            {
               part.erase(pos);
            }
            else
               break; // string too long, without a space in between
         }

         // create line image
         {
            IndexedImage img_temp;
            m_bigFont.CreateString(img_temp, part, m_textColor);
            lines.push_back(img_temp);
         }

         // cut down string
         text.erase(0, part.size() + 1);

      } while (!text.empty());

      // re-assemble the text strings in a new image
      if (lines.empty())
         return; // there definitely went something wrong

      // create new image
      {
         unsigned int lineheight = lines[0].GetYRes();
         unsigned int max = static_cast<unsigned int>(lines.size());

         IndexedImage& img = m_textImage.GetImage();
         img.Create(320, lineheight*max);

         // paste all lines into the new image
         for (unsigned int i = 0; i < max; i++)
         {
            const IndexedImage &img_temp = lines[i];

            unsigned int destx = (320 - img_temp.GetXRes()) / 2;
            img.PasteRect(img_temp, 0, 0, img_temp.GetXRes(), img_temp.GetYRes(),
               destx, lineheight*i);
         }
      }
   }

   // calculate screen position
   unsigned int startx = (320 - m_textImage.GetImage().GetXRes()) / 2;
   unsigned int starty = 200 - 5 - m_textImage.GetImage().GetYRes();

   // init after new creation
   m_textImage.Init(m_game, startx, starty);

   // upload texture
   m_textImage.Update();
}

/// stack indices/values:
/// -2: actioncode
/// -1: actionvalue
void CutsceneViewScreen::DoAction()
{
   lua_State* L = m_lua.GetLuaState();
   unsigned int action = static_cast<unsigned int>(lua_tonumber(L, -2));

   // note: the case values must be in sync with the ones in the script
   switch (action)
   {
   case 0: // cuts_finished
      m_isEnded = true;
      m_animationFadeCount = static_cast<unsigned int>(
         s_fadeTime*GetTickRate()) + 1;
      break;

   case 1: // cuts_set_string_block
      m_stringBlock = static_cast<unsigned int>(lua_tonumber(L, -1));
      break;

   case 2: // cuts_sound_play
      if (m_canPlayBackSound)
      {
         const char *str = lua_tostring(L, -1);
         m_game.GetAudioManager().PlaySound(str);
      }
      break;

   case 3: // cuts_text_set_color
      m_textColor = static_cast<Uint8>(lua_tonumber(L, -1));
      break;

   case 4: // cuts_text_fadein
      if (m_canShowText)
      {
         unsigned int strnum = static_cast<unsigned int>(lua_tonumber(L, -1));
         CreateTextImage(m_game.GetGameStrings().GetString(m_stringBlock, strnum));
         m_textFadeState = 1; // fade in
         m_textFadeCount = 0;
         m_isShowingText = true;
      }
      break;

   case 5: // cuts_text_fadeout
      if (m_canShowText)
      {
         m_textFadeState = 2; // fade out
         m_textFadeCount = 0;
         m_isShowingText = true;
      }
      break;

   case 6: // cuts_text_show
      if (m_canShowText)
      {
         unsigned int strnum = static_cast<unsigned int>(lua_tonumber(L, -1));
         CreateTextImage(m_game.GetGameStrings().GetString(m_stringBlock, strnum));
         m_isShowingText = true;
         m_textFadeState = 0;
      }
      break;

   case 7: // cuts_text_hide
      m_isShowingText = false;
      break;

   case 8: // cuts_anim_fadein
      m_animationFadeState = 1;
      m_animationFadeCount = 0;
      // falls through to "cuts_anim_show"

   case 9: // cuts_anim_show
   {
      std::string animname("cuts/");
      animname.append(lua_tostring(L, -1));

      // load animation
      m_cutsceneAnimation.Load(m_game.GetResourceManager(), animname.c_str(), m_image.GetImage());
      m_image.Init(m_game, 0, 0);
      m_showAnimation = true;
      m_loopAnimation = true;
      m_currentFrame = 0;
      m_animationTickCount = 0.0;
      m_stopFrame = -1;
   }
   break;

   case 10: // cuts_anim_set_stopframe
      // get "stop frame" parameter
      m_stopFrame = static_cast<int>(lua_tonumber(L, -1));
      break;

   case 11: // cuts_anim_fadeout
      m_animationFadeState = 2;
      m_animationFadeCount = 0;
      break;

   case 12: // cuts_anim_stop
      m_loopAnimation = false;
      m_currentFrame = static_cast<unsigned int>(lua_tonumber(L, -1));
      break;

   case 13: // cuts_anim_hide
      m_showAnimation = false;
      break;

   case 14: // cuts_anim_continue
      m_loopAnimation = true;
      break;
   }
}

int CutsceneViewScreen::cuts_do_action(lua_State* L)
{
   // check for "self" parameter being userdata

   lua_getglobal(L, s_luaThisPointerName);
   if (lua_islightuserdata(L, -1))
   {
      // get pointer to screen
      CutsceneViewScreen *self =
         reinterpret_cast<CutsceneViewScreen*>(lua_touserdata(L, -1));

      if (self->m_lua.GetLuaState() != L)
         throw Base::Exception("wrong 'self' parameter in Lua script");

      lua_pop(L, 1);

      // perform action
      self->DoAction();
   }
   else
      throw Base::Exception("'self' parameter wasn't set by Lua script");

   return 0;
}
