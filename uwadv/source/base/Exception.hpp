//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2006,2019 Michael Fink
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
/// \file Exception.hpp
/// \brief Exception classes
//
#pragma once

#include <stdexcept>
#include <string>

namespace Base
{
   /// \brief Exception class
   /// Base class for all exception classes. An exception should be thrown when an
   /// operation cannot be completed successfully. Return types for functions are
   /// discouraged and only used for IsXYZ functions. The exception can be given an
   /// informational text about the exception.
   class Exception : public std::runtime_error
   {
   public:
      /// ctor
      Exception(const std::string& description)
         :std::runtime_error(description.c_str())
      {
      }

      /// dtor
      ~Exception() throw() {}
   };

   /// \brief Exception that is thrown for file system errors
   /// Can be thrown from all functions in Base::FileSystem, but also from
   /// functions and classes that deal with files, e.g. Base::File. The
   /// exception also takes a filename for the file or folder where the
   /// exception occured, and an OS specific error code.
   class FileSystemException : public Exception
   {
   public:
      /// ctor
      FileSystemException(const std::string& description, const std::string& filename, unsigned int osErrorCode)
         : Exception(description),
         m_filename(filename),
         m_osErrorCode(osErrorCode)
      {
      }

      /// dtor
      ~FileSystemException() throw() {}

      /// returns filename of the file that was involved when the exception was thrown
      std::string GetFilename() const { return m_filename; }

      /// returns OS specific error code for the file system error
      unsigned int GetOSErrorCode() const { return m_osErrorCode; }

   protected:
      /// filename involved with the exception
      std::string m_filename;

      /// OS specific error code
      unsigned int m_osErrorCode;
   };

   /// exception that is thrown for general runtime errors
   class RuntimeException : public Exception
   {
   public:
      /// ctor
      RuntimeException(const std::string& description)
         :Exception(description)
      {
      }
   };

} // namespace Base
