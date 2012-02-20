/*
Canary
Copyright (C) 2012  Salvo "LtWorf" Tomaselli

Canary is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

@author Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>
*/

#ifndef PAGEINFO_H
#define PAGEINFO_H

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>


/*
 * kernel page flags
 */

#define KPF_BYTES               8
#define PROC_KPAGEFLAGS         "/proc/kpageflags"

/* copied from kpageflags_read() */
#define KPF_LOCKED              0
#define KPF_ERROR               1
#define KPF_REFERENCED          2
#define KPF_UPTODATE            3
#define KPF_DIRTY               4
#define KPF_LRU                 5
#define KPF_ACTIVE              6
#define KPF_SLAB                7
#define KPF_WRITEBACK           8
#define KPF_RECLAIM             9
#define KPF_BUDDY               10

/* [11-20] new additions in 2.6.31 */
#define KPF_MMAP                11
#define KPF_ANON                12
#define KPF_SWAPCACHE           13
#define KPF_SWAPBACKED          14
#define KPF_COMPOUND_HEAD       15
#define KPF_COMPOUND_TAIL       16
#define KPF_HUGE                17
#define KPF_UNEVICTABLE         18
#define KPF_HWPOISON            19
#define KPF_NOPAGE              20
#define KPF_KSM                 21

/* [32-] kernel hacking assistances */
#define KPF_RESERVED            32
#define KPF_MLOCKED             33
#define KPF_MAPPEDTODISK        34
#define KPF_PRIVATE             35
#define KPF_PRIVATE_2           36
#define KPF_OWNER_PRIVATE       37
#define KPF_ARCH                38
#define KPF_UNCACHED            39

/* [48-] take some arbitrary free slots for expanding overloaded flags
 * not part of kernel API
 */
#define KPF_READAHEAD           48
#define KPF_SLOB_FREE           49
#define KPF_SLUB_FROZEN         50
#define KPF_SLUB_DEBUG          51

#define KPF_ALL_BITS            ((uint64_t)~0ULL)
#define KPF_HACKERS_BITS        (0xffffULL << 32)
#define KPF_OVERLOADED_BITS     (0xffffULL << 48)
#define BIT(name)               (1ULL << KPF_##name)
#define BITS_COMPOUND           (BIT(COMPOUND_HEAD) | BIT(COMPOUND_TAIL))


typedef struct {
    uint64_t page_frame_number; //Bits 0-54  page frame number (PFN) if present
    uint8_t swap_type;          //Bits 0-4   swap type if swapped
    uint64_t swap_offset;       //Bits 5-54  swap offset if swapped
    uint8_t shift;              //Bits 55-60 page shift (page size = 1<<page shift)
    //bool __undefined;           //Bit  61    reserved for future use currently unset
    bool swapped;               //Bit  62    page swapped
    bool present;               //Bit  63    page present
} vm_page_t;

#endif