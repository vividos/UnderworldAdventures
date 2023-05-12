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
/// \file LevelEditor.cpp
/// \brief level editor
//
#include "pch.hpp"
#include "LevelEditor.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/RenderWindow.hpp"
#include "renderer/Viewport.hpp"
#include "underworld/Underworld.hpp"
#include "underworld/Level.hpp"

LevelEditor::LevelEditor(IBasicGame& game, const void* windowHandle)
   :m_game(game)
{
   m_renderer = std::make_shared<Renderer>();

   m_renderWindow = std::make_shared<RenderWindow>(windowHandle);

   m_viewport = std::make_shared<Viewport>(*m_renderWindow);
   m_renderer->SetViewport(m_viewport.get());

   RenderOptions& renderOptions = m_renderer->GetRenderOptions();
   renderOptions.m_renderVisibleTilesUsingOctree = false;
   renderOptions.m_useFog = false;
   renderOptions.m_renderBoundingBoxes = true;
   renderOptions.m_renderHiddenObjects = true;

   m_renderer->InitGame(m_game);

   m_renderer->PrepareLevel(m_game.GetUnderworld().GetCurrentLevel());
}

LevelEditor::~LevelEditor()
{
   if (m_renderer != nullptr)
   {
      m_renderer->Done();
      m_renderer.reset();
   }
}

void LevelEditor::SetWindowSize(unsigned int width, unsigned int height)
{
   m_viewport->SetViewport3D(0, 0, width, height);
}

void LevelEditor::Render()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   const Underworld::Underworld& underworld =
      m_game.GetUnderworld();

   Vector3d viewOffset{ 0.0, 0.0, 20.0 };
   m_renderer->SetupFor3D(viewOffset);
   m_renderer->RenderUnderworld(underworld);
}
