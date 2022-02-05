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
/// \file CutsceneViewScreen.hpp
/// \brief cutscene view screen
//
#pragma once

#include "Screen.hpp"
#include "Cutscene.hpp"
#include "Font.hpp"
#include "script/LuaScripting.hpp"

struct lua_State;

/// cutscene view screen
class CutsceneViewScreen : public Screen
{
public:
   /// ctor
   CutsceneViewScreen(IGame& game, unsigned int cutsceneNumber = 0)
      :Screen(game),
      m_cutsceneNumber(cutsceneNumber)
   {
   }

   // virtual functions from Screen
   virtual void Init() override;
   virtual void Destroy() override;
   virtual void Draw() override;
   virtual bool ProcessEvent(SDL_Event& event) override;
   virtual void Tick() override;

protected:
   /// creates text image from string
   void CreateTextImage(const std::string& str);

   /// performs cutscene action; called by cuts_do_action
   void DoAction();

   // registered lua C functions

   /// performs an action given by the script
   static int cuts_do_action(lua_State* L);

protected:
   // constants

   /// frames per second for cutscene animation
   static const double s_animationFramesPerSecond;

   /// time needed to fade in/out text
   static const double s_fadeTime;

   /// name of lua userdata variable containing the "this" pointer
   static const char* s_luaThisPointerName;

   /// lua scripting interface
   LuaScripting m_lua;

   /// cutscene number to show
   unsigned int m_cutsceneNumber;

   /// number of ticks
   unsigned int m_tickCount;

   /// indicates if cutscene script ended
   bool m_isEnded;

   // subtitle text stuff

   /// indicates if the subtitle text is currently shown
   bool m_isShowingText;

   /// string block to use
   unsigned int m_stringBlock;

   /// text color
   Uint8 m_textColor;

   /// subtitle font
   Font m_bigFont;

   /// current text as image
   ImageQuad m_textImage;

   /// cutscene image
   ImageQuad m_image;

   /// text fadein/fadeout state
   unsigned int m_textFadeState;

   /// count for text fadein/fadeout
   unsigned int m_textFadeCount;

   // cutscene animation stuff

   /// indicates if animation should be shown
   bool m_showAnimation;

   /// indicates if animation moves
   bool m_loopAnimation;

   /// cutscene animation sequence
   Cutscene m_cutsceneAnimation;

   /// current animation frame
   unsigned int m_currentFrame;

   /// frame to stop at; if negative, don't stop
   int m_stopFrame;

   /// animation tick count
   double m_animationTickCount;

   /// anim fadein/fadeout state
   unsigned int m_animationFadeState;

   /// count for anim fadein/fadeout
   unsigned int m_animationFadeCount;

   /// can playback sound
   bool m_canPlayBackSound;

   /// can show text
   bool m_canShowText;
};
