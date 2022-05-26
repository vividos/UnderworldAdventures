//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2006,2019 Underworld Adventures Team
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
/// \file SDL_rwops_gzfile.h
/// \brief SDL_RWops wrapper for zlib's gzFile APIs
//
#pragma once

#include <SDL2/SDL_rwops.h>

#ifdef __cplusplus
extern "C"
{
#endif

   /// \brief returns SDL_RWops pointer to access gzip-compressed file stream (.gz)
   /// \param filename filename of the compressed file to open
   /// \param mode mode of file to open. This is passed to gzopen and can take the
   ///        same parameters, e.g. "wb9". See zlib documentation for more.
   extern SDL_RWops* SDL_RWFromGzFile(const char* filename, const char* mode);

#ifdef __cplusplus
} /* extern C */
#endif
