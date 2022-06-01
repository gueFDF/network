#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include<fcntl.h>
#include <ctype.h>
#include"HEAD.h"
#include <sys/epoll.h>
#define SERV_PORT 6666
#define OPEN_MAX 1024
#define EPOLLLEN 1024
#define MAXLEN 10
void sys_error(const void *str)
{
    perror(str);
    exit(1);
}
int main()
{
    int efd,lfd,cfd;
    int buf[BUFSIZ];
    int i,j;
    socklen_t len;
    int ret;//接受返回值
    struct sockaddr_in sev_addr,clt_addr;
    struct epoll_event temp,events[10];
    lfd=socket(AF_INET,SOCK_STREAM,0);
    int opt;
    setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(int));//允许端口复用
    if(lfd==-1)
    sys_error("socket error");
    bzero(&sev_addr,sizeof(sev_addr));
    sev_addr.sin_family=AF_INET;
    sev_addr.sin_port=htons(SERV_PORT);
    sev_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    ret=bind(lfd,(struct sockaddr*)&sev_addr,sizeof(sev_addr));
    if(ret==-1)
    sys_error("bind error");
    ret=listen(lfd,128);
    if(ret==-1)
    sys_error("listen error");
    efd=epoll_create(10);;//创建监听红黑树
    temp.events=EPOLLIN|EPOLLET;
    len=sizeof(clt_addr);
    cfd=accept(lfd,(struct sockaddr*)&clt_addr,&len);
    printf("accept success\n");
    temp.data.fd=cfd;
    epoll_ctl(efd,EPOLL_CTL_ADD,cfd,&temp);
    //设置非阻塞
    int flag;
    flag=fcntl(cfd,F_GETFL);
    flag|=O_NONBLOCK;
    fcntl(cfd,F_SETFL,flag);
    while(1)
    {
        ret=epoll_wait(efd,events,10,-1);
        if(events[0].data.fd==cfd)
        {
            if((len=read(cfd,buf,MAXLEN/2))>0)
            {
                write(1,buf,len);
            }
        }
    }
    close(lfd);
    close(cfd);
    return 0;
}