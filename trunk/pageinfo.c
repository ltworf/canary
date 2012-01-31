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

void read_ptr_info(pid_t pid,int ptr) {
    
    uint64_t p;
    
    char pagemap[24];    
    snprintf(pagemap,strlen(pagemap),"/proc/%d/pagemap",pid);
    
    int fd=open(pagemap,O_RDONLY);
    
    //long index = (vas / PAGE_SIZE) * sizeof(unsigned long long);
    //TODO handle errors
}

typedef struct {
    uint64_t page_frame_number; //* Bits 0-54  page frame number (PFN) if present
    uint8_t swap_type; //16          * Bits 0-4   swap type if swapped
    uint64_t swap_offset; //17          * Bits 5-54  swap offset if swapped
    uint8_t shift; // 18          * Bits 55-60 page shift (page size = 1<<page shift)
    bool __undefined; //19          * Bit  61    reserved for future use
    bool swapped; //20          * Bit  62    page swapped
    bool present; //Bit 63    page present
} vm_page_t;


vm_page_t pgi_pagemap_record(uint64_t record) {
    
}