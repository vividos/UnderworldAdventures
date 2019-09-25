//
// Underworld Adventures Debugger - a debugger tool for Underworld Adventures
// Copyright (c) 2005,2019 Underworld Adventures Team
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
/// \file Filename.hpp
/// \brief filename management
//
#pragma once

/// manages complete path/filenames
class CFilename
{
public:
   /// empty ctor
   CFilename() {}
   /// ctor
   CFilename(LPCTSTR filename);

   /// sets path/filename
   void Set(LPCTSTR filename);

   /// returns path/filename
   CString Get() const { return m_filename; }

   /// returns if given filename is a valid object, either a file or folder name that exists
   bool IsValidObject() const;

   /// returns if given filename is a folder name
   bool IsFolder() const;

   /// returns if given filename is a file name
   bool IsFile() const;

   /// returns if path is relative (true) or absolute (false)
   bool IsRelativePath() const;

   /// returns path part of path/filename
   CString GetPathname() const;

   /// returns filename part of path/filename
   CString GetFilename() const;

   /// makes path absolute; if it's a relative path, baseDirectory is the base directory
   void MakeAbsolute(LPCTSTR baseDirectory);

   /// makes path absolute to current directory
   void MakeAbsoluteToCurrentDir();

   /// makes path relative to given path; returns true when paths were made relative
   bool MakeRelativeTo(LPCTSTR pathname);

protected:
   /// path/filename to manage
   CString m_filename;
};
