//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2019 Underworld Adventures Team
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
/// \file MainGameLoop.cpp
/// \brief main game loop
//
#include "pch.hpp"
#include "MainGameLoop.hpp"

MainGameLoop::MainGameLoop(const char* windowTitle, bool updateFrameCount)
   :m_windowTitle(windowTitle),
   m_updateFrameCount(updateFrameCount),
   m_exitLoop(false),
   m_appActive(true)
{
}

void MainGameLoop::RunGameLoop(unsigned int tickRate)
{
   UaTrace("main loop started\n");

   Uint32 now, then;
   Uint32 fcstart;
   unsigned int ticks = 0, renders = 0;

   fcstart = then = SDL_GetTicks();

   bool resetTickTimer = false;

   m_exitLoop = false;

   while (!m_exitLoop)
   {
      now = SDL_GetTicks();

      while ((now - then) > (1000.0 / tickRate))
      {
         then += Uint32(1000.0 / tickRate);

         OnTick(resetTickTimer);

         ticks++;

         if (resetTickTimer)
            break;
      }

      ProcessEvents();

      if (m_exitLoop)
         break;

      // reset timer when needed
      if (resetTickTimer)
      {
         then = now = SDL_GetTicks();
         resetTickTimer = false;
      }

      if (!m_appActive)
      {
         // as we're not visible, just wait for next event
         SDL_WaitEvent(NULL);
         continue;
      }

      OnRender();

      renders++;

      if ((now - then) > (1000.0 / tickRate))
         then = now - Uint32(1000.0 / tickRate);

      if (m_updateFrameCount)
      {
         now = SDL_GetTicks();

         if (now - fcstart > 2000)
         {
            // set new caption
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "%s: %3.1f ticks/s, %3.1f frames/s",
               m_windowTitle.c_str(),
               ticks * 1000.0 / (now - fcstart), renders * 1000.0 / (now - fcstart));

            UpdateCaption(buffer);

            // restart counting
            ticks = renders = 0;
            fcstart = now;
#ifdef HAVE_DEBUG
            // reset time count when rendering lasted longer than 5 seconds
            // it's likely that we just debugged through some code
            if (now - then > 5000)
               then = now;
#endif
         }
      }

      SDL_Delay(1); // give up time slice
   }

   UaTrace("main loop ended\n\n");
}

void MainGameLoop::QuitLoop()
{
   m_exitLoop = true;
}

void MainGameLoop::ProcessEvents()
{
   SDL_Event event;

   while (SDL_PollEvent(&event))
   {
      OnEvent(event);

      switch (event.type)
      {
      case SDL_QUIT:
         m_exitLoop = true;
         break;

      case SDL_APP_WILLENTERBACKGROUND:
         m_appActive = false;
         break;

      case SDL_APP_DIDENTERFOREGROUND:
         m_appActive = true;
         break;

      case SDL_WINDOWEVENT:
         switch (event.window.event)
         {
         case SDL_WINDOWEVENT_ENTER:
            m_appActive = true;
            break;

         case SDL_WINDOWEVENT_LEAVE:
            m_appActive = false;
            break;

         default:
            break;
         }
         break;

      default:
         break;
      }
   }
}
