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

static int page_size;
int flags_fd;
int pagemap_fd;

/**
 * Initializes the file descriptors to
 * obtain the status of the various pages.
 **/
void pgi_init() {
    char pagemap[24];
    
    page_size=sysconf(_SC_PAGESIZE);
    
    flags_fd=open(PROC_KPAGEFLAGS,O_RDONLY|O_CLOEXEC);
    
    pid_t pid = getpid();
    snprintf(pagemap,strlen(pagemap),"/proc/%d/pagemap",pid);
    pagemap_fd=open(pagemap,O_RDONLY|O_CLOEXEC);
    
    if (flags_fd == -1 || pagemap_fd == -1) {
        err_fatal("Unable to open the required files in /proc");
    }
}


void read_ptr_info(uint64_t ptr) {
    
    uint64_t p;
    
    //TODO copied and not checked
    long index = (ptr / page_size) * sizeof(unsigned long long);
    //TODO handle errors
    
    
    off_t pagemap_seek = lseek(pagemap_fd,index,SEEK_SET);
    
    if (pagemap_seek==-1) {
        //TODO handle errors
    }
    
    uint64_t buf;
    ssize_t pagemap_read=read(pagemap_fd,&buf,sizeof(uint64_t));
    //TODO check errors
    
    pgi_pagemap_record(buf);
    //TODO get the flags, if assigned
}

static vm_page_t pgi_pagemap_record(uint64_t record) {
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

/**
 * Reads one record from kpageflags and returns the result
 **/
uint16_t pgi_pageflags(uint16_t index) {
    
    off_t seek_r = lseek(flags_fd,index,SEEK_SET);
    //TODO handle errors
    
    uint64_t buf;
    ssize_t read_r = read(flags_fd,&buf,sizeof(uint64_t));
    
    return buf;
}