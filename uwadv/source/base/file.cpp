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
:m_lFileLength(-1)
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
:m_rwops(rwops),
 m_lFileLength(-1)
{
   UaAssert(rwops != NULL);
}

/*! \return file length, or -1 if the file length couldn't be determined (e.g.
    for streaming sources or compressed files.
*/
long File::FileLength()
{
   if (m_lFileLength != -1)
      return m_lFileLength;

   UaAssert(m_rwops.get() != NULL);

   long lCurPos = Tell();

   if (-1 == SDL_RWseek(m_rwops.get(), 0L, SEEK_END))
      return -1L; // couldn't seek, so file length isn't known

   m_lFileLength = SDL_RWtell(m_rwops.get());

   Seek(lCurPos, seekBegin);

   return m_lFileLength;
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

unsigned int File::ReadBuffer(Uint8* pBuffer, unsigned int uiLength) const
{
   UaAssert(m_rwops.get() != NULL);

   int iRet = SDL_RWread(m_rwops.get(), pBuffer, 1, uiLength);
   if (iRet < 0)
      iRet = 0;

   return static_cast<unsigned int>(iRet);
}

void File::Write8(Uint8 val)
{
   UaAssert(m_rwops.get() != NULL);

   SDL_RWwrite(m_rwops.get(), &val, 1, 1);
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
