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
/*! \file vocfile.cpp

   \brief .voc file implementation

*/

// needed includes
#include "audio.hpp"
#include "vocfile.hpp"
#include <SDL_endian.h>

namespace Detail
{

void ResampleChunk12048_22050(const std::vector<double>& vecSourceSamples,
   std::vector<double>& vecDestSamples);

//! voc file loader
/*! Loads Creative Voice file (.voc) and resamples the audio data if
    necessary. The class can cope with 12048 Hz .voc files usually found in
    uw1, and does the following audio rate conversion:
    * 12048 Hz -> 44100 Hz (true resampling)
    * 11111 Hz -> 11025 Hz (just remapping so that SDL audio can load the file

    The reason for this class is because SDL_mixer's .voc loader does a bad
    job of loading a .voc file. No integral sample rates can be specified in
    the file, and that's why the strange sample rates above happen. We just
    satisfy SDL here by providing a fake .wav file that contains proper sample
    rates and properly converted samples.
*/
class VoiceFileLoader
{
public:
   //! ctor; SDL_RWops ptr is freed automatically after loading
   VoiceFileLoader(SDL_RWops* rwops):m_rwops(rwops){}

   //! loads voc file
   void Load();

   //! resamples audio if neccessary
   void ResampleAudio();

   //! generate in-memory .wav file
   void GenerateWaveFile(std::vector<Uint8>& vecWaveFile);

private:
   //! ptr to read .voc file from
   SDL_RWops* m_rwops;

   //! samplerate of .voc file
   Uint32 m_uiSamplerate;

   //! audio samples
   std::vector<Sint16> m_vecSamples;
};

/*!
The .voc file format is documented in:
http://icculus.org/SDL_sound/downloads/external_documentation/Voc.txt
*/
void VoiceFileLoader::Load()
{
   const char* c_cstrHeaderString = "Creative Voice File\x1a";

   // read in header
   char cstrHeaderString[0x13+1];
   SDL_RWread(m_rwops, cstrHeaderString, 1, SDL_TABLESIZE(cstrHeaderString));
   UaAssert(0 == strncmp(cstrHeaderString, c_cstrHeaderString, SDL_TABLESIZE(cstrHeaderString)));

   Uint16 uiOffsetDataBlock = SDL_ReadLE16(m_rwops);
   UaAssert(uiOffsetDataBlock == 0x001a);

   Uint16 uiVersionNumber = SDL_ReadLE16(m_rwops);
   UaAssert(uiVersionNumber == 0x010a); // assume version 1.10 always

   Uint16 uiVersionNumber1sComplement = SDL_ReadLE16(m_rwops);
   UaAssert(uiVersionNumber1sComplement == 0x1129);

   // read in first data block
   Uint8 uiBlockType = 0;

   // read in block type; must be 1
   SDL_RWread(m_rwops, &uiBlockType, 1, 1);
   UaAssert(uiBlockType == 1);

   // read in size
   Uint32 uiSize = static_cast<Uint32>(SDL_ReadLE16(m_rwops));

   Uint8 uiSizeHigh;
   SDL_RWread(m_rwops, &uiSizeHigh, 1, 1);
   uiSize |= static_cast<Uint32>(uiSizeHigh) << 16;

   // read samplerate
   Uint8 uiCodedSamplerate = 0;
   SDL_RWread(m_rwops, &uiCodedSamplerate, 1, 1);

   // note: this formula comes from the above URL, but it seems to be wrong
   // for uw1 and uw2 files (or it's just flawed)
   // uw1 .voc files give 0xAD as coded sample rate, which represents 12048 Hz
   // uw2 .voc files give 0xA5, which represents 11111 Hz 
   m_uiSamplerate = 1000000 / (256-uiCodedSamplerate);

   Uint8 uiCompressionType = 0;
   SDL_RWread(m_rwops, &uiCompressionType, 1, 1);
   UaAssert(uiCompressionType == 0); // 8-bit

   // read in 8-bit samples
   std::vector<Uint8> vecRawSamples(uiSize);
   SDL_RWread(m_rwops, &vecRawSamples[0], 1, uiSize);

   // convert to signed 16-bit
   m_vecSamples.resize(uiSize);
   for (unsigned int ui=0; ui<uiSize; ui++)
      m_vecSamples[ui] = (Sint16(vecRawSamples[ui])-128)<<8;
}

void VoiceFileLoader::ResampleAudio()
{
   // SDL_mixer calculates the samplerate wrongly, so fix this for 11025 Hz
   // .voc files
   if (m_uiSamplerate == 11111)
      m_uiSamplerate = 11025;

   // check if we need to convert the file
   if (m_uiSamplerate != 12048)
      return;

   // convert samples to double
   // audio length is doubled by interleaving samples with zeros
   // this is the same as upsampling with factor 2
   // the mirrors created in the spectrum by this method are filtered out
   // afterwards when doing resampling from 12048 Hz to 22050 Hz
   std::vector<double> vecSourceSamples, vecDestSamples;

   unsigned int uiSize = m_vecSamples.size();
   vecSourceSamples.resize(uiSize*2, 0);

   for (unsigned int ui=0; ui<uiSize; ui++)
      vecSourceSamples[ui*2] = static_cast<double>(m_vecSamples[ui]);

   ResampleChunk12048_22050(vecSourceSamples, vecDestSamples);

   // convert back to signed 16-bit values
   // note: the last 7 samples are not used, since they might contain erroneous
   // sample from resampling
   uiSize = vecDestSamples.size();
   m_vecSamples.resize(uiSize);

   for (unsigned int ui2=0; ui2<uiSize-7; ui2++)
      m_vecSamples[ui2] = static_cast<Sint16>(vecDestSamples[ui2]);

   m_uiSamplerate = 44100;
}

/*! Produces a .wav file in memory with the audio samples read from .voc file.
    The wave file format is described here:
    http://www.borg.com/~jglatt/tech/wave.htm
*/
void VoiceFileLoader::GenerateWaveFile(std::vector<Uint8>& vecWaveFile)
{
   // prepare data bytes array for wave file
   unsigned int uiFileSize = 12+24+8 + m_vecSamples.size()*sizeof(Sint16);
   vecWaveFile.resize(uiFileSize);

   SDL_RWops* rwops = ::SDL_RWFromMem(&vecWaveFile[0], vecWaveFile.size());

   // header: 12 bytes
   SDL_RWwrite(rwops, "RIFF", 4, 1);
   SDL_WriteLE32(rwops, uiFileSize-8);
   SDL_RWwrite(rwops, "WAVE", 4, 1);

   // format chunk: 24 bytes
   SDL_RWwrite(rwops, "fmt ", 4, 1);
   SDL_WriteLE32(rwops, 16);
   SDL_WriteLE16(rwops, 1); // wFormatTag
   SDL_WriteLE16(rwops, 1); // wChannels
   SDL_WriteLE32(rwops, m_uiSamplerate); // dwSamplesPerSec
   SDL_WriteLE32(rwops, m_uiSamplerate*sizeof(Sint16)); // dwAvgBytesPerSec = dwSamplesPerSec * wBlockAlign
   SDL_WriteLE16(rwops, sizeof(Sint16)); // wBlockAlign = wChannels * (wBitsPerSample / 8)
   SDL_WriteLE16(rwops, sizeof(Sint16)*8); // wBitsPerSample

   // data chunk: 8 + numsamples*sizeof(Sint16) bytes
   SDL_RWwrite(rwops, "data", 4, 1);
   unsigned int uiSize = m_vecSamples.size();
   SDL_WriteLE32(rwops, uiSize*sizeof(Sint16)); // size

   for(unsigned int ui=0; ui<uiSize; ui++)
      SDL_WriteLE16(rwops, m_vecSamples[ui]);
}

} // namespace Detail

using Audio::VoiceFile;

// Playlist methods

/*! SDL_RWops ptr is automatically closed after loading */
VoiceFile::VoiceFile(SDL_RWops* rwops)
{
   Detail::VoiceFileLoader loader(rwops);

   loader.Load();
   loader.ResampleAudio();
   loader.GenerateWaveFile(m_vecFileData);

   SDL_RWclose(rwops);
}

/*! user has to free SDL_RWops ptr returned. */
SDL_RWops* VoiceFile::GetFileData() const
{
   return SDL_RWFromConstMem(&m_vecFileData[0], m_vecFileData.size());
}
