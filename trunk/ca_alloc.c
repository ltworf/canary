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

#include "config.h"
#include "types.h"

//TODO remove this header
#include <stdio.h>

void* malloc(size_t size) {
    static void* (*real_malloc)(size_t) = NULL;
    if (!real_malloc)
        real_malloc = dlsym(RTLD_NEXT, "malloc");

    size_t bsize=size+(2*sizeof(canary_t));
    void *p = real_malloc(bsize);
    
    buffer_t buf = {p,bsize};
    
    fprintf(stderr, "malloc(%d) = %p\n", size, p);
    return p;
}

void free(void *ptr) {
    static void* (*real_free)(void*) = NULL;
    if (!real_free)
        real_free = dlsym(RTLD_NEXT,"free");
    
    real_free(ptr);
}
// gcc -shared -ldl -fPIC jmalloc.c -o libjmalloc.so