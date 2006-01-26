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

   $Id$

*/
/*! \file smart_ptr.hpp

   \brief Smart pointer class

*/

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
*/
template<typename T, typename TDeletor=PtrDeletor<T> >
class SmartPtr
{
public:
   // typedefs
   typedef T  value_type;     //!< value type pointed to
   typedef T* pointer_type;   //!< pointer type
   typedef T& reference_type; //!< reference type

   //! ctor
   explicit SmartPtr(T* p=0){ data=new SmartPtrData; data->px=p; }
   //! dtor
   ~SmartPtr(){ dispose(); }

   //! copy ctor
   SmartPtr(const SmartPtr& ptr):data(ptr.data){ ++(data->n); }
   //! assignment operator
   SmartPtr& operator =(const SmartPtr& ptr){ share(ptr.data); return *this; }

   //! member-selection operator
   T* operator->() const { return data->px; }

   //! indirection operator
   T& operator*() const { return *data->px; }

   //! returns pointer
   T* get() const { return data->px; }

   //! resets the smart ptr to an empty value
   void reset()
   {
      if (--data->n == 0)
      {
         TDeletor d;
         d(data->px);
         data->px = NULL;
      }
   }

   //! resets the smart ptr to another value
   void reset(T* p)
   {
      if (p == data->px)
         return;

      dispose();
      data = new SmartPtrData;
      data->px = p;
    }

protected:
   //! smart ptr data
   struct SmartPtrData
   {
      //! ctor
      SmartPtrData():n(1), px(NULL){}

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
      {
         TDeletor d;
         d(data->px);
         delete data;
         data = NULL;
      }
   }

   //! shares pointer
   void share(SmartPtrData* rdata)
   {
      if (data != rdata)
      {
         ++rdata->n;
         dispose();
         data = rdata;
      }
   }
};

} // namespace Base

#endif
