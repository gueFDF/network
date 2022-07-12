#include<stdio.h>
#include<arpa/inet.h>
#include<errno.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<strings.h>
#include<stdlib.h>
#include<ctype.h>
#define MAX 1024
int main(int argc,char *argv[])
{
    struct sockaddr_in ser,cli;
    int s;
    int ret;
    char buf[MAX];
    int serfd;
    serfd= socket(AF_INET,SOCK_DGRAM,0);
    bzero(&ser,sizeof(ser));
    ser.sin_family=AF_INET;
    if(argc!=2)//采用自己分配的端口
    {
        ser.sin_port=htons(9527);
    }
    else//采用提供的端口
    {
        ser.sin_port=htons(atoi(argv[1]));
    }
    ser.sin_addr.s_addr=INADDR_ANY;
    ret=bind(serfd,(struct sockaddr*)&ser,sizeof(ser));
    if(ret==-1)
    {
        perror("bind 绑定失败");
        exit(1);
    }
    socklen_t len=sizeof(cli);
    //全部准备工作已完成，接下来提供服务
    while(1)
    {
       s=recvfrom(serfd,buf,sizeof(buf)-1,0,(struct sockaddr*)&cli,&len);
       if(s<0)
       {
           printf("服务器断开连接\n");
           return 1;
       }
       else 
       {
          int i=0;
          write(1,buf,s);
          for(i=0;i<s;i++)
          {
              buf[i]=toupper(buf[i]);
          }
          sendto(serfd,buf,s,0,(struct sockaddr*)&cli,sizeof(cli));
       }
    }
    
}