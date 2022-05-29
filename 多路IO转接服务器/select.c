#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<sys/time.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<string.h>
#include<ctype.h>
#define SERV_PORT 6666
void sys_errno(const char*str)
{
    perror(str);
    exit(1);
}
int main()
{
    int ret;
    int i,j,n,nready;
    int maxfd=0;
    int listenfd,connfd;
    socklen_t clie_len;
    char buf[BUFSIZ];
    listenfd=socket(AF_INET,SOCK_STREAM,0);
    if(listenfd==-1)
    sys_errno("socket errno");
    int opt=1;
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));//允许端口复用-
    struct sockaddr_in clie_addr,serv_addr;
    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(SERV_PORT);
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    ret=bind(listenfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    if(ret==-1)
    sys_errno("bind errno");
    ret=listen(listenfd,128);
    if(ret==-1)
    sys_errno("listenfd errno");
    fd_set rset,allset;  //rest读事件文件描述符集合，allset用来暂存
    maxfd=listenfd;
    FD_ZERO(&allset);//清空
    FD_SET(listenfd,&allset);
    while(1)
    {
        rset=allset;
        nready=select(maxfd+1,&rset,NULL,NULL,NULL);
        if(nready<0)
        sys_errno("select errno");
        if(FD_ISSET(listenfd,&rset))
        {
            clie_len=sizeof(clie_addr);
            connfd=accept(listenfd,(struct sockaddr*)&clie_addr,&clie_len);//accept必定不会阻塞
            FD_SET(connfd,&allset);
            if(maxfd<connfd)
            maxfd=connfd;  //保存最大的文件描述符
            if(0==--nready)   //只有listenfd有事件后续的for不用执行
            continue;
        }
        for(i=listenfd+1;i<=maxfd;i++)   //检测客户端事件
        {
            if(FD_ISSET(i,&rset))
            {
                if((n=read(i,buf,sizeof(buf)))==0)//客户端关闭连接，服务器也关闭对应链接
                {
                    close(i);
                    FD_CLR(i,&allset);//解除slect对此文件描述符的监听
                }
                else if(n>0)
                {
                    for(j=0;j<n;j++)
                    {
                        buf[j]=toupper(buf[j]);
                    }
                    write(i,buf,n);
                }

            }
        }
    }



    return  0;
}