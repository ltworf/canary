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

#ifndef CA_MONITOR_H
#define CA_MONITOR_H

#include "config.h"
#include "types.h"

/* number of monitor cycles before checking the pipelines for in/out buffer.
 * the number is an exponent, so actually the number is 1<<MONITOR_CHECK_PIPES
 */
#define MONITOR_CHECK_PIPES 6
#define MONITOR_CHECK_COLD 7

/*
 * seconds to wait if there are no buffers to check
 */
#define MONITOR_EMPTY_SLEEP 2

#define MONITOR_DELAY 5

//size of the IPC structure
#define MONITOR_PIPE_SIZE 10


void ca_monitor_buffer(buffer_t buffer);
void ca_unmonitor_ptr(void* ptr);

#endif