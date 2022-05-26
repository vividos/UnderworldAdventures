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
#pragma once

#include "resource.h"
#include <AtlOpengl.h>

/// 3D editor view for tilemap
class Tilemap3DEditorView :
   public ChildWindowBase<IDR_TILEMAP_EDITOR_VIEW>,
   private COpenGL<Tilemap3DEditorView>
{
   typedef Tilemap3DEditorView thisClass;
   typedef ChildWindowBase<IDR_TILEMAP_EDITOR_VIEW> baseClass;

private:
   BEGIN_MSG_MAP(thisClass)
      ATLASSERT_ADDED_REFLECT_NOTIFICATIONS()
      CHAIN_MSG_MAP(COpenGL<Tilemap3DEditorView>)
      CHAIN_MSG_MAP(baseClass)
      DEFAULT_REFLECTION_HANDLER()
   END_MSG_MAP()

private:
   friend COpenGL<Tilemap3DEditorView>;

   /// initializes render window
   void OnInit();

   /// renders a new frame
   void OnRender();

   /// called when the reder window is resized
   void OnResize(int cx, int cy);
};
