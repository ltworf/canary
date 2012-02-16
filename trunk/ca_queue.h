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
    buffer_t data;
    int prev;
    int next;
} q_node_t;

typedef struct {
    size_t size;
    int head;
    int tail;
    int current;
    q_node_t *nodes;
    size_t space_left;
} queue_t;

bool q_init(queue_t*,size_t size);
size_t q_get_size(queue_t*);
buffer_t q_get_current(queue_t*);
bool q_delete_previous(queue_t*);
bool q_append(queue_t*, buffer_t);

//bool q_insert(queue_t*,buffer_t);

//bool q_delete_ptr(queue_t*,void*);


#endif