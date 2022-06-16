//
// Underworld Adventures Debugger - a debugger tool for Underworld Adventures
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
/// \file Tilemap3DEditorView.hpp
/// \brief tilemap 3D editor view
//
#include "pch.hpp"
#include "Tilemap3DEditorView.hpp"
#include "DebugClient.hpp"
#include "game/LevelEditor.hpp"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

LRESULT Tilemap3DEditorView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   m_levelEditor.reset();

   bHandled = false;
   return 0;
}

void Tilemap3DEditorView::OnInit()
{
}

void Tilemap3DEditorView::InitDebugWindow(IMainFrame* mainFrame)
{
   baseClass::InitDebugWindow(mainFrame);

   CClientDC dc{ m_hWnd };
   dc.wglMakeCurrent(m_hRC);

   DebugClient& debugClient = m_mainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   m_levelEditor = debugClient.CreateLevelEditor(m_hWnd);

   debugClient.Lock(false);

   dc.wglMakeCurrent(nullptr);
}

void Tilemap3DEditorView::OnRender()
{
   if (m_levelEditor == nullptr)
      return;

   DebugClient& debugClient = m_mainFrame->GetDebugClientInterface();
   debugClient.Lock(true);

   m_levelEditor->Render();

   debugClient.Lock(false);
}

void Tilemap3DEditorView::OnResize(int cx, int cy)
{
   if (m_levelEditor == nullptr)
      return;

   CRect rc;
   GetClientRect(&rc);

   m_levelEditor->SetWindowSize(rc.Width(), rc.Height());
}
