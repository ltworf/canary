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

#include "config.h"
#include "error.h"

#include <syslog.h>
#include <unistd.h>
#include <stdlib.h>

/**
 * Reports a fatal error on log and stderr and terminates
 */
void err_fatal(char* msg) {
    //TODO causing a segfault could be a much faster way to terminate
    
    openlog("canary monitor",LOG_PID | LOG_PERROR,LOG_AUTHPRIV);
    syslog(LOG_CRIT,"%s",msg);
    exit(1);
}