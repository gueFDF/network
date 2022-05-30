#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<strings.h>
#include<ctype.h>
#include<errno.h>
#include<poll.h>
#define SERV_PORT 6666
#define OPEN_MAX 1024
void sys_error(const void*str)
{
    perror(str);
    exit(1);
}
int main()
{
    int i,j;
    int lfd,cfd;
    int nready;
    socklen_t len;
    struct pollfd fds[OPEN_MAX];
    struct sockaddr_in ser_addr,clt_addr;
    int maxfd=-1;
    char buf[BUFSIZ];
    int ret;//检查返回值
    lfd=socket(AF_INET,SOCK_STREAM,0);
    if(lfd==-1)
    sys_error("socket error");
    bzero(&ser_addr,sizeof(ser_addr));
    ser_addr.sin_family=AF_INET;
    ser_addr.sin_port=htons(SERV_PORT);
    ser_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    ret=bind(lfd,(struct sockaddr*)&ser_addr,sizeof(ser_addr));
    if(ret==-1)
    sys_error("bind error");
    ret=listen(lfd,128);
    if(ret==-1)
    sys_error("listen error");
    for(i=0;i<OPEN_MAX;i++)
    fds[i].fd=-1;
    fds[0].fd=lfd;
    fds[0].events=POLLIN;
    maxfd=0;
    while(1)
    {
        nready=poll(fds,maxfd+1,-1);
        if(fds[0].revents&POLLIN)
        {
            len=sizeof(clt_addr);
            cfd=accept(lfd,(struct sockaddr*)&clt_addr,&len);
            if(cfd==-1)
            sys_error("accept error");
            for(i=1;i<OPEN_MAX;i++)
            {
                if(fds[i].fd<0)
                {
                    fds[i].fd=cfd;
                    fds[i].events=POLLIN;
                    printf("client[%d] creat success\n",i);
                    break;
                }
            }
            if(i==OPEN_MAX)
            {
                sys_error("max out");          //超出最大容量
            }
            if(maxfd<i)
            maxfd=i;                           //获取最大下标
            if(--nready<=0)
            continue;
        }
            for(i=1;i<=maxfd;i++)
            {
                int ccfd;
                if((ccfd=fds[i].fd)<0)
                continue;
                if(fds[i].revents&POLLIN)  //客户端写入数据
                {
                    if((ret=read(ccfd,buf,sizeof(buf)))<0) 
                    {
                        if(errno==ECONNRESET)  //收到RST标志
                        {
                            printf("client[%d] aborted connection\n",i);
                            close(ccfd);
                            fds[i].fd=-1;
                        }
                        else
                        sys_error("read error");
                    }
                    else if(ret==0)    //说明客户端申请断开连接
                    {
                            printf("client[%d] aborted connection\n",i);
                            close(ccfd);
                            fds[i].fd=-1;
                    }
                    else
                    {
                        write(1,buf,ret);
                        for(j=0;j<ret;j++)
                        {
                            buf[j]=toupper(buf[j]);
                        }
                        write(ccfd,buf,ret);
                    }
                }

            }

    }
    close(lfd);
    return 0;
}