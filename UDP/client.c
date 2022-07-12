#include <netinet/in.h>
#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<arpa/inet.h>
#define MAX 1024
int main(int argc,char *argv[])
{
    ssize_t s;
    int clifd;
    char buf[MAX];
    struct sockaddr_in ser,temp;
    clifd=socket(AF_INET,SOCK_DGRAM,0);
    ser.sin_family=AF_INET;
    ser.sin_port=htons(atoi(argv[2]));
    ser.sin_addr.s_addr=inet_addr(argv[1]);
    //编写完成
    while(1)
    {
        s=read(0,buf,sizeof(buf));
        socklen_t len=sizeof(temp);
        sendto(clifd,buf,s,0,(struct sockaddr*)&ser,sizeof(ser));
        s=recvfrom(clifd,buf,sizeof(buf)-1,0,(struct sockaddr*)&temp,&len);
        write(1,buf,s);
    }
}