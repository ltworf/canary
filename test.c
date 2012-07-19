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

#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "types.h"

#include "ca_canary.h"
#include "bit_op.h"
#include "ca_pageinfo.h"
#include "ca_tree.h"

int main () {
    int c;
    char *a=malloc(200);
    //char *a=calloc(100,2);
    //a=realloc(a,200);
    
   for ( c=0;c<200;c++) {
        a[c]=c;
        malloc(12);
        malloc(12);
        malloc(12);
    }
    
    printf("all good so far\n");
    //a=realloc(a,200);
    sleep(3);
    free(malloc(1441));
    printf("BAD!!\n");
    memset(a,0,218);
    sleep(3);
    printf("this should not be printed\n");
    
}

