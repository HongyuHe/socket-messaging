void perr_exit (const char *s);

int Accept (int fd, struct sockaddr *sa, socklen_t *salenptr);

void Bind (int fd, const struct sockaddr *sa, socklen_t salen);

void Connect (int fd, const struct sockaddr *sa, socklen_t salen);

void Listen (int fd, int backlog);

int Socket (int family, int type, int protocol);

int Select (int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

void Add_client (int client, int connfd);

ssize_t Read (int fd, void *ptr, size_t nbytes);

ssize_t Write (int fd, const void *ptr, size_t nbytes);

void Close (int fd);

ssize_t Readn (int fd, void *vptr, size_t n);

ssize_t Writen (int fd, const void *vptr, size_t n);

static ssize_t Read_char (int fd, char *ptr);

ssize_t Read_line (int fd, void *vptr, size_t maxlen);

FILE *Fopen (const char *path, const char *mode);

size_t Fread (void *ptr, size_t size, size_t nmemb, FILE *stream);

size_t Fwrite (const void *ptr, size_t size, size_t nmemb, FILE *stream);

void Fclose (FILE *fp);

int Online_list (char *data, int sockfd, FILE *fp, char *id, int cmd);

void To_all (char from[], int client[], int sub_max, char *data);

void To_sole (char from[], int tofd, int start, char *data);







