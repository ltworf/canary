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

#ifndef CA_QUEUE_H
#define CA_QUEUE_H

#include "config.h"
#include "types.h"

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    void* data;
    int left;
    int right;
    int parent;
} q_node_t;

typedef struct {
    size_t size;
    int root;
    int current;
    size_t growth;
    q_node_t *nodes;
    size_t space_left;
} tree_t;

bool t_init(tree_t*,size_t,size_t);
size_t t_get_size(tree_t*);
bool t_insert(tree_t*, void*);
bool t_remove(tree_t*,void*);
void* t_get_current(tree_t* q);


#endif