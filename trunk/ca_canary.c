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

#include "config.h"
#include "types.h"
#include "ca_canary.h"
#include "error.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>

/**
 * Reads "size" from /dev/random and writes it to ptr
 * In case of failure it will return false
 **/
static bool ca_random_val(void* ptr,size_t size) {
    int fd= open("/dev/random",O_RDONLY);
    
    if (fd==-1) return false;
    
    ssize_t r= read(fd,ptr,size);
    close(fd);
    
    if (r!=size) return false;
    
    return true;
}

static canary_t random=0;       //Random canary
static pthread_once_t random_is_initialized = PTHREAD_ONCE_INIT;

/**
 * Inits the random variable
 **/
static void ca_init_random() {
    if (!ca_random_val(&random,sizeof(canary_t)))
        err_fatal("Not enough random data");
}

/**
 * Returns a random canary_t value. Always the same at every call
 **/
static canary_t ca_get_random() {
    (void) pthread_once(&random_is_initialized, ca_init_random);
    return random;
}

/**
 * Sets a canary in the beginning of the buffer.
 * For the size, check the type canary_t
 * 
 * This function makes use of random values,
 * in case the kernel has not enough entropy or
 * random data is not available for any other reason,
 * the failure will cause the termination of the program.
 **/
static void ca_set_canary(void* ptr) {
    canary_t *dest=ptr;
    canary_t c = ptr;
    dest[0]=c^ca_get_random();
}


/**
 * Checks if the canary cointained at ptr
 * is valid or not
 * 
 * Returns true if the canary is valid.
 **/
static bool ca_test_canary(void*ptr) {
    canary_t *dest=ptr;
    canary_t c = ptr;
    
    canary_t expected = c^ca_get_random();
    
    if (expected == dest[0]) {
        return true;
    }
    return false;
}

/**
 * Initiates canaries on a buffer.
 * 
 * Ptr is the pointer to the buffer
 * and size is the total size of the buffer
 * (including the space for the canaries).
 * 
 * This function makes use of random values,
 * in case the kernel has not enough entropy or
 * random data is not available for any other reason,
 * the failure will cause the termination of the program.
 **/
void ca_init(void* ptr,size_t size) {
    ca_set_canary(ptr);
    ca_set_canary(ptr+size-sizeof(canary_t));
}

/**
 * Checks the canaries of a buffer.
 * Returns true if the canaries have the expected
 * value, and false otherwise
 **/
bool ca_test(void* ptr,size_t size) {
    return ca_test_canary(ptr) && ca_test_canary(ptr+size-sizeof(canary_t));
}

