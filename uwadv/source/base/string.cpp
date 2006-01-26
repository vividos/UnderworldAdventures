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
/*! \file string.cpp

   \brief string functions implementation

*/

// needed includes
#include "base.hpp"
#include "string.hpp"
#include <algorithm>
/*
#ifdef HAVE_WIN32
#include <windows.h>
#endif
*/

namespace Detail
{

//! functor for std::transform to change a string to lowercase or uppercase
class CaseChangeFunctor
{
public:
   //! ctor
   /*! \param bToLower when true, a lowercase operation is provided, else a uppercase op. */
   CaseChangeFunctor(bool bToLower): m_bToLower(bToLower){}

   //! function operator for use in algorithms
   char operator()(char ch){ return static_cast<char>(m_bToLower ? tolower(ch) : toupper(ch)); }
private:
   //! type of case change
   bool m_bToLower;
};

}

void Base::String::Lowercase(std::string& str)
{
   std::transform(str.begin(), str.end(), str.begin(), Detail::CaseChangeFunctor(true));
}

void Base::String::Uppercase(std::string& str)
{
   std::transform(str.begin(), str.end(), str.begin(),  Detail::CaseChangeFunctor(false));
}

bool Base::String::ConvertToUnicode(const std::string& str, std::wstring& wstr)
{
   size_t iSize = mbstowcs(NULL, str.c_str(), 0);
   if (iSize <= 0)
      return false;

   wstr.resize(iSize);
   return -1 != mbstowcs(&*wstr.begin(), str.c_str(), wstr.size());
}

/*
//! \ todo implement for platforms other than Win32
bool Base::String::ConvertToUTF8(const std::wstring& wstr, std::vector<Uint8>& vecUtf8Data)
{
#ifdef HAVE_WIN32
   int iSize = ::WideCharToMultiByte(CP_UTF8, 0,
      wstr.c_str(), static_cast<int>(wstr.size()),
      NULL, 0,
      NULL, NULL);

   vecUtf8Data.resize(iSize);

   int iRet = ::WideCharToMultiByte(CP_UTF8, 0,
      wstr.c_str(), static_cast<int>(wstr.size()),
      reinterpret_cast<LPSTR>(&vecUtf8Data[0]), iSize,
      NULL, NULL);

   return iRet != 0;
#else
   // implement UTF8 conversion
   return false;
#endif
}
*/
