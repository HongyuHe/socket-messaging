#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "my_wrap.h"

pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

void To_all (char from[], int client[], int sub_max, char *data)
{
	int i;
	int sockfd;
	char msg[300] = {0};

	//pthread_mutex_lock(&counter_mutex);	//上锁;

	for (i = 0; i <= sub_max; i++) 
    {	/* check all clients for data */
     	if ( (sockfd = client[i]) < 0)
     	{
      		continue;
      	}
    	else  
    	{
    		sprintf(msg, "[群消息]来自 @%s——>: %s\n", from, data);
    		//fflush(stdin);
    		Write (sockfd, msg, sizeof(msg));
    		printf("A\n");
    	}
    }  
    
    //pthread_mutex_unlock(&counter_mutex);	//解锁;  
}
