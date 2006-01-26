/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2006 Michael Fink

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
/*! \file resourcemanager.hpp

   \brief resource manager

*/

// include guard
#ifndef uwadv_base_resourcemanager_hpp_
#define uwadv_base_resourcemanager_hpp_

// needed includes
#include <string>

struct SDL_RWops;

namespace Base
{

class Settings;

//! Resource manager
/*! Manages access to resource files. */
class ResourceManager
{
public:
   //! ctor
   ResourceManager(const Settings& settings);

   //! returns "uadata" resource file
   SDL_RWops* GetResourceFile(const std::string& strRelFilename);

protected:
   //! home path
   std::string m_strHomePath;

   //! path to uadata resources
   std::string m_strUaDataPath;
};

} // namespace Base

#endif
