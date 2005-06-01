/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2005 Jim Dovey

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
/*! \file atomic.h

   \brief Atomic functions for PowerPC - Mac OS X only

*/

//! \ingroup macosx
//@{

// include guard
#ifndef uwadv_atomic_h_
#define uwadv_atomic_h_

// Mac OS X only
#if defined(__MACH__) && defined(__ppc__)

#include <sys/cdefs.h>

__BEGIN_DECLS

/*! This routine will atomically change the 32-bit word contained at the
    specified memory address, after first comparing it with a given value
    (oldVal). If the current value doesn't match what's expected, it'll
    return failure. Otherwise, it will atomically set the value at the 
    address to the given new value, and will instruct the processor to sync
    its data cache against main memory.
 */
int ua_compare_and_swap( unsigned long oldVal, unsigned long newVal, void * address );

/*! This routine will read an existing 32-bit value from memory, and write
    a new value to the same memory, using the same atomic instructions as
    ua_compare_and_swap(). If the target is changed by another thread in
    between read & write, the routine will loop & retry, to guarantee that 
    it returns whatever value was stored immediately prior to writing.
 */
long ua_fetch_and_store( void * address, unsigned long newVal );

/*! For the ultimate in memory paranoia: sync the processor's data cache with 
    main memory.
 */
void ua_sync_data_cache( void );

__END_DECLS

#else    /* !(__MACH__ && __ppc__) */
#error macosx/atomic.h only supported on PowerPC
#endif   /* __MACH__ && __ppc__ */

#endif
//@}
