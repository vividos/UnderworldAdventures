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
/*! \file game_win32.cpp

   \brief win32 specific game base class implementation

*/

// needed includes
#include "common.hpp"
#include "game_win32.hpp"
#include "resource.h"
#include <SDL_syswm.h>


// implement win32 game object

ua_uwadv_game_implement(ua_uwadv_game_win32);


// ua_uwadv_game_win32 members

void ua_uwadv_game_win32::init()
{
   // call base class method
   ua_uwadv_game::init();

   // get window manager info
   SDL_SysWMinfo info;
   info.window = NULL;

   SDL_VERSION(&info.version); // fill in SDL version
   SDL_GetWMInfo(&info);

   HINSTANCE inst = (HINSTANCE)GetModuleHandle(NULL);

   icon = ::LoadIcon(inst,MAKEINTRESOURCE(IDI_ICON));
   icon_small = (HICON)::LoadImage(inst,
      MAKEINTRESOURCE(IDI_ICON),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);

   // set icons
   ::SendMessage(info.window,WM_SETICON,ICON_BIG,(LPARAM)icon);
   ::SendMessage(info.window,WM_SETICON,ICON_SMALL,(LPARAM)icon_small);
}

void ua_uwadv_game_win32::done()
{
   ua_uwadv_game::done();

   // free icon resources
   ::DestroyIcon(icon);
   ::DestroyIcon(icon_small);
}

void ua_uwadv_game_win32::error_msg(const char* msg)
{
   ua_uwadv_game::error_msg(msg);

   // present the user a message box
   SDL_SysWMinfo info;
   info.window = NULL;

   SDL_GetWMInfo(&info);

   ::MessageBox(info.window, msg, "Underworld Adventures", MB_OK);
}
