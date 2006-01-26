/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2002,2003,2004,2005,2006 Michael Fink

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
/*! \file cfgfile.cpp

   \brief config file loading implementation

*/

// needed includes
#include <cerrno>
#include "base.hpp"
#include "cfgfile.hpp"
#include "textfile.hpp"

//! \brief namespace for implementation details
namespace Detail
{

//! config file loader class
class ConfigFileLoader: public Base::NonCopyable
{
public:
   //! ctor without saving file
   ConfigFileLoader():m_bIsWriting(false){}

   //! ctor with saving file
   ConfigFileLoader(const std::string strOutputFilename)
      :m_bIsWriting(true), m_outputFile(strOutputFilename, Base::modeWrite){}

   //! loads config file (and rewrites it, when m_bIsWriting is on)
   void LoadFile(Base::TextFile& file, Base::ConfigValueMap& mapValues);

   //! returns if config file is open
   bool IsOutputFileOpen() const { return m_outputFile.IsOpen(); }

private:
   //! indicates if config file is being rewritten
   bool m_bIsWriting;

   //! output text file when config file is rewritten
   Base::TextFile m_outputFile;
};

void ConfigFileLoader::LoadFile(Base::TextFile& file, Base::ConfigValueMap& mapValues)
{
   // find out filelength
   long lFileLen = file.FileLength();

   // read in all strLines
   std::string strLine;

   while (file.Tell() < lFileLen)
   {
      file.ReadLine(strLine);

      // empty line?
      if (strLine.size() == 0)
      {
         if (m_bIsWriting)
            m_outputFile.WriteLine(strLine.c_str());
         continue;
      }

      // trim spaces at start of line
      for(;strLine.size() > 0 && isspace(strLine.at(0));)
         strLine.erase(0, 1);

      // comment line?
      if (strLine.size() == 0 || strLine.at(0) == '#' || strLine.at(0) == ';')
      {
         if (m_bIsWriting)
            m_outputFile.WriteLine(strLine.c_str());
         continue;
      }

      // comment somewhere in the line?
      std::string::size_type pos2 = strLine.find('#');
      if (pos2 != std::string::npos)
      {
         // write comment before line
         if (m_bIsWriting)
         {
            std::string comment;
            comment.assign(strLine.c_str() + pos2);
            m_outputFile.WriteLine(comment.c_str());
         }

         // remove comment
         strLine.erase(pos2);
      }

      // trim spaces at end of line
      {
         int len;
         do
         {
            len = strLine.size()-1;
            if (isspace(strLine.at(len)))
               strLine.erase(len);
            else
               break;
         }
         while (strLine.size() > 0);
      }

      // empty line?
      if (strLine.size() == 0)
      {
         if (m_bIsWriting)
            m_outputFile.WriteLine(strLine.c_str());
         continue;
      }

      // replace all '\t' with ' '
      std::string::size_type pos = 0;
      while( (pos = strLine.find('\t',pos)) != std::string::npos )
         strLine.replace(pos, 1, " ");

      // there must be at least one space, to separate key from value
      pos = strLine.find(' ');
      if (pos==std::string::npos)
      {
         if (m_bIsWriting)
            m_outputFile.WriteLine(strLine.c_str());
         continue;
      }

      // retrieve key and value
      {
         std::string strKey(strLine.substr(0, pos));
         std::string strValue(strLine.substr(pos+1));

         // trim spaces at start of "value"
         for(;strValue.size() > 0 && isspace(strValue.at(0));)
            strValue.erase(0,1);

         // hand over key and value
         if (m_bIsWriting)
         {
            Base::ConfigValueMap::iterator pos = mapValues.find(strKey);

            if (pos != mapValues.end())
               strValue = pos->second;

            m_outputFile.WriteLine(strKey + ' ' + strValue);
         }
         else
            mapValues[strKey] = strValue;
      }
   }
}

} // namespace Detail


// ConfigFile methods

//! common error message when a config file couldn't be opened
const char* c_strFailedOpenConfigFile = "could not open config file";

using Base::ConfigFile;

void ConfigFile::Load(const std::string& strFilename)
{
   // try to open from file
   Base::TextFile file(strFilename, Base::modeRead);
   if (!file.IsOpen())
      throw Base::FileSystemException(c_strFailedOpenConfigFile, strFilename, ENOENT);

   Load(file);
}

void ConfigFile::Load(Base::TextFile& file)
{
   // load via config file loader
   Detail::ConfigFileLoader loader;
   loader.LoadFile(file, m_mapValues);
}

void ConfigFile::Save(const std::string& strOriginalFilename, const std::string& strNewFilename)
{
   // open original file
   Base::TextFile origFile(strOriginalFilename, Base::modeRead);
   if (!origFile.IsOpen())
      throw Base::FileSystemException(c_strFailedOpenConfigFile, strOriginalFilename, ENOENT);

   // load and rewrite via config file loader
   Detail::ConfigFileLoader loader(strNewFilename);

   if (!loader.IsOutputFileOpen())
      throw Base::FileSystemException("could not rewrite new config file", strNewFilename, ENOENT);

   loader.LoadFile(origFile, m_mapValues);
}
