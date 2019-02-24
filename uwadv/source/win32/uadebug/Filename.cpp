//
// Underworld Adventures Debugger - a debugger tool for Underworld Adventures
// Copyright (c) 2005,2019 Michael Fink
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
#include "stdatl.hpp"
#include "Filename.hpp"

CFilename::CFilename(LPCTSTR pszFilename)
{
   Set(pszFilename);
}

void CFilename::Set(LPCTSTR pszFilename)
{
   m_cszFilename = pszFilename;

   // replace unix-style slashes
   m_cszFilename.Replace(_T("/"), _T("\\"));

   // add trailing backslash, if it's a folder
   if (m_cszFilename.GetLength() > 0 && (m_cszFilename.Right(1) != _T('\\')) && IsFolder())
      m_cszFilename += _T('\\');

   // canonicalize path, if not relative
   if (!IsRelativePath())
   {
      CString cszTemp(m_cszFilename), cszCanonical;

      BOOL bRet = ::PathCanonicalize(cszCanonical.GetBuffer(MAX_PATH), cszTemp);
      cszCanonical.ReleaseBuffer();
      if (bRet == TRUE)
         m_cszFilename = cszCanonical;
   }
}

bool CFilename::IsValidObject() const
{
   return ::GetFileAttributes(m_cszFilename) != 0xFFFFFFFF;
}

bool CFilename::IsFolder() const
{
   DWORD nAttr = ::GetFileAttributes(m_cszFilename);
   return nAttr != 0xFFFFFFFF && (nAttr & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
}

bool CFilename::IsFile() const
{
   DWORD nAttr = ::GetFileAttributes(m_cszFilename);
   return nAttr != 0xFFFFFFFF && (nAttr & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY;
}

bool CFilename::IsRelativePath() const
{
   CString cszPathname(GetPathname());
   if (cszPathname.IsEmpty())
      return true; // empty paths are relative to current directory

   // on windows, paths are absolute if they start with "C:" or similar
   if (cszPathname.GetLength() > 2 && cszPathname.GetAt(1) == _T(':'))
      return false;

   // if it starts with a slash, the path is absolute to the current drive
   if (cszPathname.GetLength() > 1 && cszPathname.GetAt(0) == _T('\\'))
      return false;

   // in any other case the path is relative
   return true;
}

CString CFilename::GetPathname() const
{
   int nPos = m_cszFilename.ReverseFind(_T('\\'));
   if (nPos == -1)
      return CString();
   else
      return m_cszFilename.Left(nPos + 1);
}

CString CFilename::GetFilename() const
{
   int nPos = m_cszFilename.ReverseFind(_T('\\'));
   return m_cszFilename.Mid(nPos + 1);
}

void CFilename::MakeAbsolute(LPCTSTR pszBaseDirectory)
{
   if (IsRelativePath())
   {
      CFilename fileBase(pszBaseDirectory);
      ATLASSERT(false == fileBase.IsRelativePath()); // base directory must be absolute!

      CString cszTemp = fileBase.GetPathname();
      cszTemp += m_cszFilename;

      CString cszNewFilename;
      BOOL bRet = ::PathCanonicalize(cszNewFilename.GetBuffer(MAX_PATH), cszTemp);
      cszNewFilename.ReleaseBuffer();

      if (bRet == TRUE)
         m_cszFilename = cszNewFilename;
   }
}

void CFilename::MakeAbsoluteToCurrentDir()
{
   if (!IsRelativePath())
      return;

   CString cszCurrentDir;
   ::GetCurrentDirectory(MAX_PATH, cszCurrentDir.GetBuffer(MAX_PATH));
   cszCurrentDir.ReleaseBuffer();

   MakeAbsolute(cszCurrentDir);
}

bool CFilename::MakeRelativeTo(LPCTSTR pszPathname)
{
   if (IsRelativePath())
      return false; // only absolute paths may be made relative to each other

   CFilename fileRelativeTo(pszPathname);

   CString cszNewPath;
   BOOL bRet = ::PathRelativePathTo(cszNewPath.GetBuffer(MAX_PATH),
      fileRelativeTo.GetPathname(),
      FILE_ATTRIBUTE_DIRECTORY
      ,
      m_cszFilename,
      IsFolder() ? FILE_ATTRIBUTE_DIRECTORY : 0
   );
   cszNewPath.ReleaseBuffer();

   if (bRet == TRUE)
   {
      if (cszNewPath.GetLength() > 0 && cszNewPath.GetAt(0) == _T('\\'))
         m_cszFilename = cszNewPath.Mid(1);
      else
         m_cszFilename = cszNewPath;
   }

   return bRet == TRUE;
}
