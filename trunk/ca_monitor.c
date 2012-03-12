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
#include "ca_tree.h"
#include "ca_pageinfo.h"
#include "error.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>

static int in_monitor_pipe[2];
static int out_monitor_pipe[2];

#define PIPE_READ 0
#define PIPE_WRITE 1

void* monitor() __attribute__ ((noreturn));

static pthread_once_t thread_is_initialized = PTHREAD_ONCE_INIT;

/**
 * Inits the monitor thread, and the 
 * pipes used to communicate with it.
 **/
static void ca_init_thread() {
    pgi_init();
    
    if (
        pipe2(in_monitor_pipe,O_CLOEXEC|O_NONBLOCK) + 
        pipe2(out_monitor_pipe,O_CLOEXEC|O_NONBLOCK)
        !=0
    ) {
        err_fatal("Unable to create IPC structures.");
    }
    
        /*if (
        fcntl(in_monitor_pipe[PIPE_READ],F_SETFD,O_NONBLOCK) +
        fcntl(out_monitor_pipe[PIPE_READ],F_SETFD,O_NONBLOCK)
        !=0
    ) {
        err_fatal("Unable to use correctly IPC structures");
    }*/

    
    pthread_attr_t t_attr;
    pthread_t id;
    
    pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);
    
    pthread_create(&id,&t_attr,monitor,(void*)NULL);
}

/**
 * Adds a buffer to be monitored
 **/
void ca_monitor_buffer(buffer_t buffer) {
    static pthread_mutex_t cs_mutex = PTHREAD_MUTEX_INITIALIZER;
    (void) pthread_once(&thread_is_initialized, ca_init_thread); //Initializes the monitor at the first malloc
    int res=-1;
    
    
    ca_init(buffer.ptr,buffer.size);
    
    pthread_mutex_lock(&cs_mutex);
    while (res<1) {
        /*
         * Since pipes are non blocking, cycles until the data is written.
         * Hopefully it will work at the 1st attempt in most cases
         */
        res=write(in_monitor_pipe[PIPE_WRITE],&(buffer.ptr),sizeof(void*));
    }
    pthread_mutex_unlock(&cs_mutex);
    
}

/**
 * Removes a buffer from being monitored
 **/
void ca_unmonitor_ptr(void* ptr) {

    static pthread_mutex_t cs_mutex = PTHREAD_MUTEX_INITIALIZER;
    int res=-1;
    pthread_mutex_lock(&cs_mutex);
    while (res<1) {
        /*
         * Since pipes are non blocking, cycles until the data is written.
         * Hopefully it will work at the 1st attempt in most cases
         */
        res=write(out_monitor_pipe[PIPE_WRITE],&(ptr),sizeof(void*));
    }
    pthread_mutex_unlock(&cs_mutex);
}

/**
 * This function runs in parallel, in a separate thread
 * it performs the checking.
 **/
void* monitor() {
    queue_t hot; //hot buffers
    queue_t cold;//cold buffers
    void* buffer;
    unsigned int n=0; //count the iterations
    if (!(
        q_init(&hot,100) &&
        q_init(&cold,100)
        )) 
        err_fatal("Unable to allocate space for the list of buffers.");
    
    while (true) {
        n++;
        if (n % (1<<MONITOR_CHECK_PIPES)==0) {
            void* new_buffer;
            int r = read(in_monitor_pipe[PIPE_READ],&new_buffer,sizeof(void*));
            
            if (r>=0) {
                if (!q_insert(&hot,new_buffer)) {
                    err_fatal("Unable to allocate space for the list of buffers.");
                }
            }
            
            int s = read(in_monitor_pipe[PIPE_READ],&new_buffer,sizeof(void*));
            if (s>=0) {
                __real_free(new_buffer);
                q_remove(&hot,new_buffer);
                q_remove(&cold,new_buffer);
            }
        }
        
        if (n % (1<<MONITOR_CHECK_COLD)==0) {
            buffer = q_get_current(&cold);
            if (buffer!=NULL && pgi_dirty(buffer)) {
                q_insert(&hot,buffer);
                q_remove(&cold,buffer);
            }
        }
        
        buffer = q_get_current(&hot);
        if (buffer==NULL) {
            //TODO react to this!
            continue;
        } else if (pgi_dirty(buffer)) {
                q_insert(&cold,buffer);
                q_remove(&hot,buffer);
        }
        
        
        if (ca_test(buffer)==false) {
            //TODO add some more informations
            err_quit("The canary died! :-(");
        }
        
    }
    
}