//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Underworld Adventures Team
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
/// \file ConfigFile.cpp
/// \brief config file loading implementation
//
#include "pch.hpp"
#include <cerrno>
#include "ConfigFile.hpp"
#include "TextFile.hpp"

/// \brief namespace for implementation details
namespace Detail
{
   /// config file loader class
   class ConfigFileLoader
   {
   public:
      /// ctor without saving file
      ConfigFileLoader()
         :m_isWriting(false)
      {
      }

      /// ctor with saving file
      ConfigFileLoader(const std::string outputFilename)
         :m_isWriting(true), m_outputFile(outputFilename, Base::modeWrite) {}

      /// loads config file (and rewrites it, when m_isWriting is on)
      void LoadFile(Base::TextFile& file, Base::ConfigValueMap& mapValues);

      /// returns if config file is open
      bool IsOutputFileOpen() const { return m_outputFile.IsOpen(); }

   private:
      /// deleted copy ctor
      ConfigFileLoader(const ConfigFileLoader&) = delete;

      /// deleted assignment operator
      ConfigFileLoader& operator=(const ConfigFileLoader&) = delete;

   private:
      /// indicates if config file is being rewritten
      bool m_isWriting;

      /// output text file when config file is rewritten
      Base::TextFile m_outputFile;
   };

   void ConfigFileLoader::LoadFile(Base::TextFile& file, Base::ConfigValueMap& mapValues)
   {
      long fileLength = file.FileLength();

      std::string line;

      while (file.Tell() < fileLength)
      {
         file.ReadLine(line);

         if (line.empty())
         {
            if (m_isWriting)
               m_outputFile.WriteLine(line.c_str());
            continue;
         }

         Base::String::TrimStart(line);

         // comment line?
         if (line.empty() || line[0] == '#' || line[0] == ';')
         {
            if (m_isWriting)
               m_outputFile.WriteLine(line.c_str());
            continue;
         }

         // comment somewhere in the line?
         std::string::size_type pos2 = line.find('#');
         if (pos2 != std::string::npos)
         {
            // write comment before line
            if (m_isWriting)
            {
               std::string comment;
               comment.assign(line.c_str() + pos2);
               m_outputFile.WriteLine(comment.c_str());
            }

            // remove comment
            line.erase(pos2);
         }

         Base::String::TrimEnd(line);

         // empty line?
         if (line.empty())
         {
            if (m_isWriting)
               m_outputFile.WriteLine(line.c_str());
            continue;
         }

         Base::String::Replace(line, "\t", " ");

         // there must be at least one space, to separate key from value
         std::string::size_type pos = line.find(' ');
         if (pos == std::string::npos)
         {
            if (m_isWriting)
               m_outputFile.WriteLine(line.c_str());
            continue;
         }

         // retrieve key and value
         {
            std::string key(line.substr(0, pos));
            std::string value(line.substr(pos + 1));

            Base::String::TrimStart(value);

            // hand over key and value
            if (m_isWriting)
            {
               Base::ConfigValueMap::iterator pos3 = mapValues.find(key);

               if (pos3 != mapValues.end())
                  value = pos3->second;

               m_outputFile.WriteLine(key + ' ' + value);
            }
            else
               mapValues[key] = value;
         }
      }
   }

} // namespace Detail

/// common error message when a config file couldn't be opened
const char* c_failedOpenConfigFile = "could not open config file";

using Base::ConfigFile;

void ConfigFile::Load(const std::string& filename)
{
   Base::TextFile file(filename, Base::modeRead);
   if (!file.IsOpen())
      throw Base::FileSystemException(c_failedOpenConfigFile, filename, ENOENT);

   Load(file);
}

void ConfigFile::Load(Base::TextFile& file)
{
   Detail::ConfigFileLoader loader;
   loader.LoadFile(file, m_mapValues);
}

void ConfigFile::Save(const std::string& originalFilename, const std::string& newFilename)
{
   // open original file
   Base::TextFile origFile(originalFilename, Base::modeRead);
   if (!origFile.IsOpen())
      throw Base::FileSystemException(c_failedOpenConfigFile, originalFilename, ENOENT);

   // load and rewrite via config file loader
   Detail::ConfigFileLoader loader(newFilename);

   if (!loader.IsOutputFileOpen())
      throw Base::FileSystemException("could not rewrite new config file", newFilename, ENOENT);

   loader.LoadFile(origFile, m_mapValues);
}
