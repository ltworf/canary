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
 * This module implements some kind of BST.
 * It is NOT meant to be used by multiple threads and
 * does not provide any kind of syncronization.
 * 
 */

#include "ca_tree.h"
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
    q->root=
    q->current=-1;
    q->nodes=__real_malloc(sizeof(q_node_t)*size);
    q->space_left=size;
    
    if (q->nodes == NULL) {
        return false;
    }
    return true;
}

/**
 * removes an element from the tree
 * 
 * returns false if the removal has failed
 **/
bool q_remove(queue_t*,void*) {
        //TODO
}

/**
 * Appends an element to a tree
 * 
 * returns false if the insertion failed
 **/
bool q_insert(queue_t* q, void* b) {
    if (!realloc_if_needed(q)) return false;
    
    q_node_t *node;
    node= &(q->nodes[q->size]);
    node->data=b;
    node->left=
    node->right=-1;
    
    if (q->size>0) {
        int root=q->root;
        
        for (;;) {
            if (q->nodes[root].data<b) {
                if (q->nodes[root].right==-1) {
                    q->nodes[root].right=q->size;
                    node->parent=root;
                    break;
                } else {
                    root=q->nodes[root].right;
                }
            } else {
                if (q->nodes[root].left==-1) {
                    q->nodes[root].left=q->size;
                    node->parent=root;
                    break;
                } else {
                    root=q->nodes[root].left;
                }
            }
        }
        
    } else { //root element
        
        node->parent=-1;
        
        q->current=
        q->root=0;
    }
    
    
    q->size++;
    q->space_left--;
    
    return true;
}

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
}