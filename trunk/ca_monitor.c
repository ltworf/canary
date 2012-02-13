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

#include "config.h"
#include "types.h"
#include "ca_monitor.h"
#include "ca_canary.h"

#include <pthread.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static int in_monitor_pipe[2];
static int out_monitor_pipe[2];

void __init() __attribute__((constructor));
void __init() {
    
    if (
        pipe2(in_monitor_pipe,O_NONBLOCK|O_CLOEXEC) + 
        pipe2(out_monitor_pipe,O_NONBLOCK|O_CLOEXEC)
        !=0
    ) {
        //TODO no pipe, should terminate
    }
    
    //TODO hopefully here i should launch the new thread
}


void ca_monitor_buffer(buffer_t buffer) {
    ca_init(buffer.ptr,buffer.size);
    
    //TODO critical section
    //TODO write the buffer_t on the pipe
    
}

void ca_unmonitor_ptr(void* ptr) {
    
}