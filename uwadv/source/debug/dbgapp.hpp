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
/*! \file dbgapp.hpp

   \brief debugger main application

*/

// include guard
#ifndef uadebug_dbgapp_hpp_
#define uadebug_dbgapp_hpp_

// needed includes
#include <wx/docview.h>


// classes

//! debugger application
class ua_debugger: public wxApp
{
public:
   //! ctor
   ua_debugger(){}

   //! called on initialisation
   virtual bool OnInit();

   //! command function
   static ua_debug_command_func command;

protected:
   //! document manager
   wxDocManager* doc_manager;
};


DECLARE_APP(ua_debugger)


#endif
