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
#include "types.h"

//TODO remove this header
#include <stdio.h>

void __init() __attribute__ ((constructor));
static void* __temporary_calloc(size_t, size_t);

static void* (*r_free)(void*) = NULL;
static void* (*r_malloc)(size_t) = NULL;
static void* (*r_calloc)(size_t,size_t) = NULL;
static void* (*r_realloc)(void*,size_t) = NULL;

/**
 * constructor function, initializates the pointers to 
 * the real libc memory functions
 **/
void __init() {
    //This is a weird hack, needed because dlsym calls calloc but apparently doesn't really use it
    r_calloc = __temporary_calloc;
    r_calloc = dlsym(RTLD_NEXT, "calloc");
    
    r_free = dlsym(RTLD_NEXT,"free");
    r_malloc = dlsym(RTLD_NEXT, "malloc");
    r_realloc = dlsym(RTLD_NEXT, "realloc");
    
}

void* malloc(size_t size) {
    size_t bsize=size+(2*sizeof(canary_t));
    void *p = r_malloc(bsize);
    
    buffer_t buf = {p,bsize};
    
    ca_monitor_buffer(buf);
    
    fprintf(stderr, "malloc(%ul) = %p\n", size, p);
    return p+sizeof(canary_t);
}

void free(void *ptr) {
    void* real_ptr=ptr-sizeof(canary_t);
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
//static void* __temporary_calloc(size_t x, size_t y) {
    return NULL;
}


//TODO this thing doesn't really work
void *calloc(size_t nmemb, size_t size) {
    
    //if (size);
    
    //size=(size*nmemb)+(2*sizeof(canary_t));
    
    void *p = r_calloc(nmemb, size);
    printf("calloc(%d,%d)=%p\n",nmemb,size,p);
    //return p;
    buffer_t buf = {p,size};
    //ca_monitor_buffer(buf);
    
    return p ;//+ sizeof(canary_t);
}

/* TODO
void *realloc(void *ptr, size_t size) {
    if (size==0) {
        free(ptr);
    } else if (ptr==null) {
        return malloc(size);
    }
    
    void* p= r_realloc(ptr,size+sizeof(canary_t));
} */

// gcc -shared -ldl -fPIC jmalloc.c -o libjmalloc.so