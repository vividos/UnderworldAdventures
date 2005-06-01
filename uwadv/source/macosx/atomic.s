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
/*! \file atomic.s

   \brief PowerPC atomic functions

*/

#if defined(__MACH__) && defined(__ppc__)

#include <architecture/ppc/asm_help.h>

.macro ENTRY
   .text
   .align      2
   .globl      $0
$0:
.endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; Note - PowerPC Mach-O ABI has arguments placed in registers, starting at r3

/*
int ua_compare_and_swap( unsigned long oldVal, unsigned long newVal, unsigned long* address )
*/
   ENTRY _ua_compare_and_swap
.cas_loop:
   lwarx    r6, 0, r5            ; load 32-bit word at 'address' (r5) into r6, and 
                                 ;  reserve memory location
   cmpw     r6, r3               ; compare contents of r6 with 'oldVal' (r3)
   bne-     .cas_fail            ; branch to failure handle if not equal
   stwcx.   r4, 0, r5            ; try to store 'newVal' (r4) into 'address' (r5)
   bne-     .cas_loop            ; if store failed (reservation gone), then retry
;   sync                          ; synchronize cache with main memory
                                 ;  this ensures that another thread about to call this
                                 ;  doesn't compare against a cached copy of the value we
                                 ;  just replaced.
   li       r3, 1                ; return 1 on success
   blr                           ; return to calling function
.cas_fail:
   li       r3, 0                ; return 0 on failure
   blr                           ; return to calling function

/*
long ua_fetch_and_store( unsigned long* address )
*/
   ENTRY _ua_fetch_and_store
.fas_loop:
   lwarx    r5, 0, r3            ; load word at 'address' (r3) into r5, and reserve memory
   stwcx.   r4, 0, r3            ; store 'newVal' (r4) into 'address'
   bne-     .fas_loop            ; if reservation not held, loop & try again
;   sync                          ; update processor data cache from memory
   mr       r3,r5                ; return value from storage
   blr                           ; return to calling function

/*
void ua_sync_data_cache( void )
*/
   ENTRY _ua_sync_data_cache
   sync                          ; synchronize processor data cache
   blr                           ; return to calling function

#endif
