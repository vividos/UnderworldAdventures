/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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

//! returns minimum value
#define ua_min(a,b) ((a)<(b) ? (a) : (b))
//! returns maximum value
#define ua_max(a,b) ((a)>(b) ? (a) : (b))


// trace messages

int ua_trace_printf(const char *fmt,...);

// debug trace
/*! \fn ua_trace
    The function ua_trace is used to log text during the game. The text is
    printed on the console (the program has to be built with console support
    to show the text.

    The function has the same syntax as the printf function and uses the
    ua_trace_printf() helper function. The function can be switched off
    conditionally.
*/
#if 1 //defined(_DEBUG) || defined(DEBUG)
# define ua_trace ua_trace_printf
#else
# define ua_trace true ? 0 : ua_trace_printf
#endif


// assert messages

//! checks for assert
void ua_assert_check(bool cond, const char* cond_str, const char* file, int line);

//! macro to check for conditions and pass source filename and line
#define ua_assert(cond) ua_assert_check((cond), #cond, __FILE__, __LINE__);


// functions

//! creates a folder with given mode
int ua_mkdir(const char* dirname, int mode);

//! checks if a given file name exists
bool ua_file_exists(const char* filename);

//! finds files matching a wildcard pattern pathname
void ua_find_files(const char* pathname, std::vector<std::string>& filelist);

//! makes a string lowercase
void ua_str_lowercase(std::string& str);

//! makes a string uppercase
void ua_str_uppercase(std::string& str);


// classes

//! smart pointer class
/*! The class was inspired by boost::shared_ptr; it represents a pointer
    to an object of class T and may conveniently be used in STL containers
    like std::map or std::vector. The smart pointer does reference counting of
    the pointer and destroys the object when the objects isn't used anymore
    (refcount drops to 0). The smart pointer may noticeable reduce the number
    of constructions, destructions and copy operations of objects in STL.
*/
template<typename T>
class ua_smart_ptr
{
public:
   // typedefs
   typedef T  value_type;     //!< value type pointed to
   typedef T* pointer_type;   //!< pointer type
   typedef T& reference_type; //!< reference type

   //! ctor
   explicit ua_smart_ptr(T* ptr=0){ data=new ua_smart_ptr_data; data->px=ptr; }
   //! dtor
   ~ua_smart_ptr(){ dispose(); }

   //! copy ctor
   ua_smart_ptr(const ua_smart_ptr& ptr):data(ptr.data){ ++(data->n); }
   //! assignment operator
   ua_smart_ptr& operator =(const ua_smart_ptr& ptr){ share(ptr.data); return *this; }

   //! member-selection operator
   T* operator->() const { return data->px; }

   //! indirection operator
   T& operator*() const { return *data->px; }

   //! returns pointer
   T* get(){ return data->px; }

protected:
   //! smart ptr data
   struct ua_smart_ptr_data
   {
      //! ctor
      ua_smart_ptr_data():n(1), px(NULL){}

      //! pointer to refcount
      long n;
      //! pointer to type object
      T* px;
   }* data;

protected:
   //! deletes pointer
   void dispose()
   {
      if (--data->n == 0)
      { delete data->px; delete data; }
   }

   //! shares pointer
   void share(ua_smart_ptr_data* rdata)
   {
      if (data != rdata)
      {
         ++rdata->n;
         dispose();
         data = rdata;
      }
   }
};


//! exception class
/*! The ua_exception class is the class used in uwadv to throw an exception.
    It should be thrown when there is no way to continue normal operation,
    e.g. when a needed resource file can't be loaded. It can be constructed
    with an informational text about the exception.
*/
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

//! endian-converts 16-bit value
/*! Converts between host and network byte ordering by swapping the low and
    high byte. This can become handy when reading a little-endian value from a
    resource file when running on a big endian machine (like an Apple Mac).
*/
inline Uint16 ua_endian_convert16(Uint16 x)
{
   return Uint16(((x)&0x00ff)<<8) | (((x)&0xff00)>>8);
}

//! endian-converts 32-bit value
/*! Converts between host and network byte ordering by swapping the low and
    high word. */
inline Uint32 ua_endian_convert32(Uint32 x)
{
   return Uint32(ua_endian_convert16(((x)&0xffff0000)>>16)) |
      Uint32(ua_endian_convert16((x)&0x0000ffff)<<16);
}

// SDL_RWops helper functions

//! function to read 8-bit value from SDL_RWops structure
inline Uint8 SDL_RWread8(SDL_RWops* rwops)
{
   Uint8 val;
   SDL_RWread(rwops,&val,1,1);
   return val;
}

//! function to read little-endian 16-bit value from SDL_RWops structure
/*! The function always reads a little-endian value, even on big endian
    machines. */
inline Uint16 SDL_RWread16(SDL_RWops* rwops)
{
   Uint16 val;
   SDL_RWread(rwops,&val,2,1);
#if (SDL_BYTEORDER==SDL_BIG_ENDIAN)
   val = ua_endian_convert16(val);
#endif
   return val;
}

//! function to read little-endian 32-bit value from SDL_RWops structure
inline Uint32 SDL_RWread32(SDL_RWops* rwops)
{
   Uint32 val;
   SDL_RWread(rwops,&val,4,1);
#if (SDL_BYTEORDER==SDL_BIG_ENDIAN)
   val = ua_endian_convert32(val);
#endif
   return val;
}

//! function to write 8-bit value from SDL_RWops structure
inline void SDL_RWwrite8(SDL_RWops* rwops, Uint8 val)
{
   SDL_RWwrite(rwops,&val,1,1);
}

//! function to write little-endian 16-bit value from SDL_RWops structure
inline void SDL_RWwrite16(SDL_RWops* rwops, Uint16 val)
{
#if (SDL_BYTEORDER==SDL_BIG_ENDIAN)
   val = ua_endian_convert16(val);
#endif
   SDL_RWwrite(rwops,&val,2,1);
}

//! function to write little-endian 32-bit value from SDL_RWops structure
inline void SDL_RWwrite32(SDL_RWops* rwops, Uint32 val)
{
#if (SDL_BYTEORDER==SDL_BIG_ENDIAN)
   val = ua_endian_convert32(val);
#endif
   SDL_RWwrite(rwops,&val,4,1);
}


#endif
//@}
