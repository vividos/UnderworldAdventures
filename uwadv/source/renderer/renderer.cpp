/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file renderer.cpp

   \brief underworld renderer

   picking tutorial:
   http://www.lighthouse3d.com/opengl/picking/index.php3

*/

// needed includes
#include "common.hpp"
#include "renderer.hpp"
#include "renderer_impl.hpp"
#include "critter.hpp"
#include "models.hpp"
#include "underworld.hpp"
#include "game_interface.hpp"


// constants

//! near plane distance from the camera
const double ua_renderer::near_dist = 0.05;


// ua_renderer methods

ua_renderer::ua_renderer()
:view_offset(0.0, 0.0, 0.0), renderer_impl(NULL)
{
}

ua_renderer::~ua_renderer()
{
   done();
}

/*! Initializes the renderer, the texture manager, critter frames manager and
    OpenGL flags.

    \param game game interface
*/
void ua_renderer::init(ua_game_interface& game)
{
   renderer_impl = new ua_renderer_impl;
   if (renderer_impl == NULL)
      throw ua_exception("couldn't create ua_renderer_impl class");

   get_texture_manager().init(game);
   get_critter_frames_manager().init(game.get_settings(), game.get_image_manager());

   // culling: only render front face, counter clockwise
   glCullFace(GL_BACK);
   glFrontFace(GL_CCW);
   glEnable(GL_CULL_FACE);

   // enable z-buffer
   glEnable(GL_DEPTH_TEST);

   // enable texturing
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,0);

   // smooth shading
   glShadeModel(GL_SMOOTH);

   // give some rendering hints
   glHint(GL_FOG_HINT,GL_DONT_CARE);
   glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
   glHint(GL_POLYGON_SMOOTH_HINT,GL_DONT_CARE);
}

void ua_renderer::done()
{
   delete renderer_impl;
   renderer_impl = NULL;

   glDisable(GL_FOG);
}

void ua_renderer::clear()
{
   glClearColor(0,0,0,0);
   glClear(GL_COLOR_BUFFER_BIT);
   SDL_GL_SwapBuffers();
}

ua_texture_manager& ua_renderer::get_texture_manager()
{
   return renderer_impl->get_texture_manager();
}

ua_critter_frames_manager& ua_renderer::get_critter_frames_manager()
{
   return renderer_impl->get_critter_frames_manager();
}

void ua_renderer::setup_camera2d()
{
   // setup orthogonal projection
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0,320,0,200);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glDisable(GL_DEPTH_TEST);
   glDisable(GL_BLEND);

   glDisable(GL_FOG);
}

void ua_renderer::setup_camera3d(const ua_vector3d& the_view_offset,
   double the_fov, double the_far_dist)
{
   view_offset = the_view_offset;
   far_dist = the_far_dist;
   fov = the_fov;

   view_offset.z += 20.0;

   // set projection matrix
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   SDL_Surface* surf = SDL_GetVideoSurface();
   double aspect = surf->w / surf->h;

   gluPerspective(fov, aspect, near_dist, far_dist);

   // switch back to modelview matrix
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glEnable(GL_DEPTH_TEST);

   // fog
   glEnable(GL_FOG);
   glFogi(GL_FOG_MODE,GL_EXP2);
   glFogf(GL_FOG_DENSITY,0.2f); // 0.65f
   glFogf(GL_FOG_START,0.0);
   glFogf(GL_FOG_END,1.0);
   GLint fog_color[4] = { 0,0,0,0 };
   glFogiv(GL_FOG_COLOR,fog_color);
}

void ua_renderer::render_underworld(const ua_underworld& underw)
{
   const ua_player& pl = underw.get_player();

   double plheight = 0.6+pl.get_height();
   ua_vector3d pos(pl.get_xpos(),pl.get_ypos(),plheight);

   pos += view_offset;

   // render map
   renderer_impl->render(underw.get_current_level(),pos,pl.get_angle_pan(),
      pl.get_angle_rot(), fov);
}

void ua_renderer::select_pick(const ua_underworld& underw, unsigned int xpos,
   unsigned int ypos, unsigned int& tilex, unsigned int& tiley, bool& isobj,
   unsigned int& id)
{
   // set selection buffer
   GLuint select_buf[64];
   {
      glSelectBuffer(64,select_buf);

      // render objects in selection mode
      glRenderMode(GL_SELECT);

      // init name stack
      glInitNames();
   }

   // save projection matrix on stack
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();

   // set up projection matrix for selection rendering
   {
      //glMatrixMode(GL_PROJECTION);
      glLoadIdentity();

      // calculate pick matrix
      GLint viewport[4];
      glGetIntegerv(GL_VIEWPORT, viewport);
      gluPickMatrix(GLdouble(xpos), GLdouble(viewport[3]-ypos), 5.0, 5.0, viewport);

      SDL_Surface* surf = SDL_GetVideoSurface();
      double aspect = surf->w / surf->h;

      gluPerspective(fov, aspect, near_dist, far_dist);

      // switch back to modelview matrix
      glMatrixMode(GL_MODELVIEW);
   }

   // render scene
//   selection_mode = true;
   render_underworld(underw);
//   selection_mode = false;

   // switch off selection mode
   GLint hits = glRenderMode(GL_RENDER);

   // restore previous projection matrix
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);

   // find out hit object
   GLint hitidx = 0;
   if (hits>0)
   {
      GLuint min = 0xffffffff;
      unsigned int idx = 0;

      for(int i=0; i<hits && idx<64; i++)
      {
         // get count of names stored in this record
         GLuint namecount = select_buf[idx++];

         // check min. hit dist.
         if (select_buf[idx] < min)
         {
            // new min. hit dist.
            min = select_buf[idx++];
            idx++; // jump over max. hit dist.
            if (namecount>0 && namecount==2)
               hitidx = idx; // hit index
         }
         else
            idx+=2; // jump over min./max. hit dist.

         // move idx to next record
         idx += namecount;
      }
   }

   if (hitidx!=0)
   {
      GLuint coords = select_buf[hitidx];
      GLuint renderid = select_buf[hitidx+1];

      tilex = coords & 0x00ff;
      tiley = (coords>>8) & 0x00ff;
      isobj = renderid<0x0400;
      id = renderid - (isobj ? 0 : 0x0400);
   }
}

void ua_renderer::prepare_level(ua_level& level)
{
   ua_trace("preparing textures for level... ");
   ua_texture_manager& texmgr = get_texture_manager();

   // reset stock texture usage
   texmgr.reset();

   // prepare all used wall/ceiling textures
   {
      const std::vector<Uint16>& used_textures = level.get_used_textures();

      unsigned int max = used_textures.size();
      for(unsigned int n=0; n<max; n++)
         texmgr.prepare(used_textures[n]);
   }

   // prepare all switch, door and tmobj textures
   {
      unsigned int n;
      for(n=0; n<16; n++) texmgr.prepare(ua_tex_stock_switches+n);
      for(n=0; n<13; n++) texmgr.prepare(ua_tex_stock_door+n);
      for(n=0; n<33; n++) texmgr.prepare(ua_tex_stock_tmobj+n);
   }

   // prepare all object images
   {
      for(unsigned int n=0; n<0x01c0; n++) texmgr.prepare(ua_tex_stock_objects+n);
   }

   // reset critters controlled by critter frames manager
   //get_critter_frames_manager().prepare(get_texture_manager(),&level.get_mapobjects());

   ua_trace("done\n");
}

/*! \todo enable critter pool tick() call */
void ua_renderer::tick(double ticktime)
{
   // do texture manager tick processing
   get_texture_manager().tick(ticktime);

   // do critter frames processing, too
   get_critter_frames_manager().tick(ticktime);
}
