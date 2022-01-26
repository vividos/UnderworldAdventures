//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2022 Underworld Adventures Team
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
/// \file Path.hpp
/// \brief Path class
//
#pragma once

namespace Base
{
   /// path handling methods, inspired by .NET System.IO.Path class
   class Path
   {
   public:
      /// returns the directory part of the path, including an ending slash
      static std::string Directory(const std::string& path);

      /// returns the file name part of the path, including file extension
      static std::string FilenameAndExt(const std::string& path);

      /// returns the file name part of the path, excluding file extension
      static std::string FilenameOnly(const std::string& path);

      /// returns the file extension of the filename, including a starting dot
      static std::string Extension(const std::string& path);

      /// ensures that the folder path ends with a slash
      static void EnsureEndingSlash(std::string& path);

      /// combines two path parts
      static std::string Combine(const std::string& part1,
         const std::string& part2);

      /// combines three path parts
      static std::string Combine(const std::string& part1,
         const std::string& part2, const std::string& part3);
   };

} // namespace Base
