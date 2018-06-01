#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

void perr_exit (const char *s)
{
    perror(s);
    exit(1);
}

int Accept (int fd, struct sockaddr *sa, socklen_t *salenptr)
{
    int n;

again:
    if ( (n = accept(fd, sa, salenptr)) < 0)
    {
        if ((errno == ECONNABORTED) || (errno == EINTR))	//链接放弃或被打断，重新链接；
            goto again;
        else
            perr_exit("accept error");
    }
    return n;
}

void Bind (int fd, const struct sockaddr *sa, socklen_t salen)
{
    if (bind(fd, sa, salen) < 0)
        perr_exit("bind error");
}

void Connect (int fd, const struct sockaddr *sa, socklen_t salen)
{
    if (connect(fd, sa, salen) < 0)
        perr_exit("connect error");
}

void Listen (int fd, int backlog)
{
    if (listen(fd, backlog) < 0)
        perr_exit("listen error");
}

int Socket (int family, int type, int protocol)
{
    int n;

    if ( (n = socket(family, type, protocol)) < 0)
        perr_exit("socket error");
    return n;
}

int Select (int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
    if (select(n, readfds, writefds, exceptfds, timeout) < 0)
    {
        perr_exit ("select error:");
    }
}

void Add_client (int client[], int connfd)
{
    int i;

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
        fputs("Sorry,there are too many clients. Please reboot server!\n", stderr);    //客户端数目达到（client[]）上限；
        exit(1);
    }
}

ssize_t Read (int fd, void *ptr, size_t nbytes)		//用于文本传输；
{
    ssize_t n;

again:
    if ( (n = read(fd, ptr, nbytes)) == -1)
    {
        if (errno == EINTR)
            goto again;
        else
            return -1;
    }
    return n;
}

ssize_t Write (int fd, const void *ptr, size_t nbytes)	//用于文本传输
{
    ssize_t n;

again:
    if ( (n = write(fd, ptr, nbytes)) == -1) {
        if (errno == EINTR)
            goto again;
        else
            return -1;
    }
    return n;
}

void Close (int fd)
{
    if (close(fd) == -1)
        perr_exit("close error");
}

//解决由于网络缓冲机构的特性造成的read不能确保返回指定读取的字节数；（vptr：读取的缓冲区）；
ssize_t Readn (int fd, void *vptr, size_t n)	//用于网络传输；
{
    size_t  nleft;
    ssize_t nread;
    char   *ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0)
    {
        if ( (nread = read(fd, ptr, nleft)) < 0)
        {		
            if (errno == EINTR)
                nread = 0;
            else
                return -1;
        } else if (nread == 0)	//中间放弃或已经读取完毕；
            break;

        nleft -= nread;
        ptr += nread;
    }
    return n - nleft;	//返回实际读取的字节数；
}

ssize_t Writen (int fd, const void *vptr, size_t n)	//用于网络传输；
{
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0) 
    {
        if ( (nwritten = write(fd, ptr, nleft)) <= 0) 
        {
            if (nwritten < 0 && errno == EINTR)
            {   
            	nwritten = 0;
            }
            else
            {
                return -1;
            }
	}
        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}

//缓冲读取原理 以减少切换kernal的次数（只读取一个字节）；
static ssize_t Read_char (int fd, char *ptr)	//只读取一个字节；
{
    static int read_cnt = 0;
    static char *read_ptr;
    static char read_buf[100];

    if (read_cnt <= 0)
    {
again:
        if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0)
        {
            if (errno == EINTR)
            {
                goto again;
            }
            return -1;
        }
        else if (read_cnt == 0)
        {
            return 0;
        }
        read_ptr = read_buf;
    }
    read_cnt--;
    *ptr = *read_ptr++;
    return 1;
}

ssize_t Read_line (int fd, void *vptr, size_t maxlen)	//用于读取一行；
{
    ssize_t n, rc;
    char    c, *ptr;

    ptr = vptr;
    for (n = 1; n < maxlen; n++)
    {
        if ( (rc = Read_char (fd, &c)) == 1)
        {
            *ptr++ = c;
            if (c  == '\n')
                break;
        } else if (rc == 0)
        {
            *ptr = 0;
            return n - 1;
        } else
            return -1;
    }
    *ptr  = 0;
    return n;
}

FILE *Fopen (const char *path, const char *mode)
{
	FILE *fp;

	if ( NULL == (fp = fopen(path, mode)) )
	{	
		perror("error open file");	
		exit(1);
	}else
	{
		return fp;	
	}	
}
/*
size_t Fread (void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t num = fread((void *)ptr, size, nmemb, stream);	//通用指针在使用时一定要确定类型，即强制类型转换；

	if (num == size)
	{
		return num;
	}else
	{
		ferror("Fread: ");
	}
}

size_t Fwrite (const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t num = fwrite((void *)ptr, size, nmemb, stream);		//通用指针在使用时一定要确定类型，即强制类型转换；

	if (num == size)
	{
		return num;
	}else
	{
		ferror("Fwrite: ");
	}
}*/

void Fclose (FILE *fp)
{
	if (fclose(fp))
	{
		perror("Fclose :");
	}
}

int Set_nonblock (int fd) 
{
  int old_option = fcntl(fd, F_GETFL);
  int new_option = old_option | O_NONBLOCK;
  
  fcntl(fd, F_SETFL, new_option);
  
  return old_option;
} 




