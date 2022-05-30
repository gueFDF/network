#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/epoll.h>
#define SERV_PORT 6666
#define OPEN_MAX 1024
#define EPOLLLEN 1024
void sys_error(const void *str)
{
    perror(str);
    exit(1);
}
int main()
{
    struct sockaddr_in sev_addr, clit_addr;
    int i, j;
    int lfd, cfd;
    char buf[BUFSIZ];
    socklen_t len;
    int ret, ret1;
    lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
        sys_error("soclet error");
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)); //端口复用
    bzero(&sev_addr, sizeof(sev_addr));
    sev_addr.sin_family = AF_INET;
    sev_addr.sin_port = htons(SERV_PORT);
    sev_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    ret = bind(lfd, (struct sockaddr *)&sev_addr, sizeof(sev_addr));
    if (ret == -1)
        sys_error("bind error");
    ret = listen(lfd, 128);
    int epfd = epoll_create(EPOLLLEN);
    struct epoll_event temp, ep[1024];
    temp.data.fd = lfd;
    temp.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &temp); //将监听套接字的文件描述符传进监听红黑树
    while (1)
    {
        ret1 = epoll_wait(epfd, ep, 1024, -1);
        if (ret1 == -1)
            sys_error("epoll error");
        for (i = 0; i < ret1; i++)
        {
            if (ep[i].data.fd == lfd) //有客户端请求链接
            {
                len = sizeof(clit_addr);
                cfd = accept(lfd, (struct sockaddr *)&clit_addr, &len);
                if (cfd == -1)
                    sys_error("accept error");
                printf("client[%d] creat success\n", cfd);
                temp.data.fd = cfd;
                temp.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &temp);
            }
            else
            {
                int ccfd = ep[i].data.fd;
                if ((ret = read(ccfd, buf, sizeof(buf))) < 0)
                {
                    if (errno == ECONNRESET)
                    {
                        printf("client[%d] aborted connection\n", ccfd);
                        temp.data.fd = ccfd;
                        temp.events = EPOLLIN;
                        epoll_ctl(epfd, EPOLL_CTL_DEL, ccfd, &temp);
                    }
                    else
                        sys_error("read error");
                }
                else if (ret == 0) //发送FIN信号
                {
                    printf("client[%d] aborted connection\n", ccfd);
                    temp.data.fd = ccfd;
                    temp.events = EPOLLIN;
                    epoll_ctl(epfd, EPOLL_CTL_DEL, ccfd, &temp);
                }
                else
                {
                    write(1, buf, ret);
                    for (j = 0; j < ret; j++)
                        buf[j] = toupper(buf[j]);
                    write(ccfd, buf, ret);
                }
            }
        }
    }
    close(lfd);
    return 0;
}