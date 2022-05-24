#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/ip.h>
#include<netinet/in.h>
#include<ctype.h>
#include<arpa/inet.h>
#define SERV_PORT 9527
void sys_err(const char*str)
{
    perror(str);
    exit(1);
}
int main()
{
    int lfd=0,cld=0;
    int ret;
    char buf[BUFSIZ];
    struct sockaddr_in serv_addr,clit_addr;
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(SERV_PORT);
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    lfd=socket(AF_INET,SOCK_STREAM,0);
    if(lfd==-1)
    sys_err("socket error");
    bind(lfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    listen(lfd,128);
    socklen_t len=sizeof(clit_addr);
    cld=accept(lfd,(struct sockaddr*)&clit_addr,&len);
    if(cld==-1)
    sys_err("accept error");
    while(1)
    {
        ret=read(cld,buf,sizeof(buf));
        write(STDOUT_FILENO,buf,ret);
       for(int i=0;i<ret;i++)
       {
           buf[i]=toupper(buf[i]);
       }
      write(cld,buf,ret);     
    }
    close(cld);
    close(lfd);
    return  0;
}