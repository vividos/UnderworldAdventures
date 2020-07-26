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
/*
   when updating the WrlLexer.cpp file from this flex input file, be sure to
   use the modified flex skeleton file flex.skl in this folder. the command
   to regenerate is:

     flex -Sflex.skl wrllexer.l

   before commiting the new file, be sure to change the line

      #include <FlexLexer.h>

   to look like

      #include "FlexLexer.h"

   so that compiles on mingw32 and linux don't need explicit include paths
   up to here.
*/

/* flex options */

%option c++
%option outfile="WrlLexer.cpp"
%option yyclass="WrlLexer"

%option never-interactive
%option nostdinit
%option noyywrap

%option nounput
%option noinput
%option noyy_push_state
%option noyy_pop_state
%option noyy_top_state
%option noyy_scan_buffer
%option noyy_scan_bytes
%option noyy_scan_string

/* prefix code */
%{

#include "import/vrml/WrlLexer.hpp"

%}


%%
   /* rules */


   /* kill # comments */
#.*$                    ;

   /* eat all whitespaces */
\r                      |
[ \t\n]+                ;

   /* eat all commas */
,                       ;

   /* brackets */
[\[\{\(]                return tokenBracket_open;
[\]\}\)]                return tokenBracket_close;

   /* float values */
[+-]?(0|([1-9][0-9]*))\.[0-9]*      m_double = strtod(yytext,NULL); return tokenFloat;

   /* integer constant */
[+-]?(0|([1-9][0-9]*))              m_int = strtol(yytext,NULL,10); return tokenInt;

   /* keywords */
material                |
texture                 |
coord                   |
texCoord                |
coordIndex              |
texCoordIndex           |
translation             |
rotation                |
scale                   |
DEF                     m_string.assign(yytext,yyleng); return tokenKeyword;

   /* string constant */
\"[a-zA-Z\-\.\/]+\"     |
[a-zA-Z\-]+             m_string.assign(yytext,yyleng); return tokenString;

   /* other characters */
.                       return tokenInvalid;

%%
