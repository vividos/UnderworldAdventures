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
/*
   when updating the wrllexer.cpp file from this flex input file, be sure to
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
%option outfile="wrllexer.cpp"
%option yyclass="ua_wrl_lexer"

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

#include "resource/model/wrllexer.hpp"

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
[\[\{\(]                return ua_wt_bracket_open;
[\]\}\)]                return ua_wt_bracket_close;

   /* float values */
[+-]?(0|([1-9][0-9]*))\.[0-9]*      d = strtod(yytext,NULL); return ua_wt_float;

   /* integer constant */
[+-]?(0|([1-9][0-9]*))              i = strtol(yytext,NULL,10); return ua_wt_int;

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
DEF                     str.assign(yytext,yyleng); return ua_wt_keyword;

   /* string constant */
\"[a-zA-Z\-\.\/]+\"     |
[a-zA-Z\-]+             str.assign(yytext,yyleng); return ua_wt_string;

   /* other characters */
.                       return ua_wt_invalid;

%%
