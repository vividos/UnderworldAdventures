/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

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

   ua_smart_ptr was inspired by boost::shared_ptr

*/

// include guard
#ifndef __uwadv_utils_hpp_
#define __uwadv_utils_hpp_

// needed includes
#include <string>
#include <exception>


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
int ua_mkdir(const char *dirname, int mode);


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
   ua_smart_ptr(const ua_smart_ptr &ptr):px(ptr.px){ ++*(pn = ptr.pn); }
   //! assignment operator
   ua_smart_ptr &operator =(const ua_smart_ptr &ptr){ share(ptr.px,ptr.pn); return *this; }

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
   long *pn;
   //! pointer to type object
   T*px;
};


//! exception class
class ua_exception: public std::exception
{
public:
   //! ctor
   ua_exception(const char *desc){ description.assign(desc); };

   //! dtor
   ~ua_exception() throw() {}

   //! returns exception description
   virtual const char *what() const throw()
   {
      return description.c_str();
   }

protected:
   //! exception description
   std::string description;
};

#endif
