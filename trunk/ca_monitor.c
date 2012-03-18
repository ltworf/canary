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
#include "ca_alloc.h"
#include "error.h"
#include "queue.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

void* monitor() __attribute__ ((noreturn));
static inline void delay() ;

static syn_queue_t in_out_buffers;  //Buffers to be monitored

/**
 * Inits the monitor thread, and the 
 * pipes used to communicate with it.
 **/
static void ca_init_thread() {
    static pthread_mutex_t cs_mutex = PTHREAD_MUTEX_INITIALIZER;
    static bool initialized=false;
    
    if (initialized) return;
    
    if (pthread_mutex_trylock(&cs_mutex)!=0) {
        //another thread is initializing
        //waiting for the other thread to finish and returning
        //pthread_mutex_lock(&cs_mutex);
        //pthread_mutex_unlock(&cs_mutex);
        return;
    }
    
    if (q_init(&in_out_buffers,MONITOR_PIPE_SIZE) !=0 ) {
        err_fatal("Unable to create IPC structures.");
    }
    
    pthread_attr_t t_attr;
    pthread_t id;
    
    pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);
    
    pthread_create(&id,&t_attr,monitor,(void*)NULL);
    initialized=true;
    pthread_mutex_unlock(&cs_mutex);
    
}

/**
 * Adds a buffer to be monitored
 **/
void ca_monitor_buffer(buffer_t buffer) {
    ca_init_thread();
    ca_init(buffer.ptr,buffer.size);
    syn_buffer_t p={buffer.ptr,SYN_MONITOR_IN};
    q_put(&in_out_buffers,p);
}

/**
 * Removes a buffer from being monitored
 **/
void ca_unmonitor_ptr(void* ptr) {
    syn_buffer_t b={ptr,SYN_MONITOR_OUT};
    q_put(&in_out_buffers,b);
}

/**
 * delayes :-)
 **/
static inline void delay() {
    if (MONITOR_DELAY!=0) {
        struct timespec a= {0,MONITOR_DELAY};
        nanosleep(&a,NULL);
    }
}

/**
 * This function runs in parallel, in a separate thread
 * it performs the checking.
 **/
void* monitor() {
    
    pgi_init();
    
    queue_t hot; //hot buffers
    queue_t cold;//cold buffers
    void* buffer;
    unsigned int n=0; //count the iterations
    if (!(
        t_init(&hot,100,200) &&
        t_init(&cold,100,50)
        )) 
        err_fatal("Unable to allocate space for the list of buffers.");
    
    while (true) {
        
        delay();
        //printf("b");
        n++;
        
        if (n % (1<<MONITOR_CHECK_PIPES)==0) {
            
            syn_buffer_t new_buffer=q_get(&in_out_buffers);
            
            if (new_buffer.ptr!=NULL) {
                printf("get %p direction=%s\n",new_buffer.ptr, (new_buffer.direction==SYN_MONITOR_IN?"in":"out"));
                if (new_buffer.direction==SYN_MONITOR_IN) {
                    bool r=q_insert(&hot,new_buffer.ptr);
                    printf("added %d\n",q_get_size(&hot));
                    if (!r)
                        err_fatal("Unable to allocate space for the list of buffers.");
                        
                } else {
                    printf("---");
                    __real_free(new_buffer.ptr);
                    q_remove(&hot,new_buffer.ptr);
                    q_remove(&cold,new_buffer.ptr);
                }
            }
        }
        
        
        //Check a cold buffer and eventually moves it to hot
        if (n % (1<<MONITOR_CHECK_COLD)==0) {
            buffer = q_get_current(&cold);
            if (buffer!=NULL && pgi_dirty(buffer)) {
                q_insert(&hot,buffer);
                q_remove(&cold,buffer);
            }
        }
        
        buffer = q_get_current(&hot);
        
        if (buffer==NULL) {
            //sleep(MONITOR_EMPTY_SLEEP);
            continue;
        } else if (!pgi_dirty(buffer)) {
                q_insert(&cold,buffer);
                q_remove(&hot,buffer);
        }
        
        //printf("b%p %d %d\n",buffer,q_get_size(&hot),ca_test(buffer));
        if (ca_test(buffer)==false) {
            err_quit("The canary died! :-(");
        }
        
    }
    
}