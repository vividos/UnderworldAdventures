//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2020 Underworld Adventures Team
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
/// \file .hpp
/// \brief lexical analyzer class
///
/// defines a class for lexical analyzing a vrml 97 file (extension .wrl). the
/// function yylex() tokenizes input read from a SDL_RWops source and returns
/// when a specific token is read.
//
#pragma once

#include "FlexLexer.h"
#include <string>
#include <SDL2/SDL.h>

// needed return value for LexerInput()
#ifndef YY_NULL
#define YY_NULL 0
#endif

/// <summary>
/// WRL lexer token type
/// </summary>
enum WrlTokenType
{
   tokenBracketOpen = 1,
   tokenBracketClose,
   tokenInt,
   tokenFloat,
   tokenString,
   tokenKeyword,
   tokenInvalid
};

/// lexer class for .wrl files
class WrlLexer : public yyFlexLexer
{
public:
   /// ctor
   WrlLexer(SDL_RWops* rwops)
      :m_rwops(rwops)
   {
   }

   /// analyzes input and returns token type (or 0 when finished)
   int yylex() override;

   /// provides more input for lexer
   virtual int LexerInput(char* buffer, int maxSize) override
   {
      size_t numBytesRead = SDL_RWread(m_rwops, buffer, 1, maxSize);
      return numBytesRead == 0 ? YY_NULL : static_cast<int>(numBytesRead);
   }

   /// returns read integer value
   int GetInt() const { return m_int; }

   /// returns read float value
   double GetFloat() const { return m_double; }

   /// returns read string/keyword value
   const std::string& GetString() const { return m_string; }

private:
   /// source to read from
   SDL_RWops* m_rwops;

   double m_double;        ///< read double value
   int m_int;              ///< read integer value
   std::string m_string;   ///< read string value
};
