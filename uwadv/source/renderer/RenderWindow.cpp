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
/// \file RenderWindow.cpp
/// \brief Render window
//
#include "pch.hpp"
#include "RenderWindow.hpp"
#include <SDL_opengl.h>

#ifdef WIN32
#include <SDL_syswm.h>
#endif

RenderWindow::RenderWindow(int width, int height, const char* title, bool fullscreen)
   :m_width(width),
   m_height(height)
{
   // setup video mode
   int flags = fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;

   m_window = SDL_CreateWindow(
      title,
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      m_width, m_height,
      flags | SDL_WINDOW_OPENGL);

   if (m_window == NULL)
   {
      std::string text("video mode set failed: ");
      text.append(SDL_GetError());
      throw Base::Exception(text.c_str());
   }

#ifdef HAVE_DEBUG
   int rendererFlags = SDL_RENDERER_PRESENTVSYNC;
#else
   int rendererFlags = SDL_RENDERER_PRESENTVSYNC;
#endif
   m_sdlRenderer = SDL_CreateRenderer(m_window, -1, rendererFlags);

   m_context = SDL_GL_CreateContext(m_window);

   SDL_GL_SetSwapInterval(1); // 1 means VSYNC

   SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
   SDL_RenderSetLogicalSize(m_sdlRenderer, m_width, m_height);

   // setup OpenGL viewport
   glViewport(0, 0, m_width, m_height);
}

void RenderWindow::SetFullscreen(bool fullscreen)
{
   SDL_SetWindowFullscreen(m_window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);

   int width, height;
   SDL_GetWindowSize(m_window, &width, &height);
   m_width = width;
   m_height = height;

   glViewport(0, 0, m_width, m_height);
}

void RenderWindow::GetWindowSize(int& width, int& height) const
{
   SDL_GetWindowSize(m_window, &width, &height);
}

void RenderWindow::SetWindowTitle(const char* title)
{
   SDL_SetWindowTitle(m_window, title);
}

void RenderWindow::SetWindowIcon(int iconId)
{
   iconId;

#ifdef WIN32
   HINSTANCE inst = (HINSTANCE)GetModuleHandle(NULL);

   HICON icon = ::LoadIcon(inst, MAKEINTRESOURCE(iconId));
   HICON icon_small = (HICON)::LoadImage(inst,
      MAKEINTRESOURCE(iconId), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

   SDL_SysWMinfo wminfo;
   SDL_VERSION(&wminfo.version);

   if (SDL_GetWindowWMInfo(m_window, &wminfo) == 1)
   {
      HWND hwnd = wminfo.info.win.window;
      ::SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)icon);
      ::SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon_small);
   }
#endif
}

/// Swaps OpenGL screen buffers
void RenderWindow::SwapBuffers()
{
   SDL_GL_SwapWindow(m_window);
}
