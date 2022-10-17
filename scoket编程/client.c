#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<errno.h>
#include<stdlib.h>
#define SERV_PORT 9527
void sys_err(const char*str)
{
    perror(str);
    exit(1);
}
int main()
{
    char buf[BUFSIZ];
    int cfd=0;
    int ret;
    cfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in servaddr;
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(SERV_PORT);
    inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr.s_addr);
    ret=connect(cfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
    if(ret==-1)
    {
        perror("connect error");
        exit(1);
    }
    printf("连接成功\n");
    while(1)
    {
        write(cfd,"hello",5);
        int ret=read(cfd,buf,sizeof(buf));
        sleep(1);
        write(1,buf,ret);
        printf("\n");
    }
    close(cfd);
    return 0;
}