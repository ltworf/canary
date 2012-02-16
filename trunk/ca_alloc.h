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

#ifndef CA_ALLOC_H
#define CA_ALLOC_H

void* malloc(size_t size);
void* __real_malloc(size_t size);
void __real_free(void* ptr);
void* __real_realloc(void *ptr,size_t size);

void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
//void *realloc(void *ptr, size_t size);

#endif