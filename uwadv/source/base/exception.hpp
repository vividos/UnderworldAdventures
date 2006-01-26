/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2006 Michael Fink

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
/*! \file exception.hpp

   \brief Exception classes

*/

// include guard
#ifndef uwadv_base_exceptions_hpp_
#define uwadv_base_exceptions_hpp_

// needed includes
#include <exception>
#include <string>

namespace Base
{

//! Exception class
/*! Base class for all exception classes. An exception should be thrown when an
    operation cannot be completed successfully. Return types for functions are
    discouraged and only used for IsXYZ functions. The exception can be given an
    informational text about the exception.
*/
class Exception: public std::exception
{
public:
   //! ctor
   Exception(const std::string& strDesc):m_strDesc(strDesc){}

   //! dtor
   ~Exception() throw(){}

   //! returns exception description
   virtual const char* what() const throw()
   {
      return m_strDesc.c_str();
   }

protected:
   //! exception description
   std::string m_strDesc;
};

//! Exception that is thrown for file system errors
/*! Can be thrown from all functions in Base::FileSystem, but also from
    functions and classes that deal with files, e.g. Base::File. The
    exception also takes a filename for the file or folder where the
    exception occured, and an OS specific error code. */
class FileSystemException: public Exception
{
public:
   //! ctor
   FileSystemException(const std::string& strDesc, const std::string& strFilename, unsigned int uiOSErrorCode)
      : Exception(strDesc), m_strFilename(strFilename), m_uiOSErrorCode(uiOSErrorCode)
   {
   }

   //! dtor
   ~FileSystemException() throw(){}

   //! returns filename of the file that was involved when the exception was thrown
   std::string GetFilename() const { return m_strFilename; }

   //! returns OS specific error code for the file system error
   unsigned int GetOSErrorCode() const { return m_uiOSErrorCode; }

protected:
   //! filename involved with the exception
   std::string m_strFilename;

   //! OS specific error code
   unsigned int m_uiOSErrorCode;
};

//! exception that is thrown for general runtime errors
class RuntimeException: public Exception
{
public:
   //! ctor
   RuntimeException(const std::string& desc):Exception(desc.c_str()){}
};

} // namespace Base

#endif
