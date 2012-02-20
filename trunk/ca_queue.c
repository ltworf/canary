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
    
    if (q->nodes == NULL) {
        return false;
    }
    return true;
}

/**
 * Appends an element to a list
 * 
 * returns false if the insertion failed
 **/
bool q_append(queue_t* q, void* b) {
    if (!realloc_if_needed(q)) return false;
    
    q_node_t node;
    node.data=b;
    
    if (q->size>0) {
        
        q->nodes[q->tail].next=q->size;
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
    void* data;
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
bool q_insert(queue_t* q,void* b) {
    node_t* n = (sizeof(node_t));
    
    n->data = b;
    //TODO find its place
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

/**
 * Returns the "current" void*.
 * this function is used to iterate over
 * the data structure. The iteration
 * never ends.
 * 
 * If there are no elements to return,
 * void*.prt will be equal to NULL.
 * 
 **/
void* q_get_current(queue_t* q) {
    void* ret;
    if (q->current>=0) {
        q_node_t n = q->nodes[q->current];
        ret = n.data;

        q->current = n.next!=-1? n.next : q->head;
    } else {
        return NULL;
    }
    return ret;
}

/**
 * Deletes the node returned by the last
 * q_get_current
 **/
bool q_delete_previous(queue_t* q) {
    if (q->current>=0) {
        
        //index of the previous current node
        int prev= q->nodes[q->current].prev != -1 ? q->nodes[q->current].prev: q->tail;
        
        q_node_t prev_node=q->nodes[prev]; //copy of the previus node
        
        q->size--;
        q->space_left++;
        
        if (q->size==0) {
            q->current=-1;
            return true;
        }
        
        //moves the last node to the position of the deleted node
        q->nodes[prev]=q->nodes[q->size];
        q->nodes[q->nodes[prev].prev].next = prev;
        q->nodes[q->nodes[prev].next].prev = prev;
        
        if (prev_node.next==-1) { //tail
            q->tail = prev_node.prev;
            q->nodes[prev_node.prev].next=-1;
        }
        
        if (prev_node.prev==-1) { //head
            q->head = prev_node.next;
            q->nodes[prev_node.next].prev=-1;
        }
        
        //TODO must check if the compiler can optimize these redoundant conditions
        if (prev_node.prev>=0 && prev_node.next>=0) { //Normal node
            q->nodes[prev_node.next].prev=prev_node.prev;
            q->nodes[prev_node.prev].next=prev_node.next;
        }
        
    } else {
        return false;
    }
    return true;
}