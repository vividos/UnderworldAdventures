/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2002,2003,2004,2005,2006 Michael Fink

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
/*! \file level.hpp

   \brief level

*/

// include guard
#ifndef uwadv_underw_level_hpp_
#define uwadv_underw_level_hpp_

// needed includes
#include "tilemap.hpp"
#include "objectlist.hpp"
#include "mapnotes.hpp"

namespace Base
{
   class Savegame;
}

namespace Underworld
{

//! Level of the underworld
/*! A level of the underworld consists of a tilemap, an object list and automap
    notes. A level can have a unique name.
*/
class Level
{
public:
   //! ctor
   Level(){}

   //! returns level name
   std::string GetLevelName() const { return m_strLevelName; }

   //! sets new level name
   void SetLevelName(const std::string& strLevelName){ m_strLevelName = strLevelName; }

   // level contents

   //! returns tilemap
   Tilemap& GetTilemap(){ return m_tilemap; }
   //! returns tilemap
   const Tilemap& GetTilemap() const { return m_tilemap; }

   //! returns object list
   ObjectList& GetObjectList(){ return m_objectList; }
   //! returns object list
   const ObjectList& GetObjectList() const { return m_objectList; }

   //! returns map notes
   MapNotes& GetMapNotes(){ return m_mapNotes; }
   //! returns map notes
   const MapNotes& GetMapNotes() const { return m_mapNotes; }

   // loading / saving

   //! saves level
   void Load(Base::Savegame& sg)
   {
      GetTilemap().Load(sg);
      GetObjectList().Load(sg);
      GetMapNotes().Load(sg);
   }

   //! loads level
   void Save(Base::Savegame& sg) const
   {
      GetTilemap().Save(sg);
      GetObjectList().Save(sg);
      GetMapNotes().Save(sg);
   }

private:
   //! level name
   std::string m_strLevelName;

   //! tilemap of level
   Tilemap m_tilemap;

   //! level object list
   ObjectList m_objectList;

   //! automap notes
   MapNotes m_mapNotes;
};

} // namespace Underworld

#endif
