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

#ifndef BIT_OP_H
#define BIT_OP_H

#include <stdint.h>

/**
 * Applies a bitmask to a value, shifts the result to the least significant
 * position and returns it.
 * 
 * Counting bits where 0 is the least significant and 63 the most significant,
 * from must always be <=to
 * 
 * normally compilers should optimize this code, inlining it.
 **/
static inline uint64_t bit_apply_mask(uint8_t from, uint8_t to,uint64_t val) {
    return (val &(~(~0 << (to-from+1)) << from)) >> from;
}


#endif