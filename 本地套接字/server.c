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
    char buf[1024];
    int i,lfd,cfd,ret;
    socklen_t len;
    lfd=socket(AF_UNIX,SOCK_STREAM,0);
    bzero(&seraddr,sizeof(seraddr));
    seraddr.sun_family=AF_UNIX;
    strcpy(seraddr.sun_path,SER);
    unlink(SER);
    len=offsetof(struct sockaddr_un,sun_path)+strlen(seraddr.sun_path);
    ret=bind(lfd,(struct sockaddr*)&seraddr,len);
    if(ret==-1)
    {
        perror("bind error");
        exit(1);
    }
    listen(lfd,20);
     len=sizeof(cliaddr);
    cfd=accept(lfd,(struct sockaddr*)&cliaddr,&len);
    if(cfd<=0)
        {
           perror("连接失败");
           exit(1);
        }
        printf("连接成功\n");
    while(1)
    {
        ret=read(cfd,buf,sizeof(buf));
        for(i=0;i<ret;i++)
        {
            buf[i]=toupper(buf[i]);
        }
       write(cfd,buf,ret);
    }
    return 0;
}