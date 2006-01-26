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
/*! \file file.cpp

   \brief file implementation

*/

// needed includes
#include "base.hpp"
#include "file.hpp"

using Base::File;
using Base::EFileOpenMode;
using Base::ESeekMode;

// File methods

File::File(const std::string& strFilename, EFileOpenMode eOpenMode)
{
   UaAssert(strFilename.size() > 0);

   // open from file
   SDL_RWops* rwops = SDL_RWFromFile(strFilename.c_str(),
      eOpenMode == modeRead ? "rb" : "wb");

   m_rwops.reset(rwops);
}

/*! The given rwops structure will be automatically freed at destruction.
    There's no need to free the ptr. */
File::File(SDL_RWops* rwops)
:m_rwops(rwops)
{
   UaAssert(rwops != NULL);
}

/*! \return file length, or -1 if the file length couldn't be determined (e.g.
    for streaming sources or compressed files.
*/
long File::FileLength()
{
   UaAssert(m_rwops.get() != NULL);

   long lCurPos = Tell();

   if (-1 == SDL_RWseek(m_rwops.get(), 0L, SEEK_END))
      return -1L; // couldn't seek, so file length isn't known

   long lFileLength = SDL_RWtell(m_rwops.get());

   Seek(lCurPos, seekBegin);

   return lFileLength;
}

long File::Tell() const
{
   UaAssert(m_rwops.get() != NULL);

   return SDL_RWtell(m_rwops.get());
}

void File::Seek(long lOffset, ESeekMode eSeekMode)
{
   UaAssert(m_rwops.get() != NULL);
   UaAssert(eSeekMode == seekBegin || eSeekMode == seekCurrent);

   SDL_RWseek(m_rwops.get(), lOffset, eSeekMode == seekBegin ? SEEK_SET : SEEK_CUR);
}

Uint8 File::Read8() const
{
   UaAssert(m_rwops.get() != NULL);

   Uint8 val;
   SDL_RWread(m_rwops.get(), &val, 1, 1);
   return val;
}

/*! Always reads little-endian values, even on big endian machines. */
Uint16 File::Read16() const
{
   UaAssert(m_rwops.get() != NULL);

   Uint8 vals[2];
   SDL_RWread(m_rwops.get(), vals, 1, 2);

   return static_cast<Uint16>(vals[0]) |
      (static_cast<Uint16>(vals[1]) << 8);
}

/*! Always reads little-endian values, even on big endian machines. */
Uint32 File::Read32() const
{
   UaAssert(m_rwops.get() != NULL);

   Uint8 vals[4];
   SDL_RWread(m_rwops.get(), vals, 1, 4);

   return static_cast<Uint32>(vals[0]) |
      (static_cast<Uint32>(vals[1]) << 8) |
      (static_cast<Uint32>(vals[2]) << 16) |
      (static_cast<Uint32>(vals[3]) << 24);
}

void File::ReadBuffer(Uint8* pBuffer, unsigned int uiLength) const
{
   UaAssert(m_rwops.get() != NULL);

   SDL_RWread(m_rwops.get(), pBuffer, 1, uiLength);
}

void File::Write8(Uint8 val)
{
   UaAssert(m_rwops.get() != NULL);

   SDL_RWwrite(m_rwops.get(), &val, 1, 1);
}

/*! Always writes little-endian values, even on big endian machines. */
void File::Write16(Uint16 val)
{
   UaAssert(m_rwops.get() != NULL);

   Uint8 vals[2];
   vals[0] = static_cast<Uint8>( val & 0x00ff);
   vals[1] = static_cast<Uint8>((val & 0xff00) >> 8);
   SDL_RWwrite(m_rwops.get(), vals, 1, 2);
}

/*! Always writes little-endian values, even on big endian machines. */
void File::Write32(Uint32 val)
{
   UaAssert(m_rwops.get() != NULL);

   Uint8 vals[4];
   vals[0] = static_cast<Uint8>( val & 0x000000ff);
   vals[1] = static_cast<Uint8>((val & 0x0000ff00) >> 8);
   vals[2] = static_cast<Uint8>((val & 0x00ff0000) >> 16);
   vals[3] = static_cast<Uint8>((val & 0xff000000) >> 24);
   SDL_RWwrite(m_rwops.get(), vals, 1, 4);
}

void File::WriteBuffer(const Uint8* pBuffer, unsigned int uiLength)
{
   UaAssert(m_rwops.get() != NULL);

   SDL_RWwrite(m_rwops.get(), pBuffer, 1, uiLength);
}

/*! note: the file is still open for all other copies of the File object.
*/
void File::Close()
{
   m_rwops.reset();
}
