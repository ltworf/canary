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

#include <pthread.h>
#include <stdio.h>

#include "queue.h"
#include "ca_alloc.h"



/**
 * returns -1 in case of failure
 **/
int queue_initializer(queue_t *buffer,size_t capacity) { 
    
    buffer->buffer = __real_malloc(capacity * sizeof (syn_buffer_t));
    buffer->capacity = capacity;
    buffer->size=0;
    buffer->in=0;
    buffer->out=0;
    
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond_full=PTHREAD_COND_INITIALIZER;
    pthread_cond_t cond_empty=PTHREAD_COND_INITIALIZER;
    
    buffer->mutex=mutex;
    buffer->cond_empty = cond_empty;
    buffer->cond_full = cond_full;
    
    return buffer->buffer == NULL ? -1 : 0;    
}



void queue_enqueue(queue_t *queue, syn_buffer_t value)
{
        pthread_mutex_lock(&(queue->mutex));
        while (queue->size == queue->capacity)
                pthread_cond_wait(&(queue->cond_full), &(queue->mutex));
        //printf("enqueue %d\n", *(int *)value);
        queue->buffer[queue->in] = value;
        ++ queue->size;
        ++ queue->in;
        queue->in %= queue->capacity;
        pthread_mutex_unlock(&(queue->mutex));
        pthread_cond_broadcast(&(queue->cond_empty));
}

syn_buffer_t queue_dequeue(queue_t *queue)
{
        pthread_mutex_lock(&(queue->mutex));
        while (queue->size == 0)
                pthread_cond_wait(&(queue->cond_empty), &(queue->mutex));
        syn_buffer_t value = queue->buffer[queue->out];
        //printf("dequeue %d\n", *(int *)value);
        -- queue->size;
        ++ queue->out;
        queue->out %= queue->capacity;
        pthread_mutex_unlock(&(queue->mutex));
        pthread_cond_broadcast(&(queue->cond_full));
        return value;
}

int queue_size(queue_t *queue)
{
        pthread_mutex_lock(&(queue->mutex));
        int size = queue->size;
        pthread_mutex_unlock(&(queue->mutex));
        return size;
}
