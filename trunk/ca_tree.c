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
static int t_get_leftmost(queue_t*,int);
static int t_get_rightmost(queue_t*,int);
static int t_get_left_parent(queue_t*,int);
static int t_find(queue_t*,void*);

static int t_find(queue_t*q,void*buf) {
    int r=q->root;
    
    while (1) {
        if (r==-1) {
            return -1;
        } else if (q->nodes[r].data < buf) {
            r=q->nodes[r].right;
        } else if (q->nodes[r].data > buf) { 
            r=q->nodes[r].left;
        } else {
            return r;
        }
    }
    
}

/**
 * returns the index of the leftmost node from r
 **/
static int t_get_leftmost(queue_t* q ,int r) {
    
    while (1) {
        if (q->nodes[r].left==-1)  {
            return r;
        } else {
            r=q->nodes[r].left;
        }
    }
}

/**
 * returns the index of the rightmost node from r
 **/
static int t_get_rightmost(queue_t* q ,int r) {
    while (1) {
        if (q->nodes[r].right==-1)  {
            return r;
        } else {
            r=q->nodes[r].right;
        }
    }
}

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
 * removes the node at position index
 * 
 * WARNING: no checks on the size/existence are performed
 **/
static bool q_delete_index(queue_t*q,int index) {
    
    q_node_t deleted=q->nodes[index];
    q->size--;
    q->space_left++;
    
    if (q->size==0) { //Deleting last node
        q->root=-1;
        q->current=-1;
        return true;
    }
    
    //Move the last node in place of the deleted node, and update links
    {
        q->nodes[index]=q->nodes[q->size];
        q_node_t *temp=&(q->nodes[q->nodes[index].parent]);
    
        if (temp->left==(int)q->size) {
            temp->left=index;
        } else {
            temp->right=index;
        }
    }
    
    //Deletion of the node
    if (index==q->root) {
        //TODO
    }
    
    if (deleted.left==-1) {
        if (deleted.right==-1) {
            //deleting leaf
            if (q->nodes[deleted.parent].left==index) {
                q->nodes[deleted.parent].left=-1;
            } else {
                q->nodes[deleted.parent].right=-1;
            }
        } else {
            //no left subtree, has right subtree
            if (q->nodes[deleted.parent].left==index) {
                q->nodes[deleted.parent].left=deleted.right;
            } else {
                q->nodes[deleted.parent].right=deleted.right;
            }
            
            q->nodes[deleted.right].parent = deleted.parent;
        }
    } else {
        if (deleted.right==-1) {
            //only has left subtree
            if (q->nodes[deleted.parent].left==index) {
                q->nodes[deleted.parent].left=deleted.left;
            } else {
                q->nodes[deleted.parent].right=deleted.left;
            }
            q->nodes[deleted.left].parent = deleted.parent;
            
        } else {
            //has both subtrees, must do a very ugly thing
        }
    }
    
}

/**
 * removes an element from the tree
 * 
 * returns false if the removal has failed
 **/
bool q_remove(queue_t* q,void* b) {
    int i=t_find(q,b);
    if (i==-1) return false;
    
    q_delete_index(q,i);
        //TODO
    return true;
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
    node->left=-1;
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
        
        q->current=0;
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
 * returns the id of the first parent located on
 * the right of node i, or -1 if there is no such thing
 **/
static int t_get_right_parent(queue_t* q,int i) {
    int p;
    
    while (1) {
        p=q->nodes[i].parent;
        if (p==-1) {
            return -1;
        } else if (q->nodes[p].left==i) {
            return p;
        } else {
          i=p;
        }
    }
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
    //printf("current:%d\n",q->current);
    if (q->size==0) {
        return NULL;
    }
    int current=q->current;
    void* ret=q->nodes[current].data;
    
    if (q->nodes[current].right!=-1) {
        q->current=t_get_leftmost(q,q->nodes[current].right);
    } else {
        q->current=t_get_right_parent(q,current);
    }
        
    
    //Resets the cycle when it ends
    if (q->current==-1) {
        q->current=t_get_leftmost(q,q->root);
    }
    
    return ret;
}