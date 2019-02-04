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

*/
/*! \file smart_ptr.hpp

   \brief Smart pointer class
   \todo replace with std::shared_ptr
*/
/*
   Most of this class is taken from Boost, so the below copyright statement
   also applies. Modifications made were introduction of a deletor function
   object that can be specified.
*/
//
//  detail/shared_ptr_nmt.hpp - shared_ptr.hpp without member templates
//
//  (C) Copyright Greg Colvin and Beman Dawes 1998, 1999.
//  Copyright (c) 2001, 2002 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/smart_ptr/shared_ptr.htm for documentation.
//

// include guard
#ifndef uwadv_base_smart_ptr_hpp_
#define uwadv_base_smart_ptr_hpp_

namespace Base
{

//! delete functor that deletes a pointer with the delete operator
template <typename T>
class PtrDeletor
{
public:
   //! deletes given pointer
   void operator()(T* p){ delete p; }
};


//! smart pointer class
/*! The class was inspired by boost::shared_ptr; it represents a pointer
    to an object of class T and may conveniently be used in STL containers
    like std::map or std::vector. The smart pointer does reference counting of
    the pointer and destroys the object when the objects isn't used anymore
    (refcount drops to 0). The smart pointer may noticeable reduce the number
    of constructions, destructions and copy operations of objects in STL
    container.
    Note: unlike Boost's shared_ptr, the SmartPtr class cannot specified with
    incomplete types.
    \param T type of pointer to be managed
    \param TDeletor custom delete functor to clean up the pointer
    \note Don't put array pointer into this smart pointer; use std::vector
          instead.
*/
template<typename T, typename TDeletor=PtrDeletor<T> >
class SmartPtr
{
public:
   // typedefs
   typedef T  element_type;   //!< value type pointed to
   typedef T  value_type;     //!< value type pointed to
   typedef T* pointer_type;   //!< pointer type
   typedef T& reference_type; //!< reference type
   typedef long count_type;   //!< reference count type

   //! ctor
   explicit SmartPtr(T* p=0)
      :px(p)
   {
      try
      {
         pn = new count_type(1);
      }
      catch(...)
      {
         TDeletor t;
         t(p);
         throw;
      }
   }

   //! dtor
   ~SmartPtr()
   {
      if (--*pn == 0)
      {
         TDeletor t;
         t(px);
         delete pn;
      }
   }

   //! copy ctor
   SmartPtr(const SmartPtr& r)
      :px(r.px)
   {
      pn = r.pn;
      ++*pn;
   }

   //! assignment operator
   SmartPtr& operator=(const SmartPtr& r)
   {
      SmartPtr(r).swap(*this);
      return *this;
   }

   //! resets the smart ptr to another ptr or to NULL
   void reset(T* p=0)
   {
      UaAssert(p == 0 || p != px);
      SmartPtr(p).swap(*this);
   }

   //! indirection operator
   T& operator*() const { UaAssert(px != 0); return *px; }

   //! member-selection operator
   T* operator->() const { UaAssert(px != 0); return px; }

   //! returns pointer
   T* get() const { return px; }

   //! swaps pointer with given pointer
   void swap(SmartPtr<T, TDeletor>& other)
   {
      std::swap(px, other.px);
      std::swap(pn, other.pn);
   }

protected:
   //! contained pointer
   T* px;
   //! pointer to reference count
   count_type* pn;
};

//! global compare operator
template<class T, class TDeletor, class U>
inline bool operator==(SmartPtr<T, TDeletor> const& a, SmartPtr<U, TDeletor> const& b)
{
   return a.get() == b.get();
}

//! global unequality operator
template<class T, class TDeletor, class U>
inline bool operator!=(SmartPtr<T, TDeletor> const& a, SmartPtr<U, TDeletor> const& b)
{
   return a.get() != b.get();
}

//! global less operator for ptr comparison
template<class T, class TDeletor>
inline bool operator<(SmartPtr<T, TDeletor> const& a, SmartPtr<T, TDeletor> const& b)
{
   return std::less<T*>()(a.get(), b.get());
}

//! swaps smart pointers
template<class T, class TDeletor>
void swap(SmartPtr<T, TDeletor>& a, SmartPtr<T, TDeletor>& b)
{
   a.swap(b);
}

} // namespace Base

#endif
