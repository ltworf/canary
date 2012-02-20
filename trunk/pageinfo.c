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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "config.h"
#include "pageinfo.h"
#include "bit_op.h"


const int page_size;

void init() __attribute__((constructor)) {
    page_size=sysconf(_SC_PAGESIZE);
}


void read_ptr_info(pid_t pid, uint64_t ptr) {
    
    uint64_t p;
    
    char pagemap[24];    
    snprintf(pagemap,strlen(pagemap),"/proc/%d/pagemap",pid);
    
    int fd=open(pagemap,O_RDONLY);
    
    //TODO copied and not checked
    long index = (ptr / page_size) * sizeof(unsigned long long);
    //TODO handle errors
}

vm_page_t pgi_pagemap_record(uint64_t record) {
    vm_page_t result;
    
    result.present = bit_apply_mask(63,63,record);
    result.swapped = bit_apply_mask(62,62,record);
    //result.__undefined = (record >> 61) & 1;
    
    if (result.present) {
        result.page_frame_number = bit_apply_mask(0,54,record);
    } else if (result.swapped) {
        result.swap_type = bit_apply_mask(0,4,record);
        result.swap_offset = bit_apply_mask(5,54,record);
    }
    
    result.shift = bit_apply_mask(55,60,record);
    
    return result;
}