/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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
/*! \file game_interface.hpp

   \brief game interface class

*/
//! \ingroup game

//@{

// include guard
#ifndef uwadv_game_interface_hpp_
#define uwadv_game_interface_hpp_

// needed includes


// forward references
class ua_audio_manager;
class ua_settings;
class ua_files_manager;
class ua_savegames_manager;
class ua_image_manager;
class ua_renderer;
class ua_scripting;
class ua_underworld;
class ua_debug_server;
class ua_screen;
class ua_gamestrings;


// enums

//! game events that can be sent to the event queue via SDL_PushEvent using SDL_USEREVENT
enum ua_game_events
{
   //! sent to destroy the current screen, pop the last one off the stack as new current screen
   ua_event_destroy_screen = 0,

   //! event sent when a screen resumes processing, e.g. when getting popped off the screen stack
   ua_event_resume_screen,

   //! sent to current screen when an text edit window finished editing
   ua_event_textedit_finished,

   //! sent to screen when user aborted text editing
   ua_event_textedit_aborted,

   //! last (dummy) event
   ua_event_last
};


// classes

//! basic game interface class, without ui stuff
class ua_basic_game_interface
{
public:
   //! returns game tickrate
   virtual double get_tickrate()=0;

   //! returns settings object
   virtual ua_settings& get_settings()=0;

   //! returns settings object
   virtual ua_files_manager& get_files_manager()=0;

   //! returns savegames manager object
   virtual ua_savegames_manager& get_savegames_manager()=0;

   //! returns scripting object
   virtual ua_scripting& get_scripting()=0;
   
   //! returns debugger object
   virtual ua_debug_server& get_debugger()=0;

   //! returns game strings object
   virtual ua_gamestrings& get_gamestrings()=0;

   //! returns underworld object
   virtual ua_underworld& get_underworld()=0;
};


//! game interface class
class ua_game_interface: public ua_basic_game_interface
{
public:
   //! initializes game; only called after all stuff is initialized and ready
   virtual void init_game()=0;

   //! returns audio manager
   virtual ua_audio_manager& get_audio_manager()=0;

   //! returns image manager object
   virtual ua_image_manager& get_image_manager()=0;

   //! returns renderer object
   virtual ua_renderer& get_renderer()=0;

   //! replaces current screen with new one; saves current on a screen stack when selected
   virtual void replace_screen(ua_screen* new_screen, bool save_current)=0;

   //! removes current screen at next event processing
   virtual void remove_screen()=0;

protected:
   //! ctor
   ua_game_interface(){}
};


#endif
//@}
