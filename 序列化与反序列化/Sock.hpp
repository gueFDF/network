#include <iostream>
#include <unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<cstdlib>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<cstring>
#include<string>
class Sock
{
public:
    static int Socket()
    {
        int sockfd=socket(AF_INET,SOCK_STREAM,0);
        if(sockfd<0)
        {
            perror("socket");
            exit(1);
        }
        return sockfd;
    }
    static void Listen(int fd,int num=20)
    {
        if(listen(fd,num)<0)
        {
            perror("listen error");
            exit(1);
        }
    }
    static void Bind(int fd,uint16_t port,std::string ip ="0")
    {
        struct sockaddr_in local;
        bzero(&local,sizeof(local));
        local.sin_family=AF_INET;
        local.sin_port=htons(port);
        inet_pton(AF_INET,ip.c_str(),&local.sin_addr);
        if(bind(fd,(struct sockaddr*)&local,sizeof(local))<0)
        {
            perror("bind error");
            exit(1);
        }
    }
    static int Accept(int fd)
    {
        struct sockaddr cliaddr;
        bzero(&cliaddr,sizeof(cliaddr));
        socklen_t len =sizeof(cliaddr);
        int cfd=accept(fd,(struct sockaddr*)&cliaddr,&len);
        if(cfd<0)
        {
            perror("accept error");
            exit(1);
        }
        return cfd;
    }
    static void Connect(int fd,std::string ip,int port)
    {
        struct sockaddr_in addr;
        bzero(&addr,sizeof(addr));
        addr.sin_family=AF_INET;
        addr.sin_port=htons(port);
        inet_pton(AF_INET,ip.c_str(),&addr.sin_addr);
        if(connect(fd,(struct sockaddr*)&addr,sizeof(addr))<0)
        {
            perror("connect");
            exit(1);
        }
    }
};