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
#include <string.h>

#include "ca_alloc.h"
#include "ca_monitor.h"
#include "config.h"
#include "ca_canary.h"
#include "types.h"

#define PRINT_ALLOCS

#ifdef PRINT_ALLOCS
#include <stdio.h>
#endif



void __init() __attribute__ ((constructor));
static void* __temporary_calloc(size_t, size_t);

static void* (*r_free)(void*) = NULL;
static void* (*r_malloc)(size_t) = NULL;
static void* (*r_calloc)(size_t,size_t) = NULL;
static void* (*r_realloc)(void*,size_t) = NULL;

static size_t min(size_t,size_t);

/**
 * constructor function, initializates the pointers to 
 * the real libc memory functions
 **/
void __init() {
    //This is a weird hack, needed because dlsym calls calloc but apparently doesn't really use it
    r_calloc = __temporary_calloc;
    
    
    r_free = dlsym(RTLD_NEXT,"free");
    r_malloc = dlsym(RTLD_NEXT, "malloc");
    r_realloc = dlsym(RTLD_NEXT, "realloc");
    r_calloc = dlsym(RTLD_NEXT, "calloc");
}

void* malloc(size_t size) {
    size_t bsize=size+(2*sizeof(canary_t));
    void *p = r_malloc(bsize);
    if (p==NULL) return NULL;
    
    buffer_t buf = {p,bsize};
    
    ca_monitor_buffer(buf);
    
#ifdef PRINT_ALLOCS
    printf("malloc(%u) = %p\n", bsize, p);
#endif
    
    return p+sizeof(canary_t);
}

void free(void *ptr) {
    if (ptr==NULL) return;
    void* real_ptr=ptr-sizeof(canary_t);
#ifdef PRINT_ALLOCS
    printf("free(%p)\n",real_ptr);
#endif
    ca_unmonitor_ptr(real_ptr);
}

/**
 * normal libc free
 **/
void __real_free(void* ptr) {
    r_free(ptr);
}

/**
 * normal malloc, unmonitored
 **/
void* __real_malloc(size_t size) {
    return r_malloc(size);
}

/**
 * normal calloc, unmonitored
 **/
void* __real_realloc(void *ptr,size_t size) {
    return r_realloc(ptr,size);
}

/**
 * calloc-like function returning just null.
 * used for a very ugly hack
 **/
static void* __temporary_calloc(size_t x __attribute__((unused)), size_t y __attribute__((unused))) {
    return NULL;
}

void *calloc(size_t nmemb, size_t size) {
    
    size=(size*nmemb)+(2*sizeof(canary_t));
    void *p = r_calloc(1, size);
    if (p==NULL) return NULL;

#ifdef PRINT_ALLOCS
    printf("calloc(1,%d)=%p\n",size,p);
#endif
    
    buffer_t buf = {p,size};
    ca_monitor_buffer(buf);
    
    return p + sizeof(canary_t);
}


void *realloc(void *ptr, size_t size) {
    if (size==0) {
        free(ptr);
        return NULL;
    } else if (ptr==NULL) {
        return malloc(size);
    }
    
    //TODO probably i could do better than that
    void* r_ptr=ptr - sizeof(canary_t);
    size+=sizeof(canary_t)*2;
    
#ifdef PRINT_ALLOCS
    printf("realloc(%p,%d)\n",r_ptr,size);
#endif 
    
    void* new_ptr = malloc(size);
    if (new_ptr==NULL) return NULL;
    
    size_t copy_size=min(ca_test_start_canary(r_ptr)-2*sizeof(canary_t),size);
    
#ifdef PRINT_ALLOCS
    printf("copy_size %d\n",copy_size);
#endif 
    
    memcpy(new_ptr,ptr,copy_size);
    
    free(ptr);
    return new_ptr;
}

/**
 * Returns the maximum out of two values
 **/
static size_t min (size_t a,size_t b) {
    if (a<b)
        return a;
    return b;
}

// gcc -shared -ldl -fPIC jmalloc.c -o libjmalloc.so