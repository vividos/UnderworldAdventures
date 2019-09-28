//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
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
/// \file Renderer.cpp
/// \brief underworld renderer
//
#include "pch.hpp"
#include "Renderer.hpp"
#include "RendererImpl.hpp"
#include "Viewport.hpp"
#include "Critter.hpp"
#include "Model3D.hpp"
#include "Underworld.hpp"
#include "GameInterface.hpp"
#include "Constants.hpp"
#include <SDL_opengl.h>

Renderer::Renderer()
   :m_viewOffset(0.0, 0.0, 0.0),
   m_rendererImpl(NULL)
{
}

Renderer::~Renderer()
{
   Done();
}

/// Initializes the renderer, the texture manager, critter frames manager and
/// OpenGL flags common to 2d and 3d rendering.
/// \param game game interface
void Renderer::InitGame(IGame& game)
{
   m_rendererImpl = new RendererImpl;
   if (m_rendererImpl == NULL)
      throw Base::Exception("couldn't create RendererImpl class");

   GetTextureManager().Init(game);
   GetModel3DManager().Init(game);
   GetCritterFramesManager().Init(game.GetSettings(), game.GetResourceManager(), game.GetImageManager());

   // culling: only render front face, counter clockwise
   glCullFace(GL_BACK);
   glFrontFace(GL_CCW);
   glEnable(GL_CULL_FACE);

   // enable z-buffer
   glEnable(GL_DEPTH_TEST);

   // enable texturing
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, 0);

   // smooth shading
   glShadeModel(GL_SMOOTH);

   // give some rendering hints
   glHint(GL_FOG_HINT, GL_DONT_CARE);
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
   glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);
}

void Renderer::PrintOpenGLDiagnostics()
{
   GLint redbits, greenbits, bluebits, alphabits, depthbits;
   glGetIntegerv(GL_RED_BITS, &redbits);
   glGetIntegerv(GL_GREEN_BITS, &greenbits);
   glGetIntegerv(GL_BLUE_BITS, &bluebits);
   glGetIntegerv(GL_ALPHA_BITS, &alphabits);
   glGetIntegerv(GL_DEPTH_BITS, &depthbits);

   UaTrace("OpenGL stats:\n bit depths: red/green/blue/alpha = %u/%u/%u/%u, depth=%u\n",
      redbits, greenbits, bluebits, alphabits, depthbits);

   GLint maxtexsize, maxlights, maxnamestack, maxmodelstack, maxprojstack;
   glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxtexsize);
   glGetIntegerv(GL_MAX_LIGHTS, &maxlights);
   glGetIntegerv(GL_MAX_NAME_STACK_DEPTH, &maxnamestack);
   glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &maxmodelstack);
   glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, &maxprojstack);

   UaTrace(" max. texture size = %u x %u, max. lights = %u\n",
      maxtexsize, maxtexsize, maxlights);

   UaTrace(" stack depths: name stack = %u, modelview stack = %u, proj. stack = %u\n",
      maxnamestack, maxmodelstack, maxprojstack);

   const GLubyte* vendor, *rendererName, *version;
   vendor = glGetString(GL_VENDOR);
   rendererName = glGetString(GL_RENDERER);
   version = glGetString(GL_VERSION);

   UaTrace(" vendor: %s\n renderer: %s\n version: %s\n",
      vendor, rendererName, version);

   GLboolean stereo;
   glGetBooleanv(GL_STEREO, &stereo);

   UaTrace(" supports stereo mode: %s\n",
      stereo == GL_TRUE ? "yes" : "no");

   UaTrace("video driver: %s\n\n", SDL_GetCurrentVideoDriver());
}

/// Cleans up renderer.
void Renderer::Done()
{
   delete m_rendererImpl;
   m_rendererImpl = NULL;

   glDisable(GL_FOG);
}

TextureManager& Renderer::GetTextureManager()
{
   return m_rendererImpl->GetTextureManager();
}

Model3DManager& Renderer::GetModel3DManager()
{
   return m_rendererImpl->GetModel3DManager();
}

CritterFramesManager& Renderer::GetCritterFramesManager()
{
   return m_rendererImpl->GetCritterFramesManager();
}

/// Sets up camera for 2d user interface rendering. All triangles (e.g. quads)
/// should be rendered with z coordinate = 0. Also disables fog, blending and
/// depth test.
void Renderer::SetupForUserInterface()
{
   m_viewport->SetupCamera2D();

   glDisable(GL_DEPTH_TEST);
   glDisable(GL_BLEND);

   glDisable(GL_FOG);
}

/// Sets up camera for 3d scene rendering. Enables depth test and fog.
/// \param viewOffset view offset that is added to the player's position
/// \param fieldOfView field of view angle
/// \param farDistance distance from camera to far plane
void Renderer::SetupFor3D(const Vector3d& viewOffset,
   double fieldOfView, double farDistance)
{
   m_viewOffset = viewOffset;
   m_farDistance = farDistance;
   m_fieldOfView = fieldOfView;

   m_viewport->SetupCamera3D(fieldOfView, farDistance);

   glEnable(GL_DEPTH_TEST);

   // fog
   glEnable(GL_FOG);
   glFogi(GL_FOG_MODE, GL_EXP2);
   glFogf(GL_FOG_DENSITY, 0.2f); // 0.65f
   glFogf(GL_FOG_START, 0.0);
   glFogf(GL_FOG_END, 1.0);
   GLint fog_color[4] = { 0,0,0,0 };
   glFogiv(GL_FOG_COLOR, fog_color);
}

/// Renders the underworld using the current player's view.
/// \param underworld underworld object
void Renderer::RenderUnderworld(const Underworld::Underworld& underworld)
{
   const Underworld::Player& player = underworld.GetPlayer();

   double playerHeight = 0.6 + player.GetHeight();
   Vector3d pos(player.GetXPos(), player.GetYPos(), playerHeight);

   pos += m_viewOffset;

   // render map
   m_rendererImpl->Render(underworld.GetCurrentLevel(), pos, player.GetPanAngle(),
      player.GetRotateAngle(), m_fieldOfView);
}

/// Finds out selected object or tile wall by picking.
/// \param underworld underworld object
/// \param xpos mouse x position in real window coordinates
/// \param ypos mouse y position in real window coordinates
/// \param tilex tile x coordinate of picked target
/// \param tiley tile y coordinate of picked target
/// \param isobj true is returned if an object was picked; otherwise tile
///              walls were picked
/// \param id object list pos of picked object, or texture number of picked
///           tile wall
/// picking tutorial:
/// http://www.lighthouse3d.com/opengl/picking/index.php3
void Renderer::SelectPick(const Underworld::Underworld& underworld, unsigned int xpos,
   unsigned int ypos, unsigned int& tilex, unsigned int& tiley, bool& isObject,
   unsigned int& id)
{
   // set selection buffer
   GLuint selectionBuffer[64];
   {
      glSelectBuffer(64, selectionBuffer);

      // render objects in selection mode
      glRenderMode(GL_SELECT);

      // init name stack
      glInitNames();
   }

   // save projection matrix on stack
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();

   // set up projection matrix for selection rendering
   m_viewport->SetupCamera3D(m_fieldOfView, m_farDistance,
      true, xpos, ypos);

   // render scene
   m_rendererImpl->SetSelectionMode(true);
   RenderUnderworld(underworld);
   m_rendererImpl->SetSelectionMode(false);

   // switch off selection mode
   GLint hits = glRenderMode(GL_RENDER);

   // restore previous projection matrix
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);

   // find out hit object
   GLint hitIndex = 0;
   if (hits > 0)
   {
      GLuint min = 0xffffffff;
      unsigned int idx = 0;

      for (int i = 0; i < hits && idx < 64; i++)
      {
         // get count of names stored in this record
         GLuint namecount = selectionBuffer[idx++];

         // check min. hit dist.
         if (selectionBuffer[idx] < min)
         {
            // new min. hit dist.
            min = selectionBuffer[idx++];
            idx++; // jump over max. hit dist.
            if (namecount > 0 && namecount == 2)
               hitIndex = idx; // hit index
         }
         else
            idx += 2; // jump over min./max. hit dist.

         // move idx to next record
         idx += namecount;
      }
   }

   if (hitIndex != 0)
   {
      GLuint coords = selectionBuffer[hitIndex];
      GLuint renderId = selectionBuffer[hitIndex + 1];

      tilex = coords & 0x00ff;
      tiley = (coords >> 8) & 0x00ff;
      isObject = renderId < 0x0400;
      id = renderId - (isObject ? 0 : 0x0400);
   }
}

/// Prepares renderer for new level.
/// \param level level to prepare for
void Renderer::PrepareLevel(Underworld::Level& level)
{
   UaTrace("preparing textures for level... ");
   TextureManager& textureManager = GetTextureManager();

   // reset stock texture usage
   textureManager.Reset();

   // prepare all used wall/ceiling textures
   {
      const std::set<Uint16>& usedTextures = level.GetTilemap().GetUsedTextures();

      for (Uint16 textureId : usedTextures)
         textureManager.Prepare(textureId);
   }

   // prepare all switch, door and tmobj textures
   {
      for (unsigned int n = 0; n < 16; n++) textureManager.Prepare(Base::c_stockTexturesSwitches + n);
      for (unsigned int n = 0; n < 13; n++) textureManager.Prepare(Base::c_stockTexturesDoors + n);
      for (unsigned int n = 0; n < 33; n++) textureManager.Prepare(Base::c_stockTexturesTmobj + n);
   }

   // prepare all object images
   {
      for (unsigned int n = 0; n < 0x01c0; n++) textureManager.Prepare(Base::c_stockTexturesObjects + n);
   }

   UaTrace("done\npreparing critter images... ");

   // prepare critters controlled by critter frames manager
   GetCritterFramesManager().Prepare(&level.GetObjectList());

   UaTrace("done\n");
}

/// Does tick processing for renderer for texture and critter frames
/// animation.
/// \param tickRate tick rate in ticks/second
void Renderer::Tick(double tickRate)
{
   // do texture manager tick processing
   GetTextureManager().Tick(tickRate);

   // do critter frames processing, too
   GetCritterFramesManager().Tick(tickRate);
}

void Renderer::GetModel3DBoundingTriangles(unsigned int x,
   unsigned int y, const Underworld::Object& object,
   std::vector<Triangle3dTextured>& allTriangles)
{
   Model3DManager& modelManager = GetModel3DManager();
   if (modelManager.IsModelAvailable(object.GetObjectInfo().m_itemID))
   {
      Vector3d base = m_rendererImpl->CalcObjectPosition(x, y, object);
      modelManager.GetBoundingTriangles(object, base, allTriangles);
   }
}
