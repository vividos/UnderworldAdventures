//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2014,2019,2021 Underworld Adventures Team
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
/// \file Decompiler.hpp
/// \brief Underworld conversation code decompiler
//
#pragma once

#include "Base.hpp"

class GameStrings;

namespace Conv
{
   class CodeGraph;

   /// uw conversation code decompiler
   class Decompiler
   {
   public:
      /// ctor
      Decompiler(Uint16 conversationNumber, std::string basePath, GameStrings& strings);

      /// returns name of conversation partner
      std::string GetName() const;

      /// writes out decompiled code
      void Write(FILE* fd, bool showDisassembly);

   private:
      /// conversation number
      Uint16 m_conversationNumber;

      /// game strings
      GameStrings& m_strings;

      /// code graph
      std::shared_ptr<CodeGraph> m_codeGraph;
   };

} // namespace Conv
