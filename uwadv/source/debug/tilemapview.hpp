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
/*! \file tilemapview.hpp

   \brief tilemap view frame

*/

// include guard
#ifndef uadebug_tilemapview_hpp_
#define uadebug_tilemapview_hpp_

// needed includes
#include "wx/glcanvas.h"


// classes

//! tilemap view OpenGL canvas
class ua_tilemapview_canvas: public wxGLCanvas
{
public:
   //! ctor
   ua_tilemapview_canvas(wxWindow *parent, const wxWindowID id=-1,
      const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
      long style = 0);

   //! dtor
   ~ua_tilemapview_canvas();

protected:
   // message handler

   //! handles resizing
   void OnSize(wxSizeEvent& event);

   //! handles painting
   void OnPaint(wxPaintEvent& event);

   //! handles erasing background
   void OnEraseBackground(wxEraseEvent& event);

   //! inits OpenGL
   void InitGL();

   //! renders scene
   void Render();

protected:
   //! flag that indicates if OpenGL is initialized
   bool inited_gl;

   DECLARE_EVENT_TABLE()
};


//! tilemap view frame window
class ua_tilemapview_frame: public wxMDIChildFrame
{
public:
   //! ctor
   ua_tilemapview_frame(wxMDIParentFrame* parent, wxWindowID id,
      const wxPoint& pos, const wxSize& size, long style);

   //! tilemap view frame name
   static const char* frame_name;

protected:
   //! canvas to draw tilemap
   ua_tilemapview_canvas* canvas;
};

#endif
