#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include "my_wrap.h"

#define REG 1
#define LOG 2
#define LIST 3
#define DELETE 4
#define ALL 5
#define SOLE 6

typedef struct node 
{
    char name[11];
    char pw[13];
    int fd;
    struct node *next;
}node;

node sentry = {0, 0};
node *head = &sentry;

node *make_node (char *name, char *pass_word, int sockfd)
{
    node *p = malloc (sizeof (node));

    if (NULL == p)
    {
        printf("Malloc failed !");
        exit(1);

    }
    strcpy(p->name, name);
    strcpy(p->pw, pass_word);
    p->fd = sockfd;
    p->next = NULL;

}

void head_insert (node *p)
{
    p->next = head->next;
    head->next = p;

}

node *search_node (int sockfd, char id[], int cmd)
{
    node *p = head->next;

	switch (cmd)
	{
	case ALL:
	case DELETE:	
    	while (p != NULL)
    	{
        	if (p->fd == sockfd)
       		{
        		return p;
        	}
        	p = p->next;
    	}
    	break;
    	
    case SOLE:
    	printf("id:%s#\n", id);
    	while (p != NULL)
    	{
        	if (!strcmp(id, p->name))
       		{
        		return p;
        	}
        	p = p->next;
    	}
    	printf("A和%s私聊\n", p->name);
    	break;
    }
    return NULL;
}

void traverse_node (char list[], int sockfd)
{
    node *pre = head;
    char tmp[10] = {0};

    while (pre->next != NULL)
    {
    	sprintf(tmp, "%s ", pre->next->name);
        strcat(list, tmp);
        pre = pre->next;
        bzero(tmp, 0);
    }
    printf("O:%s\n", list);
    Write (sockfd, list, sizeof(list));
    memset(list, 0, sizeof(list));
}

void remove_node (node *p)
{
    node *pre = head;

    while (pre->next != NULL)
    {
        if (pre->next == p)
        {
            pre->next = p->next;
            p->next = NULL;

            return;
        }
        pre = pre->next;
    }
}

int Online_list (char *data, int sockfd, FILE *fp, char *id, int cmd)
{
    node *p;
    int i; 
    int j;
    char name[11] = {0};
    char list[100] = {0};
    char pass_word[13] = {0};
    char registry[300] = {0};
    char srch_name[50] = {0};
    char srch_pw[50] = {0};
    char input[30] = {0};

	if ((cmd == LIST) || (cmd == DELETE) || (cmd == ALL) || (cmd == SOLE))
	{
		goto cmd;
	}
	
	fread(registry, 300, 1, fp);	//将“注册表”文件内容全部读入数组，便于查找；
	
    //解包；
    for (i = 0; i < 30; i++)
    {
        if (*(data + i) != ':')
        {
            name[i] = *(data + i);
        }else
        {
            name[i] = '\0';
            i++;
            break;
        }
    }

    for (j = 0; j < 30; i++, j++)
    {
        if (*(data + i) != '\0')
        {
            pass_word[j] = *(data + i);
        }else
        {
            pass_word[j] = '\0';
            break;
        }
    }

    	sprintf(srch_name, "0x4z75%s", name);
    	sprintf(srch_pw, "0x45h7%s", pass_word);
//    	printf("%s\n", srch_name);
//    	printf("%s\n", srch_pw);

cmd:
    switch (cmd)
    {
    case REG:
        if (!strstr(registry, srch_name))
        {
            p = make_node (name, pass_word, sockfd);
            head_insert (p);		//插入在线列表；
            sprintf(input, "%s: %s\n", srch_name, srch_pw);
            printf("%d\n", strlen(input));
            fwrite(input, strlen(input), 1, fp);
            fflush(stdin);
            return 1;

        }else
        {
            return 0;
        }

    case LOG:
        if ( (strstr(registry, srch_name) && (strstr(registry, srch_pw))) )
        {
            p = make_node (name, pass_word, sockfd);
            head_insert (p);		//插入在线列表；
            return 1;

        }else
        {
            return 0;
        }
        
    case LIST:
    	traverse_node (list, sockfd);
    	break;	

    case DELETE:
        p = search_node (sockfd, NULL, DELETE);
        printf("X××...一个用户已下线！\n");
    	remove_node (p);
    	free(p);
    	break;
    	
    case ALL:
    	p = search_node (sockfd, NULL, ALL);
    	strcpy(id, p->name);
    	//printf("%s发送了[群消息]！\n", id);
    	break;	
    	
    case SOLE:
    	p = search_node (0, id, SOLE);
    	printf("和%s私聊\n", p->name);
    	return p->fd;
    	break;
    }
	
    return 0;
}
