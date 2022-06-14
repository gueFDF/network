#ifndef _SOCKET_H
#define _SOCKET_H 
#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>
#include<errno.h>
#include<strings.h>
#include<stdlib.h>
#include<string.h>
#include<sys/epoll.h>
//绑定+监听
int setListen(int lfd,unsigned short port);
int acceptConn(int lfd,struct sockaddr_in*addr);

//客户端
int connectToHost(int lfd, char*ip,unsigned short port);


int createsocket();
int sendMsg(int fd,const char*msg,int len);
int recvMsg(int fd,char**msg);
int writen(int fd, char*msg,int size);
int readn(int fd,char*buf,int size);

#endif
