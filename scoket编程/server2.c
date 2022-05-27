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
void sigcatch(int siganl)//信号捕捉函数
{
    while((waitpid(0,NULL,WNOHANG))>0);
    return;
}
int main(int argc,int *argv[])
{
    char buf[BUFSIZ];
    int lfd;
    int cfd;
    socklen_t len;
    pid_t pid;//子进程id
    int ret;//检测返回值
    struct sockaddr_in srv_addr,clt_addr;
    bzero(&srv_addr,sizeof(srv_addr));
    srv_addr.sin_family=AF_INET;
    srv_addr.sin_port=htons(SRV_PORT);
    srv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    lfd=socket(AF_INET,SOCK_STREAM,0);
    if(lfd==-1)
    myerror("socket error");
    int opt=1;
    setsockopt(lfd,SOL_SOCKET ,SO_REUSEADDR,(void *)&opt,sizeof(int));
    ret=bind(lfd,(struct sockaddr*)&srv_addr,sizeof(srv_addr));
    if(ret==-1)
    myerror("bind error");
    ret=listen(lfd,128);
    if(ret==-1)
    myerror("listen error");
    while(1)
    {
        len=sizeof(clt_addr);
        cfd=accept(lfd,(struct sockaddr*)&(clt_addr),&len);
        if(cfd==-1)
        myerror("accept error");
        pid=fork();
        if(pid<0)//错误
        {
            myerror("fork error");
        }
        else if(pid==0)//子进程
        {
            close(lfd);
            break;

        }
        else//父进程
        {
            struct sigaction act,oldact;
            act.sa_handler=sigcatch;
            sigemptyset(&act.sa_mask);
            act.sa_flags=0;
            ret=sigaction(SIGCHLD,&act,&oldact);
            if(ret!=0)
            {
                myerror("sigaction error");
            }
            close(cfd);
            continue;
        }
    }
    if(pid==0)//子进程
    {
        while(1)
        {
            ret=read(cfd,buf,sizeof(buf));
            if(ret==0)
            {
                close(cfd);
                exit(1);
            }

            for(int i=0;i<ret;i++)
            buf[i]=toupper(buf[i]);
            write(cfd,buf,ret);
            write(STDOUT_FILENO,buf,ret);
        }
        close(cfd);
    }

    
    return 0;
}