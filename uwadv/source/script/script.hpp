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
/*! \file script.hpp

   \brief scripting support for Underworld Adventures

*/

// include guard
#ifndef uwadv_script_hpp_
#define uwadv_script_hpp_

// needed includes
#include "game_interface.hpp"
#include "underworld.hpp"


// enums

//! scripting language
enum ua_scripting_language
{
   ua_script_lang_lua=0,
};


// classes

//! scripting interface class
class ua_scripting
{
public:
   //! ctor
   ua_scripting(){}
   virtual ~ua_scripting(){}

   //! inits scripting
   virtual void init(ua_game_interface* game)=0;

   //! loads a script into scripting engine
   virtual bool load_script(const char* basename)=0;

   //! cleans up scripting
   virtual void done()=0;

   //! called to init new game
   virtual void init_new_game()=0;

   //! evaluates a critter in master object list
   virtual void eval_critter(unsigned int pos)=0;

   //! executes a trigger action
   virtual void do_trigger(unsigned int pos)=0;

   //! user performed an action
   virtual void user_action(ua_underworld_user_action action,
      unsigned int param)=0;

   //! notifies script that the current level has changed
   virtual void on_changing_level()=0;

   //! performs object "look" action
   virtual void object_look(unsigned int pos)=0;

   //! performs object "use" action
   virtual void object_use(unsigned int pos)=0;

   //! performs inventory item "look" action
   virtual void inventory_look(unsigned int pos)=0;

   //! performs inventory item "use" action
   virtual void inventory_use(unsigned int pos)=0;

   //! combines two objects in inventory
   virtual void inventory_combine(unsigned int pos,unsigned int pos2)=0;

   //! creates new scripting object
   static ua_scripting* create_scripting(ua_scripting_language lang);
};

#endif
