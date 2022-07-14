#include<stdio.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<strings.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/un.h>
#include<stddef.h>
#include<ctype.h>
#define SER "serv.socket"
#define CLI "clit.socket"
 
int main()
{
    struct sockaddr_un seraddr,cliaddr;
    int lfd,cfd,ret,i;
    socklen_t len;
    char buf[1024];
    cfd=socket(AF_UNIX,SOCK_STREAM,0);
    bzero(&cliaddr,sizeof(cliaddr));
    cliaddr.sun_family=AF_UNIX;
    strcpy(cliaddr.sun_path,CLI);
    len=offsetof(struct sockaddr_un,sun_path)+strlen(cliaddr.sun_path);
    unlink(CLI);
    ret=bind(cfd,(struct sockaddr*)&cliaddr,len);
    if(ret==-1)
    {
        perror("bind error");
        exit(1);
    }
    //////////////////////////////////////////////
    bzero(&seraddr,sizeof(seraddr));
    seraddr.sun_family=AF_UNIX;
    strcpy(seraddr.sun_path,SER);
    len=offsetof(struct sockaddr_un,sun_path)+strlen(seraddr.sun_path);
    ret=connect(cfd,(struct sockaddr*)&seraddr,len);
    if(ret==-1)
    {
        printf("连接失败\n");
        exit(1);
    }
    printf("连接成功\n");
    while(1)
    {
        ret=read(0,buf,sizeof(buf));
        write(cfd,buf,ret);
        ret=read(cfd,buf,sizeof(buf));
        write(1,buf,ret);
    }

}