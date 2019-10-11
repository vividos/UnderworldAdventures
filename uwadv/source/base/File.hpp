//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2006,2019 Underworld Adventures Team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
/// \file File.hpp
/// \brief File class
//
#pragma once

#include "Base.hpp"
#include <string>
#include <SDL_types.h>
#include <SDL_rwops.h>
#include <SDL_endian.h>

namespace Base
{
   /// file open mode for File class
   enum FileOpenMode
   {
      modeRead,   ///< open in read mode
      modeWrite,  ///< create a new file in write mode
   };

   /// seek mode for File::Seek()
   enum SeekMode
   {
      seekBegin,     ///< seek position is absolute
      seekCurrent,   ///< seek position is relative to current position
   };

   /// \brief File class
   /// Note: the Read16 and Read32 functions always read little-endian values,
   /// and the Write16 and Write32 functions always write little-endian values.
   /// The underlying SDL_RWops pointer is deleted and the file is closed as soon
   /// as no instance is using the pointer anymore.
   class File
   {
   public:
      /// default ctor; doesn't open file
      File()
         :m_fileLength(-1)
      {
      }

      /// ctor; openes per filename
      File(const std::string& filename, FileOpenMode openMode);

      /// ctor; uses opened SDL_RWops pointer
      File(SDL_RWopsPtr rwops);

      /// copy ctor
      File(const File& file)
         :m_fileLength(-1)
      {
         operator=(file);
      }

      /// dtor; closes file when still open and no more copies of the object exist
      ~File()
      {
      }

      /// assignment operator
      File& operator=(const File& file)
      {
         m_rwops = file.m_rwops;
         m_fileLength = file.m_fileLength;
         return *this;
      }

      /// returns if file is open
      bool IsOpen() const { return m_rwops.get() != NULL; }

      /// returns file length
      long FileLength();

      /// tells current file position
      long Tell() const;

      /// seeks to file location
      void Seek(long offset, SeekMode seekMode);

      /// reads 8-bit value from file
      Uint8 Read8() const;
      /// reads 16-bit value from file
      Uint16 Read16() const { return SDL_ReadLE16(m_rwops.get()); }
      /// reads 32-bit value from file
      Uint32 Read32() const { return SDL_ReadLE32(m_rwops.get()); }
      /// reads array from file into buffer
      size_t ReadBuffer(Uint8* buffer, size_t length) const;

      /// writes 8-bit value to file
      void Write8(Uint8 value);
      /// writes 16-bit value to file
      void Write16(Uint16 value) { SDL_WriteLE16(m_rwops.get(), value); }
      /// writes 32-bit value to file
      void Write32(Uint32 value) { SDL_WriteLE32(m_rwops.get(), value); }
      /// writes buffer to file
      void WriteBuffer(const Uint8* buffer, size_t length);

      /// closes file
      void Close();

   private:
      /// internal rwops ptr
      SDL_RWopsPtr m_rwops;

      /// file length
      long m_fileLength;
   };

} // namespace Base
