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
/// \file File.cpp
/// \brief file implementation
//
#include "pch.hpp"
#include "File.hpp"

using Base::File;

File::File(const std::string& filename, Base::FileOpenMode openMode)
   :m_fileLength(-1)
{
   UaAssert(filename.size() > 0);

   SDL_RWops* rwops = SDL_RWFromFile(filename.c_str(),
      openMode == modeRead ? "rb" : "wb");

   m_rwops = MakeRWopsPtr(rwops);
}

File::File(Base::SDL_RWopsPtr rwops)
   :m_rwops(rwops),
   m_fileLength(-1)
{
   UaAssert(rwops.get() != NULL);
}

/// \return file length, or -1 if the file length couldn't be determined (e.g.
/// for streaming sources or compressed files.
long File::FileLength()
{
   if (m_fileLength != -1)
      return m_fileLength;

   UaAssert(m_rwops.get() != NULL);

   long currentPos = Tell();

   if (-1 == SDL_RWseek(m_rwops.get(), 0L, SEEK_END))
      return -1L; // couldn't seek, so file length isn't known

   // cast down; no file is larger than 2^32 bytes
   m_fileLength = static_cast<long>(SDL_RWtell(m_rwops.get()));

   Seek(currentPos, seekBegin);

   return m_fileLength;
}

long File::Tell() const
{
   UaAssert(m_rwops.get() != NULL);

   // cast down; no file is larger than 2^32 bytes
   return static_cast<long>(SDL_RWtell(m_rwops.get()));
}

void File::Seek(long offset, Base::SeekMode seekMode)
{
   UaAssert(m_rwops.get() != NULL);
   UaAssert(seekMode == seekBegin || seekMode == seekCurrent);

   SDL_RWseek(m_rwops.get(), offset, seekMode == seekBegin ? SEEK_SET : SEEK_CUR);
}

Uint8 File::Read8() const
{
   UaAssert(m_rwops.get() != NULL);

   Uint8 value;
   SDL_RWread(m_rwops.get(), &value, 1, 1);
   return value;
}

unsigned int File::ReadBuffer(Uint8* buffer, unsigned int length) const
{
   UaAssert(m_rwops.get() != NULL);

   int ret = SDL_RWread(m_rwops.get(), buffer, 1, length);
   if (ret < 0)
      ret = 0;

   return static_cast<unsigned int>(ret);
}

void File::Write8(Uint8 value)
{
   UaAssert(m_rwops.get() != NULL);

   SDL_RWwrite(m_rwops.get(), &value, 1, 1);
}

void File::WriteBuffer(const Uint8* buffer, unsigned int length)
{
   UaAssert(m_rwops.get() != NULL);

   SDL_RWwrite(m_rwops.get(), buffer, 1, length);
}

/// note: the file is still open for all other copies of the File object.
void File::Close()
{
   m_rwops.reset();
}
