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
/// \file RenderWindow.hpp
/// \brief Render window
//
#pragma once

struct SDL_Window;
struct SDL_Renderer;

/// window to render OpenGL content
class RenderWindow
{
public:
   /// creates new render window with given size and fullscreen flag
   RenderWindow(int width, int height, const char* title, bool fullscreen);

   /// sets fullscreen mode (or goes back to windowed mode)
   void SetFullscreen(bool fullscreen);

   /// returns current window size
   void GetWindowSize(int& width, int& height) const;

   /// sets window icon using a 32x32 icon resource
   void SetWindowIcon(int iconId);

   /// sets new window title
   void SetWindowTitle(const char* title);

   /// swaps screen buffers
   void SwapBuffers();

private:
   /// window width
   int m_width;

   /// window height
   int m_height;

   /// SDL window
   SDL_Window* m_window;

   /// SDL renderer
   SDL_Renderer* m_sdlRenderer;

   /// GL context
   SDL_GLContext m_context;
};
