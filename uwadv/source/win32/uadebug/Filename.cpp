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
/// \file Filename.cpp
/// \brief filename management
//
#include "pch.hpp"
#include "Filename.hpp"

CFilename::CFilename(LPCTSTR filename)
{
   Set(filename);
}

void CFilename::Set(LPCTSTR filename)
{
   m_filename = filename;

   // replace unix-style slashes
   m_filename.Replace(_T("/"), _T("\\"));

   // add trailing backslash, if it's a folder
   if (m_filename.GetLength() > 0 && (m_filename.Right(1) != _T('\\')) && IsFolder())
      m_filename += _T('\\');

   // canonicalize path, if not relative
   if (!IsRelativePath())
   {
      CString temp(m_filename), canonical;

      BOOL ret = ::PathCanonicalize(canonical.GetBuffer(MAX_PATH), temp);
      canonical.ReleaseBuffer();
      if (ret == TRUE)
         m_filename = canonical;
   }
}

bool CFilename::IsValidObject() const
{
   return ::GetFileAttributes(m_filename) != 0xFFFFFFFF;
}

bool CFilename::IsFolder() const
{
   DWORD attributes = ::GetFileAttributes(m_filename);
   return attributes != 0xFFFFFFFF && (attributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
}

bool CFilename::IsFile() const
{
   DWORD attributes = ::GetFileAttributes(m_filename);
   return attributes != 0xFFFFFFFF && (attributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY;
}

bool CFilename::IsRelativePath() const
{
   CString pathname(GetPathname());
   if (pathname.IsEmpty())
      return true; // empty paths are relative to current directory

   // on windows, paths are absolute if they start with "C:" or similar
   if (pathname.GetLength() > 2 && pathname.GetAt(1) == _T(':'))
      return false;

   // if it starts with a slash, the path is absolute to the current drive
   if (pathname.GetLength() > 1 && pathname.GetAt(0) == _T('\\'))
      return false;

   // in any other case the path is relative
   return true;
}

CString CFilename::GetPathname() const
{
   int pos = m_filename.ReverseFind(_T('\\'));
   if (pos == -1)
      return CString();
   else
      return m_filename.Left(pos + 1);
}

CString CFilename::GetFilename() const
{
   int pos = m_filename.ReverseFind(_T('\\'));
   return m_filename.Mid(pos + 1);
}

void CFilename::MakeAbsolute(LPCTSTR baseDirectory)
{
   if (IsRelativePath())
   {
      CFilename fileBase(baseDirectory);
      ATLASSERT(false == fileBase.IsRelativePath()); // base directory must be absolute!

      CString temp = fileBase.GetPathname();
      temp += m_filename;

      CString newFilename;
      BOOL ret = ::PathCanonicalize(newFilename.GetBuffer(MAX_PATH), temp);
      newFilename.ReleaseBuffer();

      if (ret == TRUE)
         m_filename = newFilename;
   }
}

void CFilename::MakeAbsoluteToCurrentDir()
{
   if (!IsRelativePath())
      return;

   CString currentDir;
   ::GetCurrentDirectory(MAX_PATH, currentDir.GetBuffer(MAX_PATH));
   currentDir.ReleaseBuffer();

   MakeAbsolute(currentDir);
}

bool CFilename::MakeRelativeTo(LPCTSTR pathname)
{
   if (IsRelativePath())
      return false; // only absolute paths may be made relative to each other

   CFilename fileRelativeTo(pathname);

   CString newPath;
   BOOL ret = ::PathRelativePathTo(newPath.GetBuffer(MAX_PATH),
      fileRelativeTo.GetPathname(),
      FILE_ATTRIBUTE_DIRECTORY
      ,
      m_filename,
      IsFolder() ? FILE_ATTRIBUTE_DIRECTORY : 0
   );
   newPath.ReleaseBuffer();

   if (ret == TRUE)
   {
      if (newPath.GetLength() > 0 && newPath.GetAt(0) == _T('\\'))
         m_filename = newPath.Mid(1);
      else
         m_filename = newPath;
   }

   return ret == TRUE;
}
