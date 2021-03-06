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
/// \file Uw2decode.hpp
/// \brief Ultima Underworld 2 .ark compression format decoder
//
#pragma once

#include "File.hpp"

namespace Base
{
   /// \brief decodes uw2 data block from .ark file and returns file with content
   Base::File Uw2Decode(const Base::File& file, bool isCompressed, Uint32 dataSize);

} // namespace Base
