/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

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
/*! \file wrllexer.hpp

   \brief lexical analyzer class

   defines a class for lexical analyzing a vrml 97 file (extension .wrl). the
   function yylex() tokenizes input read from a SDL_RWops source and returns
   when a specific token is read.

*/

// include guard
#ifndef wrllexer_hpp_
#define wrllexer_hpp_

// needed includes
#include "FlexLexer.h"
#include <string>
#include <SDL.h>


// needed return value for LexerInput()
#ifndef YY_NULL
#define YY_NULL 0
#endif


// enums

enum ua_wrl_token_type
{
   ua_wt_bracket_open=1,
   ua_wt_bracket_close,
   ua_wt_int,
   ua_wt_float,
   ua_wt_string,
   ua_wt_keyword,
   ua_wt_invalid
};


// classes

//! lexer class for .wrl files
class ua_wrl_lexer: public yyFlexLexer
{
public:
   //! ctor
   ua_wrl_lexer(SDL_RWops* therwops):rwops(therwops){}

   //! analyzes input and returns token type (or 0 when finished)
   int yylex();

   //! provides more input for lexer
   virtual int LexerInput( char* buf, int max_size )
   {
      int read = SDL_RWread(rwops,buf,1,max_size);
      return read==0? YY_NULL : read;
   }

   //! returns read integer value
   int get_int() const { return i; }

   //! returns read float value
   double get_float() const { return d; }

   //! returns read string/keyword value
   const std::string& get_string() const { return str; }

protected:
   //! source to read from
   SDL_RWops* rwops;

   // read values
   double d;
   int i;
   std::string str;
};


#endif
