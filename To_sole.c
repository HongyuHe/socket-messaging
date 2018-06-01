#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "my_wrap.h"

#define CONTENT 250

void To_sole (char from[], int tofd, int start, char *data)
{
	int i;
	int j;
	char msg[CONTENT] = {0};
	char send[CONTENT] = {0};
	
	for (i = 0, j = start; i < CONTENT; i++, j++)
	{
		if (data[j] != '\n')
		{
			msg[i] = data[j];
		}else
		{
			msg[i] = '\0';
			break;
		}
	}
	
	//printf("start:%d\n", start);
	//printf("SOLE:data %s\n", data);
	//printf("SOLE:msg %s\n", msg);
	sprintf(send, "[私聊]来自 @%s——>: %s\n", from, msg);
    Write (tofd, send, sizeof(send));
    printf("B\n");
}

