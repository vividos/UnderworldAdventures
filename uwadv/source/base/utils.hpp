/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

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
/*! \file utils.hpp

   \brief commonly used types, classes and functions

   ua_trace() is the standard way of logging text during the game; it has
   the same syntax as a printf() call; it may be conditionally switched off

   ua_smart_ptr<T> was inspired by boost::shared_ptr; it represents a pointer
   to an object of class T and may conveniently be used in STL containers like
   std::map or std::vector. The smart pointer does reference counting of the
   pointer and destroys the object when the objects isn't used anymore
   (refcount at 0). The smart pointer may noticeable reduce the number of
   constructions, destructions and copy operations of objects in STL.

   ua_exception is the standard way to throw an exception in uwadv. It should
   be thrown when there is no way to continue normal operation, e.g. when a
   resource file can't be loaded. It can be constructed with an informational
   text about the exception.

   ua_endian_convert16 and ua_endian_convert32 are inline functions to switch
   between host and network byte order, e.g. when loading a little endian
   16-bit value from a resource file on an apple macintosh (which is a big
   endian machine).

   SDL_RWreadN and SDL_RWwriteN are inline functions to read and write 8, 16
   and 32 bit values from SDL_RWops structures. They always read little endian
   values, even on big endian machines.

*/
//! \ingroup base

//@{

// include guard
#ifndef uwadv_utils_hpp_
#define uwadv_utils_hpp_

// needed includes
#include <string>
#include <vector>
#include <exception>
#include "SDL_byteorder.h"


// macros

#define ua_min(a,b) ((a)<(b) ? (a) : (b))
#define ua_max(a,b) ((a)>(b) ? (a) : (b))


// trace messages

int ua_trace_printf(const char *fmt,...);

// debug trace
#if 1 //defined(_DEBUG) || defined(DEBUG)
# define ua_trace ua_trace_printf
#else
# define ua_trace true ? 0 : ua_trace_printf
#endif


// functions

//! creates a folder with given mode
int ua_mkdir(const char* dirname, int mode);

//! checks if a given file name exists
bool ua_file_exists(const char* filename);

//! finds files matching a wildcard pattern pathname
void ua_find_files(const char* pathname, std::vector<std::string>& filelist);

//! makes a string lowercase
void ua_str_lowercase(std::string& str);


// classes

//! smart pointer class
template<typename T>
class ua_smart_ptr
{
public:
   // typedefs
   typedef T  value_type;
   typedef T* pointer_type;
   typedef T& reference_type;

   //! ctor
   explicit ua_smart_ptr(T* ptr=0):px(ptr){ pn=new long(1);}
   //! dtor
   ~ua_smart_ptr(){ dispose(); }

   //! copy ctor
   ua_smart_ptr(const ua_smart_ptr& ptr):px(ptr.px){ ++*(pn = ptr.pn); }
   //! assignment operator
   ua_smart_ptr& operator =(const ua_smart_ptr& ptr){ share(ptr.px,ptr.pn); return *this; }

   //! member-selection operator
   T* operator->() const { return px; }

   //! indirection operator
   T& operator*() const { return *px; }

   //! returns pointer
   T* get(){ return px; }

protected:
   //! deletes pointer
   void dispose()
   {
      if (--*pn == 0)
      { delete px; delete pn; }
   }

   //! shares pointer
   void share(T* rpx, long* rpn)
   {
      if (pn != rpn)
      {
         ++*rpn;
         dispose();
         px = rpx;
         pn = rpn;
      }
   }

protected:
   //! pointer to refcount
   long* pn;
   //! pointer to type object
   T* px;
};


//! exception class
class ua_exception: public std::exception
{
public:
   //! ctor
   ua_exception(const char* desc){ description.assign(desc); };

   //! dtor
   ~ua_exception() throw() {}

   //! returns exception description
   virtual const char* what() const throw()
   {
      return description.c_str();
   }

protected:
   //! exception description
   std::string description;
};


// inline functions

// endian conversion

inline Uint16 ua_endian_convert16(Uint16 x)
{
   return (((x)&0x00ff)<<8) | (((x)&0xff00)>>8);
}

inline Uint32 ua_endian_convert32(Uint32 x)
{
   return ua_endian_convert16(((x)&0xffff0000)>>16) |
      ua_endian_convert16((x)&0x0000ffff)<<16;
}

// SDL_RWops helper functions

inline Uint8 SDL_RWread8(SDL_RWops* rwops)
{
   Uint8 val;
   SDL_RWread(rwops,&val,1,1);
   return val;
}

inline Uint16 SDL_RWread16(SDL_RWops* rwops)
{
   Uint16 val;
   SDL_RWread(rwops,&val,2,1);
#if (SDL_BYTEORDER==SDL_BIG_ENDIAN)
   val = ua_endian_convert16(val);
#endif
   return val;
}

inline Uint32 SDL_RWread32(SDL_RWops* rwops)
{
   Uint32 val;
   SDL_RWread(rwops,&val,4,1);
#if (SDL_BYTEORDER==SDL_BIG_ENDIAN)
   val = ua_endian_convert32(val);
#endif
   return val;
}

inline void SDL_RWwrite8(SDL_RWops* rwops, Uint8 val)
{
   SDL_RWwrite(rwops,&val,1,1);
}

inline void SDL_RWwrite16(SDL_RWops* rwops, Uint16 val)
{
#if (SDL_BYTEORDER==SDL_BIG_ENDIAN)
   val = ua_endian_convert16(val);
#endif
   SDL_RWwrite(rwops,&val,2,1);
}

inline void SDL_RWwrite32(SDL_RWops* rwops, Uint32 val)
{
#if (SDL_BYTEORDER==SDL_BIG_ENDIAN)
   val = ua_endian_convert32(val);
#endif
   SDL_RWwrite(rwops,&val,4,1);
}


#endif
//@}
