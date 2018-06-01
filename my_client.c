/* my_client.c */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include "my_wrap.h"

#define SERV_PORT 8000

#define CMD_LEN 10
#define CONTENT 250

pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

struct data_stream
{
    char name[CMD_LEN];
    int pass_word[12];
    char cmd[CMD_LEN];
    char data[CONTENT];
};

void receive (void *arg)
{
	int num;
	char data[CONTENT] = {0};
	
	//Set_nonblock (*(int *)arg);

	//pthread_mutex_lock(&counter_mutex);	//上锁;
	while (1)
	{	
		pthread_mutex_lock(&counter_mutex);	//上锁;
		if ((num = Read (*(int *)arg, data, CONTENT)) > 0)		
		{	
			//Write (2, data, num);
			printf("%s\n", data);
		}
		
		//fflush(stdin);
		//printf("\n");
		bzero (data, CONTENT);
		pthread_mutex_unlock(&counter_mutex);	//解锁;			
	}
	//pthread_mutex_unlock(&counter_mutex);	//解锁;
	//pthread_exit((void *)2);
}

int main(int argc, char *argv[])
{
	int flag = 1;
	int i;
	int j;
    int num_data;
    int sockfd;
    int len;
    char pw_check1[12];
    char pw_check2[12];
    char sole[CONTENT] = {0};
    char *split = "\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
    pthread_t   tid;
    void *tret;

    struct sockaddr_in servaddr;
    struct data_stream stream;

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
    servaddr.sin_port = htons(SERV_PORT);

    Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));


    memset(&stream, 0, sizeof(stream));
    printf("%s\n", split);
    printf("\n欢迎来到《豆包豆》智能(只能)聊天室!\n\n我是豆包 咩咩～");
    printf("%s\n", split);
    printf("\n请问您要<注册>(r) 还是 <登录>(l)? \n");
again:
    fgets(stream.cmd, CMD_LEN, stdin); 	//fgets将数据加入两个回车；
    
/*注册*/
    if (0 == strcmp("r\n", stream.cmd))
    {
        Write (sockfd, "1", 1);	

        printf("\n请输入一个10个字节以内的新用户名\n(豆包温馨提示：建议傻一点儿，好记吼～):__________\b\b\b\b\b\b\b\b\b\b");           
        fgets(stream.name, 12, stdin);            

        fflush(stdin);		//???
        printf("\n");
        printf("请设置8～12位密码(数字和字母)\n(豆包暖心忠告：太长了你也记不住 [-.-]):____________\b\b\b\b\b\b\b\b\b\b\b\b");			
        fgets(stream.pass_word, 14, stdin); 

        //处理回车；
        fflush(stdin);
        len = strlen(stream.name);
        stream.name[len - 1] = '\0';
        len = strlen(stream.pass_word);
        stream.pass_word[len - 1] = '\0';

        fflush(stdin);
        sprintf(stream.data, "%s:%s", stream.name, stream.pass_word);	//添加分隔符；
        fflush(stdin);
        printf("注册成功！正在登录中......\n");	
        Write (sockfd, stream.data, CONTENT);	//将数据串发送给服务器； 		
        sleep(1); 
        printf("%s\n", split);
               	         
/*登录*/
    }else if (0 == strcmp("l\n", stream.cmd))
    {
        Write (sockfd, "2", 1);               

        printf("\n请输入您的用户名:__________\b\b\b\b\b\b\b\b\b\b");           
        fgets(stream.name, 12, stdin);            

        fflush(stdin);		//???
        printf("\n");
        printf("请输入密码:____________\b\b\b\b\b\b\b\b\b\b\b\b");			
        fgets(stream.pass_word, 14, stdin); 

        //处理回车；
        fflush(stdin);
        len = strlen(stream.name);
        stream.name[len - 1] = '\0';
        len = strlen(stream.pass_word);
        stream.pass_word[len - 1] = '\0';

        fflush(stdin);
        sprintf(stream.data, "%s:%s", stream.name, stream.pass_word);	
        fflush(stdin);		
        Write (sockfd, stream.data, CONTENT);	//将数据串发送给服务器； 
        sleep(1);	
        printf("%s\n", split);
    }else
    {
        printf("Please try again !(豆包很善良，请别欺负我！)\n");
        goto again;
    }

    if ((num_data = Read (sockfd, stream.data, CONTENT)) > 0)
    {
        Write (1, stream.data, num_data);
    }else
    {
        printf("Entry failure !(豆包不能和你玩了，555～～～）\n");
    }

	pthread_create(&tid, NULL, receive, (void *)&sockfd);		//登录成功后开启接收线程；
   
    while (1)
    {
    	//pthread_create(&tid, NULL, receive, (void *)&sockfd);		//登录成功后开启接收线程；

enter_cmd:    
    	memset(&stream, 0, sizeof(stream));
    	sleep(1);
    	printf("%s\n", split);
    	printf("\n输入命令：______\b\b\b\b\b\b");
    	fgets(stream.cmd, CMD_LEN, stdin);
    	fflush(stdin);
    	
    	/*发送命令*/
    	if (0 == strcmp("list\n", stream.cmd))
    	{
 			Write (sockfd, "3", 1);
 			
 		}else if (0 == strcmp("all\n", stream.cmd))
 		{
 			Write (sockfd, "5", 1);
 			fgets(stream.data, sizeof(stream.data), stdin); 
 			Write (sockfd, stream.data, CONTENT);
 			
 		}else if (0 == strncmp("to", stream.cmd, 2))
 		{
 			Write (sockfd, "6", 1);
 			
 			for (i = 3, j = 0; i < 30; j++, i++)
 			{
 				if (stream.cmd[i] != '\n')
 				{
 					sole[j] = stream.cmd[i];
 				}else
 				{
 					
 					break;
 				}
 			}
 			len = strlen(sole);
 			sole[len] = ';';	//添加分隔符；
 			sole[len + 1] = '\0';
 			
 			fgets(stream.data, sizeof(stream.data), stdin);
 			strcat(sole, stream.data); 
 			Write (sockfd, sole, CONTENT);
 			
 		}else
 		{
 			printf("Please try again !(豆包很善良，请别欺负我！)\n");
 			goto enter_cmd;
 		} 						   			
 	}
 	pthread_join(tid, &tret);
    Close(sockfd);
    
    return 0;
}
