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

#include <linux/kernel-page-flags.h>

#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "types.h"

#include "ca_queue.h"
#include "ca_canary.h"
#include "bit_op.h"
#include "ca_pageinfo.h"

int main () {
    pgi_init();
    //pgi_dirty(calloc(1,1));
    pgi_dirty(printf);
    exit(0);
    
    //read_ptr_info( pid,malloc(400));
    //uint64_t q=4000;
    //printf("%lu\n",bit_apply_mask(5,5,q));
    printf("ciao\n");
    int *a=malloc(400);
    //int *b = calloc(14, sizeof(int));

    //ca_init(a,400);
        
    printf("\n\na[0]=%d\n",a[0]);
    //a[0]=123;
    printf("a[0]=%d\n",a[0]);
    
    //__init();
    
    printf("all good so far\n");
    sleep(3);
    printf("BAD!!\n");
    memset(a,0,401);
    sleep(3);
    
    
    
    //printf("===\nresult: %d\n", ca_test(a,400));
    
    //if (ca_test(a,400)==true) printf ("OK\n");
    
    /*queue_t hot;
    q_init(&hot,400);
        
    buffer_t b;
    
    printf("size=%d current=%d\n",hot.size,hot.current);
    for (int i=0;i<200;i++) {
        b.size=i;
        q_append(&hot,b);
        printf("%d tail=%d head=%d\n",b.size,hot.tail,hot.head);
    }
    
    
    printf("%d\n",hot.nodes[4].next);
    printf("\n====%d \n",q_get_size(&hot));
    
    for (int i=0;i<210;i++) {
        b=q_get_current(&hot);
        
        printf("%d ",b.size);
    }
    
    q_delete_previous(&hot);
    
    printf("\n====%d \n",q_get_size(&hot));
    
    
    for (int i=0;i<210;i++) {
        b=q_get_current(&hot);
        
        printf("%d ",b.size);
    }
    printf("%d tail=%d head=%d\n",b.size,hot.tail,hot.head);
    */
}


