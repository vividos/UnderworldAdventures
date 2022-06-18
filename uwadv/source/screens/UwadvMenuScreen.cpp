//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2019 Underworld Adventures Team
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
/// \file UwadvMenuScreen.cpp
/// \brief uwadv menu implementation
//
#include "pch.hpp"
#include "UwadvMenuScreen.hpp"
#include "Settings.hpp"
#include "Renderer.hpp"
#include "ResourceManager.hpp"
#include "StartSplashScreen.hpp"
#ifdef HAVE_DEBUG
#include "screens/StartMenuScreen.hpp"
#include "screens/SaveGameScreen.hpp"
// #include "screens/ingame_new.hpp"
#include "screens/OriginalIngameScreen.hpp"
#include "Underworld.hpp"
#endif

void UwadvMenuScreen::Init()
{
   Screen::Init();

   UaTrace("uwadv menu screen started\n");

   // init 2d camera
   m_game.GetRenderer().SetupForUserInterface();
   glEnable(GL_TEXTURE_2D);

   // load texture
   Base::SDL_RWopsPtr rwops = m_gameInstance.GetResourceManager().GetResourceFile("uwadv-loading.png");

   if (rwops != NULL)
   {
      m_texture.Init();
      m_texture.Load(rwops);
      m_texture.Upload(0);
   }

   m_isRendered = false;
}

void UwadvMenuScreen::Draw()
{
   m_texture.Use();

   // set wrap parameter
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

   // set mipmap parameter
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   // calculate height, width and position
   double xres = m_game.GetScreenXRes();
   double yres = m_game.GetScreenYRes();
   double ratio = (xres / yres) * (200.0 / 320.0);

   const unsigned width = 150;
   const unsigned height = static_cast<unsigned>(60 * ratio);
   const unsigned xpos = (320 - width) / 2;
   const unsigned ypos = (200 - height) / 2;

   // draw quad
   glBegin(GL_QUADS);
   glTexCoord2d(0.0, 1.0); glVertex2i(xpos, ypos);
   glTexCoord2d(1.0, 1.0); glVertex2i(xpos + width, ypos);
   glTexCoord2d(1.0, 0.0); glVertex2i(xpos + width, ypos + height);
   glTexCoord2d(0.0, 0.0); glVertex2i(xpos, ypos + height);
   glEnd();

   m_isRendered = true;
}

void UwadvMenuScreen::Tick()
{
   if (!m_isRendered)
      return;

   Base::Settings& settings = m_gameInstance.GetSettings();

   // set game prefix to use
   std::string prefix("uw1");
   settings.SetValue(Base::settingGamePrefix, prefix);

   UaTrace("selected game: %s\n\n",
      settings.GetGameType() != Base::gameUw1 ? "uw2" :
      settings.GetBool(Base::settingUw1IsUwdemo) ? "uw_demo" : "uw1");

   // set generic uw path
   settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw1Path));

   // now that we know the generic uw path, we can init the whole game stuff
   m_gameInstance.InitGame();

   m_game.ReplaceScreen(new StartSplashScreen(m_game), false);
}
