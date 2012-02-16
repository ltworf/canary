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

/*
 * This module implements some kind of linked list.
 * It is NOT meant to be used by multiple threads and
 * does not provide any kind of syncronization.
 * 
 * The insertion is performed in a sorted way
 */

#include "ca_queue.h"
#include "types.h"
#include "ca_alloc.h"

static bool realloc_if_needed(queue_t*);
static bool realloc_if_needed(queue_t* q) {
    //TODO make a realloc if needed
    if (q->space_left==0) return false;
    return true;
}

/**
 * Initializes the queue with "size" positions already.
 * If the inserts exceed, more memory will be allocated.
 * 
 * size indicates the number of entries initially allocated
 **/
bool q_init(queue_t* q,size_t size) {
    q->size=0;
    q->head=
    q->tail=
    q->current=-1;
    q->nodes=__real_malloc(sizeof(q_node_t)*size);
    q->space_left=size;
    
    if (q->nodes == NULL) return false;
    return true;
}

/**
 * Appends an element to a list
 * 
 * returns false if the insertion failed
 **/
bool q_append(queue_t* q, buffer_t b) {
    if (!realloc_if_needed(q)) return false;
    
    q_node_t node;
    node.data=b;
    
    if (q->size>0) {
        q_node_t t=q->nodes[q->tail];
        
        t.next=q->size;
        node.prev=q->tail;
        node.next=-1;
        q->nodes[q->size]=node;
        
        
    } else { //Initial element
        node.next=
        node.prev=-1;
        
        q->nodes[0]=node;
        q->current=
        q->head=0;
        
    }
    
    q->tail=q->size;
    q->size++;
    q->space_left--;
    
    return true;
}

/*
 * typedef struct {
    buffer_t data;
    int prev;
    int next;
} q_node_t;

typedef struct {
    int size;
    int head;
    int tail;
    int current;
    q_node_t nodes[];
    size_t space_left;
} queue_t;
*/

/*
bool q_insert(queue_t* q,buffer_t b) {
    node_t* n = (sizeof(node_t));
    
    n->data = b;
    //TODO find its place
}


bool q_delete_current(queue_t* q) {
    
}
bool q_delete_ptr(queue_t*,void*);
*/

/**
 * returns the amount of elements present
 * in the queue.
 **/
size_t q_get_size(queue_t* q) {
    return q->size;
}



