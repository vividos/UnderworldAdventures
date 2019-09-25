//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Underworld Adventures Team
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
/// \file IODataSource.hpp
/// \brief input and output data sources for reading and writing MIDI files
//
#pragma once

#pragma warning(push)
#pragma warning(disable: 4244) // 'initializing': conversion from 'T1' to 'T2', possible loss of data
#include "midi/XMidiFile.h"
#include "midi/MidiDriver.h"
#include "midi/IDataSource.h"
#include "midi/ODataSource.h"
#pragma warning(pop)

namespace Detail
{
   /// \brief Input data source for Pentagram code
   /// provides implementation for Pentagram's IDataSource interface to input
   /// .xmi files into XMidiFile.
   /// The SDL_RWops ptr is closed automatically by this class.
   class InputDataSource : public IDataSource
   {
   public:
      /// ctor
      InputDataSource(Base::SDL_RWopsPtr rwops)
         :m_rwops(rwops),
         m_fileSize((uint32)-1)
      {
      }
      /// dtor
      virtual ~InputDataSource()
      {
      }

      /// reads 1 byte
      virtual uint8 read1() override
      {
         uint8 u;
         SDL_RWread(m_rwops.get(), &u, 1, 1);
         return u;
      }

      /// reads 2 bytes, little endian
      virtual uint16 read2() override
      {
         return SDL_ReadLE16(m_rwops.get());
      }

      /// reads 2 bytes, big endian
      virtual uint16 read2high() override
      {
         return SDL_ReadBE16(m_rwops.get());
      }

      /// not implemented
      virtual uint32 read3() override
      {
         UaAssert(false);
         return 0;
      }

      /// reads 4 bytes, little endian
      virtual uint32 read4() override
      {
         return SDL_ReadLE32(m_rwops.get());
      }

      /// reads 4 bytes, big endian
      virtual uint32 read4high() override
      {
         return SDL_ReadBE32(m_rwops.get());
      }

      /// reads memory block
      virtual sint32 read(void *str, sint32 num_bytes) override
      {
         return static_cast<sint32>(SDL_RWread(m_rwops.get(), str, 1, num_bytes));
      }

      /// seeks to file position
      virtual void seek(uint32 pos) override
      {
         SDL_RWseek(m_rwops.get(), pos, SEEK_SET);
      }

      /// skip bytes
      virtual void skip(sint32 delta) override
      {
         SDL_RWseek(m_rwops.get(), delta, SEEK_CUR);
      }

      /// returns size of file
      virtual uint32 getSize() override
      {
         if (m_fileSize != (uint32)-1)
            return m_fileSize;

         uint32 curPos = static_cast<uint32>(SDL_RWtell(m_rwops.get()));
         SDL_RWseek(m_rwops.get(), 0L, SEEK_END);
         m_fileSize = getPos();
         seek(curPos);

         return m_fileSize;
      }

      /// returns current file pos
      virtual uint32 getPos() override
      {
         return static_cast<uint32>(SDL_RWtell(m_rwops.get()));
      }

      /// returns if already at end of file
      virtual bool eof() override
      {
         return getPos() >= getSize();
      }

   private:
      /// rwops ptr
      Base::SDL_RWopsPtr m_rwops;

      /// cached file size
      uint32 m_fileSize;
   };

   /// \brief Output data source for Pentagram code
   /// provides implementation for Pentagram's ODataSource interface to output
   /// .xmi files into XMidiFile.
   class OutputDataSource : public ODataSource
   {
   public:
      /// ctor
      OutputDataSource(Base::SDL_RWopsPtr rwops)
         :m_rwops(rwops)
      {
      }
      /// dtor
      virtual ~OutputDataSource()
      {
      }

      /// writes 1 byte
      virtual void write1(uint32 u) override
      {
         SDL_WriteU8(m_rwops.get(), static_cast<Uint8>(u));
      }

      /// writes 2 bytes, little endian
      virtual void write2(uint16 u) override
      {
         SDL_WriteLE16(m_rwops.get(), static_cast<Uint16>(u));
      }

      /// writes 2 bytes, big endian
      virtual void write2high(uint16 u) override
      {
         SDL_WriteBE16(m_rwops.get(), static_cast<Uint16>(u));
      }

      /// not implemented
      virtual void write3(uint32) override
      {
         UaAssert(false);
      }

      /// writes 4 bytes, little endian
      virtual void write4(uint32 u) override
      {
         SDL_WriteLE32(m_rwops.get(), u);
      }

      /// writes 4 bytes, big endian
      virtual void write4high(uint32 u) override
      {
         SDL_WriteBE32(m_rwops.get(), u);
      }

      /// writes memory block
      virtual void write(const void *str, uint32 num_bytes) override
      {
         SDL_RWwrite(m_rwops.get(), str, 1, num_bytes);
      }

      /// seeks to file position
      virtual void seek(uint32 pos) override
      {
         SDL_RWseek(m_rwops.get(), pos, RW_SEEK_SET);
      }

      /// skip bytes
      virtual void skip(sint32 delta) override
      {
         SDL_RWseek(m_rwops.get(), delta, RW_SEEK_CUR);
      }

      /// returns size of file
      virtual uint32 getSize() override
      {
         return static_cast<uint32>(SDL_RWsize(m_rwops.get()));
      }

      /// returns current file pos
      virtual uint32 getPos() override
      {
         return static_cast<uint32>(SDL_RWtell(m_rwops.get()));
      }

   private:
      /// rwops ptr
      Base::SDL_RWopsPtr m_rwops;
   };

} // namespace Detail
