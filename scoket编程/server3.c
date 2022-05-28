#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<string.h>
#include<ctype.h>
#include<strings.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<pthread.h>
#include<signal.h>
#include<sys/wait.h>
void myerror(const char*str)
{
    perror(str);
    exit(1);
}
#define SRV_PORT 9995
void*do_work(void*arg)
{
    int n,i;
    int cfd=*((int*)arg);
    char buf[BUFSIZ];
    while(1)
    {
        n=read(cfd,buf,sizeof(buf));
        if(n==0)
        {
            break;
        }
        for(i=0;i<n;i++)
        {
            buf[i]=toupper(buf[i]);
        }
        write(STDOUT_FILENO,buf,n);
        write(cfd,buf,n);
    }
    close(cfd);  
    return NULL;
}
int main()
{
    struct sockaddr_in ser_addr,clt_addr;
    socklen_t len;
    pthread_t tid;
    int lfd,cfd;
    int i=0;
    int ttid[256];//用来存放文件描述符
    int ret;//判断返回值
    bzero(&ser_addr,sizeof(ser_addr));
    ser_addr.sin_family=AF_INET;
    ser_addr.sin_port=htons(SRV_PORT);
    ser_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    lfd=socket(AF_INET,SOCK_STREAM,0);
    if(lfd==-1)
    myerror("socket error");
    ret=bind(lfd,(struct sockaddr*)&ser_addr,sizeof(ser_addr));
    if(ret==-1)
    myerror("bind error");
    ret=listen(lfd,128);
    if(ret==-1)
    myerror("listem error");
    while(1)
    {
        len =sizeof(clt_addr);
        cfd=accept(lfd,(struct sockaddr*)&clt_addr,&len);
        if(cfd==-1)
        myerror("accept error");
        ttid[i]=cfd;
        pthread_create(&tid,NULL,do_work,(void*)&ttid[i]);
        pthread_detach(tid);
        i++;
    }
    close(lfd);
    return 0;
}