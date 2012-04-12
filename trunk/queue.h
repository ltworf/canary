/*
c-pthread-queue - c implementation of a bounded buffer queue using posix threads
Copyright (C) 2008  Matthew Dickinson

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Modified by Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>
*/

#ifndef _QUEUE_H
#define _QUEUE_H


#include "config.h"
#include "types.h"

#define SYN_MONITOR_IN 0
#define SYN_MONITOR_OUT 1

typedef struct {
    void* ptr;
    int direction;
    
} syn_buffer_t;

typedef struct queue
{
        syn_buffer_t *buffer;
        int capacity;
        int size;
        int in;
        int out;
        pthread_mutex_t mutex;
        pthread_cond_t cond_full;
        pthread_cond_t cond_empty;
} queue_t;

int queue_initializer(queue_t *buffer,size_t capacity);
void queue_enqueue(queue_t *queue, syn_buffer_t value);
syn_buffer_t queue_dequeue(queue_t *queue);
int queue_size(queue_t *queue);

#endif
