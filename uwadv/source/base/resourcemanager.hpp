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

namespace Base
{

//! resource path for GetUnderworldFile()
enum EUnderworldResourcePath
{
   resourceGameUw,   //!< load file from currently selected uw game
   resourceGameUw1,  //!< load file from uw1 game (if path is specified)
   resourceGameUw2,  //!< load file from uw2 game (if path is specified)
};

class Settings;

//! Resource manager
/*! Manages access to resource files. */
class ResourceManager
{
public:
   //! ctor
   ResourceManager(const Settings& settings);

   //! returns "uadata" resource file
   SDL_RWopsPtr GetResourceFile(const std::string& strRelFilename);

   //! returns ultima underworld file
   SDL_RWopsPtr GetUnderworldFile(EUnderworldResourcePath resPath, const std::string& strRelFilePath);

protected:
   //! maps lowercase filename to real filename for underworld data files
   void MapUnderworldFilename(std::string& strRelFilename);

protected:
   //! home path
   std::string m_strHomePath;

   //! path to uadata resources
   std::string m_strUaDataPath;

   //! path to uw game currently selected
   std::string m_strUwPath;

   //! path to uw1 game
   std::string m_strUw1Path;

   //! path to uw2 game
   std::string m_strUw2Path;
};

} // namespace Base

#endif