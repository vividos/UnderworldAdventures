/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

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
/*! \file core.hpp

   \brief game core interface to access all game objects

*/

// include guard
#ifndef __uwadv_core_hpp_
#define __uwadv_core_hpp_

// needed includes
#include "audio.hpp"
#include "texture.hpp"
#include "settings.hpp"
#include "files.hpp"
#include "critter.hpp"
#include "gamestrings.hpp"
#include "underworld.hpp"
#include "screen.hpp"
#include "savegame.hpp"


// forward declarations
class ua_underworld;
class ua_ui_screen_base;


// classes

//! game core abstract base class
class ua_game_core_interface
{
public:
   ua_game_core_interface(){}
   virtual ~ua_game_core_interface(){}

   //! called as soon as a game path is available
   virtual void init_core()=0;

   //! returns screen width
   virtual unsigned int get_screen_width()=0;
   //! returns screen height
   virtual unsigned int get_screen_height()=0;

   //! returns tick rate in ticks / second
   virtual unsigned int get_tickrate()=0;

   //! returns audio interface
   virtual ua_audio_interface &get_audio()=0;

   //! returns game strings
   virtual ua_gamestrings &get_strings()=0;

   //! returns settings object
   virtual ua_settings &get_settings()=0;

   //! returns texture manager object
   virtual ua_texture_manager &get_texmgr()=0;

   //! returns files manager object
   virtual ua_files_manager &get_filesmgr()=0;

   //! returns critter pool object
   virtual ua_critter_pool& get_critter_pool()=0;

   //! returns savegames manager object
   virtual ua_savegames_manager& get_savegames_mgr()=0;

   //! returns underworld object
   virtual ua_underworld &get_underworld()=0;


   //! sets new screen as top screen stack
   virtual void push_screen(ua_ui_screen_base *newscreen)=0;

   //! replaces current screen with new one
   virtual void replace_screen(ua_ui_screen_base *newscreen)=0;

   //! destroys current screen (when finished tick()) and uses the last one
   virtual void pop_screen()=0;
};

#endif
