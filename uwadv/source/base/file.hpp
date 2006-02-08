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
/*! \file file.hpp

   \brief File class

*/

// include guard
#ifndef uwadv_base_file_hpp_
#define uwadv_base_file_hpp_

// needed includes
#include <string>
#include <SDL_types.h>
#include <SDL_rwops.h>
#include <SDL_endian.h>
#include "smart_ptr.hpp"

namespace Base
{

//! file open mode for File class
enum EFileOpenMode
{
   modeRead,   //!< open in read mode
   modeWrite,  //!< create a new file in write mode
};

//! seek mode for File::Seek()
enum ESeekMode
{
   seekBegin,     //!< seek position is absolute
   seekCurrent,   //!< seek position is relative to current position
};


//! delete functor for SDL_RWops struct ptr; used with Base::SmartPtr
class SDL_RWopsDeletor
{
public:
   //! closes rwops file
   void operator()(SDL_RWops* rwops)
   {
      if (rwops != NULL)
         SDL_RWclose(rwops);
   }
};


//! File class
/*! Note: the Read16 and Read32 functions always read little-endian values,
    and the Write16 and Write32 functions always write little-endian values.
*/
class File
{
public:
   //! default ctor; doesn't open file
   File():m_lFileLength(-1){}
   //! ctor; openes per filename
   File(const std::string& strFilename, EFileOpenMode eOpenMode);
   //! ctor; uses opened SDL_RWops pointer
   File(SDL_RWops* rwops);
   //! copy ctor
   File(const File& file):m_lFileLength(-1){ operator=(file); }
   //! dtor; closes file when still open and no more copies of the object exist
   ~File(){}
   //! assignment operator
   File& operator=(const File& file)
   {
      m_rwops = file.m_rwops;
      m_lFileLength = file.m_lFileLength;
      return *this;
   }

   //! returns if file is open
   bool IsOpen() const { return m_rwops.get() != NULL; }

   //! returns file length
   long FileLength();

   //! tells current file position
   long Tell() const;

   //! seeks to file location
   void Seek(long lOffset, ESeekMode eSeekMode);

   //! reads 8-bit value from file
   Uint8 Read8() const;
   //! reads 16-bit value from file
   Uint16 Read16() const { return SDL_ReadLE16(m_rwops.get()); }
   //! reads 32-bit value from file
   Uint32 Read32() const { return SDL_ReadLE32(m_rwops.get()); }
   //! reads array from file into buffer
   unsigned int ReadBuffer(Uint8* pBuffer, unsigned int uiLength) const;

   //! writes 8-bit value to file
   void Write8(Uint8 val);
   //! writes 16-bit value to file
   void Write16(Uint16 val){ SDL_WriteLE16(m_rwops.get(), val); }
   //! writes 32-bit value to file
   void Write32(Uint32 val){ SDL_WriteLE32(m_rwops.get(), val); }
   //! writes buffer to file
   void WriteBuffer(const Uint8* pBuffer, unsigned int uiLength);

   //! closes file
   void Close();

private:
   //! internal rwops ptr
   Base::SmartPtr<SDL_RWops, Base::SDL_RWopsDeletor> m_rwops;

   //! file length
   long m_lFileLength;
};

} // namespace Base

#endif
