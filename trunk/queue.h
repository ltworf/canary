/*
Weborf
Copyright (C) 2007  Giuseppe Pappalardo

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

@author Giuseppe Pappalardo <pappalardo@dmi.unict.it>
@author Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>
 */

#ifndef WEBORF_QUEUE_H
#define WEBORF_QUEUE_H

#include "config.h"
#include "types.h"

#define SYN_MONITOR_IN 0
#define SYN_MONITOR_OUT 1

typedef struct {
    void* ptr;
    int direction;
    
} syn_buffer_t;

typedef struct {
    size_t num, size;             //Filled positions in the queue, and its maximum size
    int head, tail;               //pointers to head and tail of the round queue
    syn_buffer_t *data;                    //Socket with client
    pthread_mutex_t mutex;        //mutex to modify the queue
    pthread_cond_t for_space, for_data;
    int n_wait_sp, n_wait_dt;
} syn_queue_t;




int q_init(syn_queue_t * q, size_t size);
void q_put(syn_queue_t * q, syn_buffer_t val);
syn_buffer_t q_get(syn_queue_t * q);
void q_free(syn_queue_t * q);

#endif
