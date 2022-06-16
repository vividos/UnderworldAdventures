//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2022 Underworld Adventures Team
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
/// \file LevelEditor.hpp
/// \brief level editor
//
#pragma once

class IBasicGame;
class Renderer;
class RenderWindow;
class Viewport;

/// \brief level editor
class LevelEditor
{
public:
   /// ctor
   LevelEditor(IBasicGame& game, const void* windowHandle);
   /// dtor
   ~LevelEditor();

   /// changes window size
   void SetWindowSize(unsigned int width, unsigned int height);

   /// renders the underworld level being edited
   void Render();

private:
   /// game instance
   IBasicGame& m_game;

   /// renderer instance
   std::shared_ptr<Renderer> m_renderer;

   /// render window
   std::shared_ptr<RenderWindow> m_renderWindow;

   /// render viewport
   std::shared_ptr<Viewport> m_viewport;
};
