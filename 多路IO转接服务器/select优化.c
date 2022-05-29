#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<sys/time.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<string.h>
#include<ctype.h>
#define SERV_PORT 6665
void sys_errno(const char*str)
{
    perror(str);
    exit(1);
}
int main(int argc,char*argv[])
{

    int ret;
    int i,j,n,nready,maxi=-1;
    int client[FD_SETSIZE];//setsize自定义数组client，防止遍历1024个文件描述符，FD_SETSIZE默认为1024
    int maxfd=0;
    int listenfd,connfd;
    socklen_t clie_len;
    char buf[BUFSIZ],str[INET_ADDRSTRLEN];
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
    for(i=0;i<FD_SETSIZE;i++)  //用-1初始化client[];
     client[i]=-1;
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
            if(connfd==-1)
            sys_errno("accrept errno");
            FD_SET(connfd,&allset);
            for(i=0;i<FD_SETSIZE;i++)
            {
                if(client[i]<0)
                {
                    client[i]=connfd;
                    break;
                }
            }
            if(i==FD_SETSIZE)
            {
                fputs("too many clients\n",stderr);           //达到select上限
                exit(1);
            }
            if(maxfd<connfd)
            maxfd=connfd;  //保存最大的文件描述符
            if(maxi<i)
            maxi=i;  //保存最大的文件描述符
            if(0==--nready)   //只有listenfd有事件后续的for不用执行
            continue;
        }
        for(i=0;i<=maxi;i++)   //检测客户端事件
        {
            if(client[i]<0)
            continue;
            if(FD_ISSET(client[i],&rset))
            {
                if((n=read(client[i],buf,sizeof(buf)))==0)//客户端关闭连接，服务器也关闭对应链接
                {
                    close(client[i]);
                    FD_CLR(client[i],&allset);//解除slect对此文件描述符的监听
                    client[i]=-1;
                }
                else if(n>0)
                {
                    for(j=0;j<n;j++)
                    {
                        buf[j]=toupper(buf[j]);
                    }
                    write(client[i],buf,n);
                    write(1,buf,n);
                }
                  if(0==--nready)   //跳出for循环,但还在while中
                 break;
            }
        }
    }


    close(listenfd);
    return  0;
}