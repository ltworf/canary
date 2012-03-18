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
    a=realloc(a,200);
    
    for ( c=0;c<200;c++) {
        a[c]=c;
        //malloc(12);
    }
    
    printf("all good so far\n");
    //a=realloc(a,200);
    sleep(3);
    free(malloc(1441));
    printf("BAD!!\n");
    memset(a,0,218);
    sleep(3);
    
}

static int qmain () {
    queue_t q;
    t_init(&q,50,10);
    
    int i;
    for  (i=1;i<4;i++) {
        q_insert(&q,1000+i);
        q_insert(&q,1000-i);
    }
    
    for (i=0;i<10;i++) {
        printf("%d ",q_get_current(&q));
    }
    
    printf("====\n");
    
    int n=2;
    
    //exit(0);
    
    for  (i=1000;i<1010;i++) {
        
        if (i%2==1) {
            printf("%d--->root=%d\tpar=%d l=%d r=%d d=%d\n",n,
           q.root,
           q.nodes[n].parent,
           q.nodes[n].left,
           q.nodes[n].right,
           q.nodes[n].data
          );
            
            printf("remove %d\n",i);
            q_remove(&q,i);
        
        
        
        }
    }
    
    for (i=0;i<200;i++) {
        printf("%d ",q_get_current(&q));
    }
    
}

