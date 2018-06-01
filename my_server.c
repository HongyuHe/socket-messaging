#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "my_wrap.h"

#define SERV_PORT 8000
#define CLI_NUM 20 

#define CMD_LEN 10
#define CONTENT 250

#define REG 1
#define LOG 2
#define LIST 3
#define DELETE 4
#define ALL 5
#define SOLE 6
 
struct data_stream
{
    int cmd[FD_SETSIZE];		//用于标记流,cmd[i]>0:存储着命令号、cmd[i]==0：即将接收命令；(鸽巢原理)
    char data[CONTENT];
};

int main ()
{
    int i;
    int j;
    int num_data;
    int maxfd;
    int sub_max;
    int num_ready;
    int listenfd;
    int connfd;
    int sockfd;
    int tofd;
    int opt = 1;
    int tmp = 0;
    int client[FD_SETSIZE];
    char str[INET_ADDRSTRLEN];
 	char *registry = "./registry.txt";
 	char from[15] = {0};
 	char to[15] = {0};
 	char register_s[] = {"恭喜注册成功！\n已入包！\n"};
    char entry[] = {"\n成功入包咯咯咯～！\n"};
    char register_f[] = {"\n哎呦！巧了！这个用户名前两天刚被注册！\n请退出后重新登录！\n"};
    char entry_f[] = {"\n用户名或密码错误！\n(豆子一直以为自己很笨，不过现在。。。)\n"};
    char arrive[] = {"发送成功！\n"};
    fd_set allset;
    fd_set read_set;
    FILE *fp_registry;
    socklen_t cliaddr_len;
    struct data_stream stream;
    struct sockaddr_in cliaddr;
    struct sockaddr_in servaddr;
    memset(&stream, 0, sizeof(struct data_stream));

    listenfd = Socket (AF_INET, SOCK_STREAM, 0);
    //初始化；
    bzero (&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons (SERV_PORT);

    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));	//客户端退出处理；
    Bind (listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    Listen (listenfd, CLI_NUM);

    maxfd = listenfd;
    sub_max = -1;

    for (i = 0; i < FD_SETSIZE; i++)
    {
        client[i] = -1;
    }

    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    printf("||「@~@」《豆包豆》「@~@」服务器开始运行||\n\n——>正在接收客户端连接请求······\n");
    fp_registry = fopen(registry, "a+");	//打开”注册表“文件；
    fwrite("已注册用户列表：\n", 25, 1, fp_registry);		//初始化“注册表”;
    Fclose (fp_registry);
        
    while (1)
    {
        read_set = allset;

        num_ready = Select (maxfd + 1, &read_set, NULL, NULL, NULL); 
        /*Connection*/
        if (FD_ISSET(listenfd, &read_set))
        {
            cliaddr_len = sizeof(cliaddr);
            connfd = Accept (listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);

            printf("———> 成功与地址为：%s、端口为：%d的客户端建立连接￥～￥\n", inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)), ntohs(cliaddr.sin_port));

            for (i = 0; i < FD_SETSIZE; i++)
            {
                if (client[i] < 0) 
                {
                    client[i] = connfd;
                    break;
                }
            }
            if (i == FD_SETSIZE) 
            {
                fputs("too many clients\n", stderr);
                exit(1);
            }

            FD_SET(connfd, &allset);

            if (connfd > maxfd)
            {
                maxfd = connfd; 
            }
            if (i > sub_max)
            {
                sub_max = i;   
            }

            if (0 == --num_ready)  //第一次进入函数进行连接accept后就退出（第一次调用select的时候nready为1）；
            {
                continue;
            }
        }

        /*Data_stream*/
        for (i = 0; i <= sub_max; i++) 
        {	/* check all clients for data */
            if ( (sockfd = client[i]) < 0)
                continue;

            memset(stream.data, 0, CONTENT);    
            if (FD_ISSET(sockfd, &read_set)) 
            {
                if ( (num_data = Read (sockfd, stream.data, CONTENT)) == 0) 
                {
                    /* connection closed by client */
                    Online_list (NULL, sockfd, fp_registry, NULL, DELETE);		//从在线用户列表删去；
                    Close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                    stream.cmd[i] = 0;		//还原对应命令号；

                } else 
                {
/*有命令就解析并执行*/if (stream.cmd[i])		
                    {
                    printf("$执行命令号：%d\n", stream.cmd[i]);
                    fp_registry = fopen(registry, "a+");	//打开“注册表”；
                    
                        switch (stream.cmd[i])
                        {
                /*注册*/case REG:
                       		printf("%s\n", stream.data);
                            fflush(stdin);
                            if (Online_list (stream.data, sockfd, fp_registry, NULL, REG))
                            {
                            	Write (sockfd, entry, sizeof(entry));
                            }else
                            {
                            	Write (sockfd, register_f, sizeof(register_f));
                            }
                            stream.cmd[i] = 0;		//还原对应命令号；
                            break;
                            
                /*登录*/case LOG:
                        	printf("%s\n", stream.data);
                            fflush(stdin);
                            if (Online_list (stream.data, sockfd, fp_registry, NULL, LOG))
                            {
                            	Write (sockfd, entry, sizeof(entry));
                            }else
                            {
                            	Write (sockfd, entry_f, sizeof(entry_f));
                            }
                            stream.cmd[i] = 0;		//还原对应命令号；
                            break;
                            
                /*群发*/case ALL:
                			printf("Server:%s(All)\n", stream.data);
                			Online_list (NULL, sockfd, NULL, from, ALL);	//找到发送者的用户名；
                			To_all (from, client, sub_max, stream.data);
                			
                			stream.cmd[i] = 0;		//还原对应命令号；
                			bzero(from, sizeof(from));
                			break;
                			
                		case SOLE:
                			printf("Server:%s(Sole)\n", stream.data);
                			for (j = 0; j < 30; j++)
                			{
                				if (stream.data[j] != ';')
                				{
                					to[j] = stream.data[j];
                				}else
                				{
                					to[j] = '\0';
                					break;
                				}
                			}
                			
                			printf("S:to %s\n", to);
                			Online_list (NULL, sockfd, NULL, from, ALL);	//找到发送者的用户名；
                			printf("S:from %s\n", from);
                			tofd = Online_list (NULL, 0, NULL, to, SOLE);	//找到接受者的用户名(to)和fd(tofd)；
                			To_sole (from, tofd, j + 1, stream.data);
                			Write (sockfd, arrive, sizeof(arrive));
                			stream.cmd[i] = 0;		//还原对应命令号；
                			
                			bzero(from, sizeof(from));
                			bzero(to, sizeof(to));
                			stream.cmd[i] = 0;		//还原对应命令号；
                			break;
                		
                            
                        }
                        Fclose (fp_registry);	//千万要现开现关，才能保证数据真正写入成功；
  /*无命令则存储命令*/ }else			
                    {
                        stream.cmd[i] = atoi(stream.data);
                        printf("$设置命令号：%d\n", stream.cmd[i]);
                        
          /*查看在线列表*/if (stream.cmd[i] == LIST)		//LIST命令特殊：只有命令，无数据；
                        {
                         	printf("$执行命令号：%d\n", stream.cmd[i]);
                        	Online_list (NULL, sockfd, fp_registry, NULL, LIST);
                        	stream.cmd[i] = 0;
                        }	                       
                    }
                }

                if (0 == --num_ready)
                {
                    break;
                }
            }
        }
    }
    Fclose (fp_registry);
    return 0;
}
