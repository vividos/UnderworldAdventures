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
/*! \file tilemapview.cpp

   \brief tilemap view frame implementation

*/

// needed includes
#include "dbgcommon.hpp"
#include "tilemapview.hpp"
#include "dbgapp.hpp"


// event tables

BEGIN_EVENT_TABLE(ua_tilemapview_canvas, wxGLCanvas)
   EVT_SIZE(ua_tilemapview_canvas::OnSize)
   EVT_PAINT(ua_tilemapview_canvas::OnPaint)
   EVT_ERASE_BACKGROUND(ua_tilemapview_canvas::OnEraseBackground)
END_EVENT_TABLE()


// static variables

//! tilemap view frame name
const char* ua_tilemapview_frame::frame_name = "tilemap_view_frame";


// ua_tilemapview_canvas methods
ua_tilemapview_canvas::ua_tilemapview_canvas(wxWindow *parent, const wxWindowID id,
   const wxPoint& pos, const wxSize& size, long style)
:wxGLCanvas(parent,id,pos,size,style,"tilemap_view_canvas"),inited_gl(false)
{
}

ua_tilemapview_canvas::~ua_tilemapview_canvas()
{
   // deinit tilemap renderer
   ua_debug_command_func cmd = wxGetApp().command;

   cmd(udc_lock,0,NULL,NULL);
   cmd(udc_render,1,NULL,NULL);
   cmd(udc_unlock,0,NULL,NULL);
}

void ua_tilemapview_canvas::InitGL()
{
   /* set viewing projection */
   glMatrixMode(GL_PROJECTION);
   glFrustum(-0.5F, 0.5F, -0.5F, 0.5F, 1.0F, 3.0F);

   /* position viewer */
   glMatrixMode(GL_MODELVIEW);
   glTranslatef(0.0F, 0.0F, -2.0F);

   /* position object */
   glRotatef(30.0F, 1.0F, 0.0F, 0.0F);
   glRotatef(30.0F, 0.0F, 1.0F, 0.0F);

   glEnable(GL_DEPTH_TEST);
/*   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
*/

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum(-0.5F, 0.5F, -0.5F, 0.5F, 1.0F, 3.0F);
   glMatrixMode(GL_MODELVIEW);


   // init tilemap renderer
   {
      ua_debug_command_func cmd = wxGetApp().command;

      cmd(udc_lock,0,NULL,NULL);
      cmd(udc_render,0,NULL,NULL);

      ua_debug_param param1;
      param1.set_int(0);
      cmd(udc_render,3,&param1,NULL);

      cmd(udc_unlock,0,NULL,NULL);
   }
}

void ua_tilemapview_canvas::Render()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   ua_debug_command_func cmd = wxGetApp().command;

   // render underworld
   cmd(udc_lock,0,NULL,NULL);
   cmd(udc_render,2,NULL,NULL);
   cmd(udc_unlock,0,NULL,NULL);
}

void ua_tilemapview_canvas::OnSize(wxSizeEvent& event)
{
   // this is also necessary to update the context on some platforms
/*   wxGLCanvas::OnSize(event);

   // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
   int w, h;
   GetClientSize(&w, &h);
#ifndef __WXMOTIF__
   if (GetContext())
#endif
   {
      SetCurrent();
      glViewport(0, 0, (GLint) w, (GLint) h);
   }
*/
}

void ua_tilemapview_canvas::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

#ifndef __WXMOTIF__
    if (!GetContext()) return;
#endif

   SetCurrent();

   if (!inited_gl)
   {
      inited_gl = true;
      InitGL();
   }

   Render();

   glFlush();
   SwapBuffers();
}

void ua_tilemapview_canvas::OnEraseBackground(wxEraseEvent& event)
{
   // do nothing, to avoid flashing
}


// ua_tilemapview_frame methods

ua_tilemapview_frame::ua_tilemapview_frame(wxMDIParentFrame* parent, wxWindowID id,
   const wxPoint& pos, const wxSize& size, long style)
:wxMDIChildFrame(parent, id, "Tilemap View", pos, size, style,frame_name)
{
   canvas = new ua_tilemapview_canvas(this);
}
