//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2006,2019 Michael Fink
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
/// \file VoiceFile.cpp
/// \brief .voc file implementation
//
#include "pch.hpp"
#include "VoiceFile.hpp"
#include <SDL_rwops.h>
#include <SDL_endian.h>

namespace Detail
{
   void ResampleChunk12048_22050(const std::vector<double>& sourceSamples,
      std::vector<double>& destSamples);

   /// \brief voc file loader
   /// \details Loads Creative Voice file (.voc) and resamples the audio data if
   /// necessary. The class can cope with 12048 Hz .voc files usually found in
   /// uw1, and does the following audio rate conversion:
   /// * 12048 Hz -> 44100 Hz (true resampling)
   /// * 11111 Hz -> 11025 Hz (just remapping so that SDL audio can load the file
   ///
   /// The reason for this class is because SDL_mixer's .voc loader does a bad
   /// job of loading a .voc file. No integral sample rates can be specified in
   /// the file, and that's why the strange sample rates above happen. We just
   /// satisfy SDL here by providing a fake .wav file that contains proper sample
   /// rates and properly converted samples.
   class VoiceFileLoader
   {
   public:
      /// ctor
      VoiceFileLoader(Base::SDL_RWopsPtr rwops)
         :m_rwops(rwops)
      {
      }

      /// loads voc file
      void Load();

      /// resamples audio if neccessary
      void ResampleAudio();

      /// generate in-memory .wav file
      void GenerateWaveFile(std::vector<Uint8>& waveFile);

   private:
      /// ptr to read .voc file from
      Base::SDL_RWopsPtr m_rwops;

      /// samplerate of .voc file
      Uint32 m_sampleRate;

      /// audio samples
      std::vector<Sint16> m_audioSample;
   };

   /// The .voc file format is documented in:
   /// http://icculus.org/SDL_sound/downloads/external_documentation/Voc.txt
   /// \todo reimplement using Base::File
   void VoiceFileLoader::Load()
   {
      const char* c_headerString = "Creative Voice File\x1a";

      // read in header
      char headerString[0x13 + 1];
      SDL_RWread(m_rwops.get(), headerString, 1, SDL_TABLESIZE(headerString));
      UaAssert(0 == strncmp(headerString, c_headerString, SDL_TABLESIZE(headerString)));

      Uint16 offsetDataBlock = SDL_ReadLE16(m_rwops.get());
      UaAssert(offsetDataBlock == 0x001a);

      Uint16 versionNumber = SDL_ReadLE16(m_rwops.get());
      UaAssert(versionNumber == 0x010a); // assume version 1.10 always

      Uint16 versionNumber1sComplement = SDL_ReadLE16(m_rwops.get());
      UaAssert(versionNumber1sComplement == 0x1129);

      // read in first data block
      Uint8 blockType = 0;

      // read in block type; must be 1
      SDL_RWread(m_rwops.get(), &blockType, 1, 1);
      UaAssert(blockType == 1);

      // read in size
      Uint32 size = static_cast<Uint32>(SDL_ReadLE16(m_rwops.get()));

      Uint8 sizeHigh;
      SDL_RWread(m_rwops.get(), &sizeHigh, 1, 1);
      size |= static_cast<Uint32>(sizeHigh) << 16;

      // read samplerate
      Uint8 codedSamplerate = 0;
      SDL_RWread(m_rwops.get(), &codedSamplerate, 1, 1);

      // note: this formula comes from the above URL, but it seems to be wrong
      // for uw1 and uw2 files (or it's just flawed)
      // uw1 .voc files give 0xAD as coded sample rate, which represents 12048 Hz
      // uw2 .voc files give 0xA5, which represents 11111 Hz
      m_sampleRate = 1000000 / (256 - codedSamplerate);

      Uint8 compressionType = 0;
      SDL_RWread(m_rwops.get(), &compressionType, 1, 1);
      UaAssert(compressionType == 0); // 8-bit

      // read in 8-bit samples
      std::vector<Uint8> rawSamples(size);
      SDL_RWread(m_rwops.get(), &rawSamples[0], 1, size);

      // convert to signed 16-bit
      m_audioSample.resize(size);
      for (unsigned int index = 0; index < size; index++)
         m_audioSample[index] = (Sint16(rawSamples[index]) - 128) << 8;
   }

   void VoiceFileLoader::ResampleAudio()
   {
      // SDL_mixer calculates the samplerate wrongly, so fix this for 11025 Hz
      // .voc files
      if (m_sampleRate == 11111)
         m_sampleRate = 11025;

      // check if we need to convert the file
      if (m_sampleRate != 12048)
         return;

      // convert samples to double
      // audio length is doubled by interleaving samples with zeros
      // this is the same as upsampling with factor 2
      // the mirrors created in the spectrum by this method are filtered out
      // afterwards when doing resampling from 12048 Hz to 22050 Hz
      std::vector<double> sourceSamples, destSamples;

      unsigned int size = m_audioSample.size();
      sourceSamples.resize(size * 2, 0);

      for (unsigned int index = 0; index < size; index++)
         sourceSamples[index * 2] = static_cast<double>(m_audioSample[index]);

      ResampleChunk12048_22050(sourceSamples, destSamples);

      // convert back to signed 16-bit values
      // note: the last 7 samples are not used, since they might contain erroneous
      // sample from resampling
      size = destSamples.size();
      m_audioSample.resize(size);

      for (unsigned int index = 0; index < size - 7; index++)
         m_audioSample[index] = static_cast<Sint16>(destSamples[index]);

      m_sampleRate = 44100;
   }

   /// Produces a .wav file in memory with the audio samples read from .voc file.
   /// The wave file format is described here:
   /// http://www.borg.com/~jglatt/tech/wave.htm
   void VoiceFileLoader::GenerateWaveFile(std::vector<Uint8>& waveFile)
   {
      // prepare data bytes array for wave file
      unsigned int fileSize = 12 + 24 + 8 + m_audioSample.size() * sizeof(Sint16);
      waveFile.resize(fileSize);

      SDL_RWops* rwops = ::SDL_RWFromMem(&waveFile[0], waveFile.size());

      // header: 12 bytes
      SDL_RWwrite(rwops, "RIFF", 4, 1);
      SDL_WriteLE32(rwops, fileSize - 8);
      SDL_RWwrite(rwops, "WAVE", 4, 1);

      // format chunk: 24 bytes
      SDL_RWwrite(rwops, "fmt ", 4, 1);
      SDL_WriteLE32(rwops, 16);
      SDL_WriteLE16(rwops, 1); // wFormatTag
      SDL_WriteLE16(rwops, 1); // wChannels
      SDL_WriteLE32(rwops, m_sampleRate); // dwSamplesPerSec
      SDL_WriteLE32(rwops, m_sampleRate * sizeof(Sint16)); // dwAvgBytesPerSec = dwSamplesPerSec * wBlockAlign
      SDL_WriteLE16(rwops, sizeof(Sint16)); // wBlockAlign = wChannels * (wBitsPerSample / 8)
      SDL_WriteLE16(rwops, sizeof(Sint16) * 8); // wBitsPerSample

      // data chunk: 8 + numsamples*sizeof(Sint16) bytes
      SDL_RWwrite(rwops, "data", 4, 1);
      unsigned int size = m_audioSample.size();
      SDL_WriteLE32(rwops, size * sizeof(Sint16)); // size

      for (unsigned int index = 0; index < size; index++)
         SDL_WriteLE16(rwops, m_audioSample[index]);
   }

} // namespace Detail

using Audio::VoiceFile;

/// SDL_RWops ptr is automatically closed after loading
VoiceFile::VoiceFile(Base::SDL_RWopsPtr rwops)
{
   Detail::VoiceFileLoader loader(rwops);

   loader.Load();
   loader.ResampleAudio();
   loader.GenerateWaveFile(m_fileData);
}

Base::SDL_RWopsPtr VoiceFile::GetFileData() const
{
   return Base::MakeRWopsPtr(SDL_RWFromConstMem(&m_fileData[0], m_fileData.size()));
}
