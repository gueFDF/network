#ifndef _SOCKET_H
#define _SOCKET_H 
//绑定+监听
int setListen(int lfd,unsigned short port);
int acceptConn(int lfd,struct socket_in*addr);

//客户端
int connectToHost(int fd,const char*ip,unsigned short port);


int creatsocket();
int sendMsg(int fd,const char*msg,int len);
int recvMsg(int fd,char**msg);

#endif
