/*
Canary
Copyright (C) 2012  Salvo "LtWorf" Tomaselli

Weborf is free software: you can redistribute it and/or modify
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


#include "config.h"
#include "ca_pageinfo.h"
#include "bit_op.h"
#include "error.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

static vm_page_t pgi_pagemap_record(uint64_t record);
static vm_page_t read_ptr_info(uint64_t ptr);
static uint64_t pgi_pageflags(uint64_t index);
char *page_flag_longname(uint64_t flags); //TODO

static int page_size;
int flags_fd;
int pagemap_fd;

/**
 * Initializes the file descriptors to
 * obtain the status of the various pages.
 **/
void pgi_init() {
    
    page_size=sysconf(_SC_PAGESIZE);
    
    flags_fd=open(PROC_KPAGEFLAGS,O_RDONLY|O_CLOEXEC);
    
    pagemap_fd=open("/proc/self/pagemap",O_RDONLY|O_CLOEXEC);
    
    if (flags_fd == -1 || pagemap_fd == -1) {
        err_fatal("Unable to open the required files in /proc");
    }
}

/**
 * returns true if the page has been modified, false otherwise.
 * if two or more subsequent requests refer to the same page,
 * the result is cached and not extracted again from the kernel.
 * 
 * ptr is the memory pointer.
 **/
bool pgi_dirty(void* ptr) {
    static uint64_t old_index;
    static bool old_result;
    
    //TODO copied and not checked
    uint64_t ptr1 = ptr-((uint64_t)ptr % page_size); //Address of beginning of the page
    uint64_t index = (ptr1 / page_size) * sizeof(void*);
    
    //caching the result
    if (index==old_index) {
        return old_result;
    } else {
        old_index=index;
    }
    
    //Get the flags from the page in virtual memory
    vm_page_t vpage= read_ptr_info(index);
    if (!vpage.present || vpage.swapped) {
        return (old_result=false);
    }
    return (old_result=true); //TODO i might want to do other stuff like check if the page is dirty
    
}

/**
 * Returns the flags assigned to a virtual page.
 **/
static vm_page_t read_ptr_info(uint64_t ptr) {
    uint64_t buf;
    ssize_t pagemap_read=pread(pagemap_fd,&buf,sizeof(uint64_t),ptr);
    if (pagemap_read!=sizeof(uint64_t)) {
        err_fatal("Read from pagemap with unexpected value");
    }
    return pgi_pagemap_record(buf);
}

static vm_page_t pgi_pagemap_record(uint64_t record) {
    vm_page_t result;
    result.present = record>>63;//bit_apply_mask(63,63,record);
    result.swapped = (record>>62) & 1;
    //result.__undefined = (record >> 61) & 1;
    
    /*if (result.present) {
        result.page_frame_number = record & 36028797018963967lu;
        //bit_apply_mask(0,54,record);
    } else if (result.swapped) {
        result.swap_type = bit_apply_mask(0,4,record);
        result.swap_offset = bit_apply_mask(5,54,record);
    }
    
    result.shift = bit_apply_mask(55,60,record);*/
    
    return result;
}

/**
 * Reads one record from kpageflags and returns the result.
 * index
 **/
static uint64_t pgi_pageflags(uint64_t index) {
    index+=8-(index%8);
    uint64_t buf;
    ssize_t read_r = pread(flags_fd,&buf,sizeof(uint64_t),index);
    if (read_r!=sizeof(uint64_t)) {
        
        
        err_fatal("Read from pageflag returned unexpected value");
    }
    
    return buf;
}


/////////////////////////////////////////////////////////////////////////////////////////////////

static const char *page_flag_names[] = {
        [KPF_LOCKED]            = "L:locked",
        [KPF_ERROR]             = "E:error",
        [KPF_REFERENCED]        = "R:referenced",
        [KPF_UPTODATE]          = "U:uptodate",
        [KPF_DIRTY]             = "D:dirty",
        [KPF_LRU]               = "l:lru",
        [KPF_ACTIVE]            = "A:active",
        [KPF_SLAB]              = "S:slab",
        [KPF_WRITEBACK]         = "W:writeback",
        [KPF_RECLAIM]           = "I:reclaim",
        [KPF_BUDDY]             = "B:buddy",

        [KPF_MMAP]              = "M:mmap",
        [KPF_ANON]              = "a:anonymous",
        [KPF_SWAPCACHE]         = "s:swapcache",
        [KPF_SWAPBACKED]        = "b:swapbacked",
        [KPF_COMPOUND_HEAD]     = "H:compound_head",
        [KPF_COMPOUND_TAIL]     = "T:compound_tail",
        [KPF_HUGE]              = "G:huge",
        [KPF_UNEVICTABLE]       = "u:unevictable",
        [KPF_HWPOISON]          = "X:hwpoison",
        [KPF_NOPAGE]            = "n:nopage",
        [KPF_KSM]               = "x:ksm",

        [KPF_RESERVED]          = "r:reserved",
        [KPF_MLOCKED]           = "m:mlocked",
        [KPF_MAPPEDTODISK]      = "d:mappedtodisk",
        [KPF_PRIVATE]           = "P:private",
        [KPF_PRIVATE_2]         = "p:private_2",
        [KPF_OWNER_PRIVATE]     = "O:owner_private",
        [KPF_ARCH]              = "h:arch",
        [KPF_UNCACHED]          = "c:uncached",

        [KPF_READAHEAD]         = "I:readahead",
        [KPF_SLOB_FREE]         = "P:slob_free",
        [KPF_SLUB_FROZEN]       = "A:slub_frozen",
        [KPF_SLUB_DEBUG]        = "E:slub_debug",
};

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

 static char *page_flag_longname(uint64_t flags)
{
        static char buf[1024];
        unsigned long int i, n;

        for (i = 0, n = 0; i < ARRAY_SIZE(page_flag_names); i++) {
                if (!page_flag_names[i])
                        continue;
                if ((flags >> i) & 1)
                        n += snprintf(buf + n, sizeof(buf) - n, "%s,",
                                        page_flag_names[i] + 2);
        }
        if (n)
                n--;
        buf[n] = '\0';

        return buf;
}