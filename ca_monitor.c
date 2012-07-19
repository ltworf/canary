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
static inline void delay();
static void check_pipes();
static void cold_2_hot();

static queue_t in_out_buffers;  //Buffers to be monitored
tree_t hot; //hot buffers
tree_t cold;//cold buffers


/**
 * if the page containing the buffer is not dirty
 * it is moved in the cold buffers
 * 
 * returns true if the page was moved,
 * false otherwise
 **/
static bool hot_2_cold(void* buffer) {
    if (!pgi_dirty(buffer)) {
                t_insert(&cold,buffer);
                t_remove(&hot,buffer);
                return true;
    }
    return false;
}

/**
 * Check a cold buffer and eventually moves it to hot
 **/
static void cold_2_hot() {
    void* buffer = t_get_current(&cold);
    if (buffer!=NULL && pgi_dirty(buffer)) {
        t_insert(&hot,buffer);
        t_remove(&cold,buffer);
    }
}

/**
 * removes all the data from the buffers from the pipe and processes them
 **/
static void check_pipes() {
    size_t times;
    //printf("<---<\n");
    for (times=queue_size(&in_out_buffers);times>0;times--) {
        syn_buffer_t new_buffer=queue_dequeue(&in_out_buffers);
        
        char d;
        if (new_buffer.direction==SYN_MONITOR_IN)
            d='<';
        else if (new_buffer.direction==SYN_MONITOR_OUT)
            d='>';
        else
            d='@';
        //printf ("%d + \'%c\'\n",new_buffer.ptr, d);
        
        if (new_buffer.direction==SYN_MONITOR_IN) {
            bool r=t_insert(&hot,new_buffer.ptr);
            if (!r)
                err_fatal("Unable to allocate space for the list of buffers.");
        } else {
            continue;
            bool a=t_remove(&hot,new_buffer.ptr);
            if (!(a || t_remove(&cold,new_buffer.ptr)))
                err_fatal("unexpected free");
            __real_free(new_buffer.ptr);
        }
         
    }
    //printf(">-->\n");
}

/**
 * Inits the monitor thread, and the 
 * pipes used to communicate with it.
 **/
static void ca_init_thread() {
    static pthread_mutex_t cs_mutex = PTHREAD_MUTEX_INITIALIZER;
    static bool initialized=false;
    
    if (initialized) return;
    
    /*
     * All of this is based on the assumption that only
     * pthread_create will trigger a memory allocation
     **/
    if (pthread_mutex_trylock(&cs_mutex)!=0) {
        //another thread is initializing
        //waiting for the other thread to finish and returning
        //pthread_mutex_lock(&cs_mutex);
        //pthread_mutex_unlock(&cs_mutex);
        return;
    }
    
    initialized=true;
    
    if (queue_initializer(&in_out_buffers,MONITOR_PIPE_SIZE) !=0 ) {
        err_fatal("Unable to create IPC structures.");
    }
    
    pthread_attr_t t_attr;
    pthread_t id;
    
    pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);
    
    pthread_create(&id,&t_attr,monitor,(void*)NULL);
    
    pthread_mutex_unlock(&cs_mutex);
    
}

/**
 * Adds a buffer to be monitored
 **/
void ca_monitor_buffer(void* ptr,size_t size) {
    ca_init_thread();
    ca_init(ptr,size);
    syn_buffer_t p={ptr,SYN_MONITOR_IN};
    queue_enqueue(&in_out_buffers,p);
}

/**
 * Removes a buffer from being monitored
 **/
void ca_unmonitor_ptr(void* ptr) {
    syn_buffer_t b={ptr,SYN_MONITOR_OUT};
    queue_enqueue(&in_out_buffers,b);
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
    
    if (!(t_init(&hot,100,200) && t_init(&cold,100,50))) 
        err_fatal("Unable to allocate space for the list of buffers.");
    
    void* buffer;
    unsigned int n=0; //count the iterations
    
    while (true) {
        delay();
        
        n++;
        if (n % (1<<MONITOR_CHECK_PIPES)==0) {
            check_pipes();
        }
        
        
        if (n % (1<<MONITOR_CHECK_COLD)==0) {
            cold_2_hot();
        }

        buffer = t_get_current(&hot);
        if (buffer==NULL) {
            //sleep(MONITOR_EMPTY_SLEEP);
            continue;
        }
        
        //NOTE only reached if there are buffers to monitor
        
        if (hot_2_cold(buffer))
            continue;
        
        //printf("b%p %d %d\n",buffer,t_get_size(&hot),ca_test(buffer));
        if (ca_test(buffer)==0) {
            err_fatal("The canary died! :-(");
        }
        
    }
    
}