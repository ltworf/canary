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

static bool realloc_if_needed(tree_t*);
static int t_get_leftmost(tree_t*,int);
static int t_get_rightmost(tree_t*,int);
static int t_get_right_parent(tree_t*,int);
static int t_find(tree_t*,void*);
static void t_delete_index(tree_t*,int);

/**
 * Finds the index of the node containing
 * the value buf
 * 
 * returns -1 if it does not exist.
 **/
static int t_find(tree_t*q,void*buf) {
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
 * r is the index of the node.
 * if r has no left child, r itself is returned
 **/
static int t_get_leftmost(tree_t* q ,int r) {
    
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
 * r is the index of the node.
 * if r has no right child, r itself is returned
 **/
static int t_get_rightmost(tree_t* q ,int r) {
    while (1) {
        if (q->nodes[r].right==-1)  {
            return r;
        } else {
            r=q->nodes[r].right;
        }
    }
}

/**
 * returns false if there are no slots available.
 * 
 * this function tries to allocate more memory in case of shortage,
 * but in case of failure it will try to handle it.
 **/
static bool realloc_if_needed(tree_t* q) {
    if (q->space_left==0) {
        //Try to allocate more space
        void *new_buf = __real_realloc(q->nodes,sizeof(q_node_t)*(q->growth+q->size));
        
        if (new_buf==NULL) //reallocation miserably failed
            return false;
        
        //increase the amount of space left
        q->space_left+=q->growth;
        q->nodes=new_buf;
    }
    return true;
}


/**
 * Initializes the queue with "size" positions already.
 * If the inserts exceed, more memory will be allocated.
 * 
 * size indicates the number of entries initially allocated
 * growth indicates how much extra space is allocated when the 
 * structure becomes full. it must be greater than 0, or unspecified
 * behavior will occur.
 **/
bool t_init(tree_t* q,size_t size,size_t growth) {
    q->size=0;
    q->root=
    q->current=-1;
    q->growth=growth;
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
static void t_delete_index(tree_t*q,int index) {
    
    if (q->size==0) { //Only one node
        q->root=-1;
        q->current=-1;
        return;
    }
    
    int replacement=-1; //Index of the replacement node
    
    //Remove internal nodes
    if (q->nodes[index].left >= 0 && q->nodes[index].right >= 0 ) {
        replacement = t_get_leftmost(q,q->nodes[index].right);
        q->nodes[index].data=q->nodes[replacement].data;
        return t_delete_index(q,replacement);
    }
    
    
    //At this point, we have at least 2 nodes, and it is not an internal node
    
    q_node_t deleted=q->nodes[index]; //Copy of the deleted node
    q->size--;
    q->space_left++;
    
    //Move the last node in place of the deleted node, and update links
    {
        q->nodes[index]=q->nodes[q->size];
        q_node_t *temp=&(q->nodes[q->nodes[index].parent]);
    
        if (temp->left==(int)q->size) {
            temp->left=index;
        } else {
            temp->right=index;
        }
        
        if (q->nodes[index].left>=0) 
            q->nodes[q->nodes[index].left].parent=index;
        if (q->nodes[index].right>=0) 
            q->nodes[q->nodes[index].right].parent=index;
    }
    
    if (deleted.left==-1) {
        //no left subttree, might have right subtree
        replacement = deleted.right;
    } else {
        //no right subtree, might have left subtree
        replacement = deleted.left;
    }
    
    //Replacement of deleted node
    if (deleted.parent>=0) {
        
        if (replacement>=0)
            q->nodes[replacement].parent=deleted.parent;
        
        if (q->nodes[deleted.parent].left==index) {
            q->nodes[deleted.parent].left=replacement;
        } else {
            q->nodes[deleted.parent].right=replacement;
        }
        
    } else {
        //replaced root
        q->nodes[replacement].parent=-1;
        q->root=replacement;
    }
    
}

/**
 * removes an element from the tree
 * 
 * returns false if the removal has failed
 **/
bool t_remove(tree_t* q,void* b) {
    int i=t_find(q,b);
    if (i==-1) return false;
    t_delete_index(q,i);
    return true;
}

/**
 * Appends an element to a tree
 * 
 * returns false if the insertion failed
 **/
bool t_insert(tree_t* q, void* b) {
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
size_t t_get_size(tree_t* q) {
    return q->size;
}

/**
 * returns the id of the first parent located on
 * the right of node i, or -1 if there is no such thing
 **/
static int t_get_right_parent(tree_t* q,int i) {
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
void* t_get_current(tree_t* q) {
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