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

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdlib.h>

#include "ca_alloc.h"
#include "ca_monitor.h"
#include "config.h"
#include "types.h"

//TODO remove this header
#include <stdio.h>

void* malloc(size_t size) {
    size_t bsize=size+(2*sizeof(canary_t));
    void *p = __real_malloc(bsize);
    
    buffer_t buf = {p,bsize};
    
    ca_monitor_buffer(buf);
    
    fprintf(stderr, "malloc(%d) = %p\n", size, p);
    return p+sizeof(canary_t);
}

void free(void *ptr) {
    
    
    void* real_ptr=ptr-sizeof(canary_t);
    
    //TODO the actual free should be delayed to when the buffer is removed from the list of checked buffers
    //real_free(real_ptr);
    
    ca_unmonitor_ptr(real_ptr);
}

/**
 * normal libc free
 **/
void __real_free(void* ptr) {
    static void* (*r_free)(void*) = NULL;
    if (!r_free)
        r_free = dlsym(RTLD_NEXT,"free");
    r_free(ptr);
}

/**
 * normal malloc, unmonitored
 **/
void* __real_malloc(size_t size) {
    static void* (*r_malloc)(size_t) = NULL;
    if (!r_malloc)
        r_malloc = dlsym(RTLD_NEXT, "malloc");    
    return r_malloc(size);
}

// gcc -shared -ldl -fPIC jmalloc.c -o libjmalloc.so