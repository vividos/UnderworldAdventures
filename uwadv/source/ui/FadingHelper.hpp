//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2003,2019 Underworld Adventures Team
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
/// \file FadingHelper.hpp
/// \brief fading class
/// \details The fading helper class FadingHelper helps fading in or out screens,
/// e.g. when one leaves an active screen. The class manages a value in the
/// range 0 to 255 that can be used in a call to glColor3ub(). The init()
/// method can be called multiple times. The Tick() method (should be called
/// every tick) returns true if the fading action is at its end. The fading
/// value stays at it's value (either 0 or 255) after finishing fading.
//
#pragma once

/// helper class for fading in/out screens
class FadingHelper
{
public:
   /// ctor
   FadingHelper()
      :m_fadeIn(false), m_isFinished(true), m_tickRate(20.0),
      m_fadeTime(0.0), m_tickCount(0)
   {
   }

   /// (re)initialize fading helper
   void Init(bool fadeIn, double tickRate, double fadeTime,
      unsigned int startTickCount = 0)
   {
      m_fadeIn = fadeIn;
      m_tickRate = tickRate;
      m_fadeTime = fadeTime;
      m_tickCount = startTickCount;
      m_isFinished = false;
   }

   /// does a tick and returns true if the fading action is finished
   bool Tick()
   {
      if (!m_isFinished)
         m_tickCount++;

      return m_isFinished = m_tickCount >= m_fadeTime * m_tickRate;
   }

   /// returns fading color value [0..255]
   Uint8 GetFadeValue() const
   {
      if (m_isFinished)
         return m_fadeIn ? 255 : 0;

      return m_fadeIn ? Uint8(255 * (double(m_tickCount) / (m_fadeTime * m_tickRate))) :
         Uint8(255 - 255 * (double(m_tickCount) / (m_fadeTime * m_tickRate)));
   }

protected:
   /// indicates if a fade-in is done
   bool m_fadeIn;
   /// indicates if fading is finished
   bool m_isFinished;

   /// ticks per second
   double m_tickRate;
   /// time the fadein/out should take
   double m_fadeTime;
   /// tick count
   unsigned int m_tickCount;
};
