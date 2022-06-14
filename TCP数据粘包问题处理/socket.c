#include"socket.h"
int createsocket()
{
  int lfd=socket(AF_INET,SOCK_STREAM,0);
  if(lfd==-1)
  {
    perror("socket error");
    return -1;
  }
  else 
  {
    printf("套接字创建成功\n");
    return lfd;
  }
}
int setListen(int lfd,unsigned short port)
{
  struct sockaddr_in saddr;
  saddr.sin_family=AF_INET;
  saddr.sin_port=htons(port);
  saddr.sin_addr.s_addr=INADDR_ANY;
  int ret=0;
  ret=bind(lfd,(struct sockaddr*)&saddr,sizeof(saddr));
  if(ret==-1)
  {
    perror("bind error");
    return -1;
  }
  else 
  {
    printf("套接字地址绑定成功\n");
  }
  listen(lfd,128);
  return 1;
}
int acceptConn(int lfd,struct sockaddr_in*addr)
{
  int cfd=-1;
  if(addr==NULL)
  {
    cfd=accept(lfd,NULL,NULL);
    return cfd;
  }
  socklen_t len=sizeof(addr);
  cfd=accept(lfd,(struct sockaddr*)&addr,&len);
  return cfd;
}
int connectToHost(int  lfd,char*ip,unsigned short port)
{
  struct sockaddr_in caddr;
  caddr.sin_family=AF_INET;
  caddr.sin_port=htons(port);
  inet_pton(AF_INET,ip,&caddr.sin_addr.s_addr);
  int fd= connect(lfd,(struct sockaddr*)&caddr,sizeof(caddr));
  printf("成功与客户端建立链接\n");
  return fd;
}
int writen(int fd, char*msg,int size)
{
  char*buf=msg;
  int count =size;
  while(count>0)
  {
    int len=send(fd,buf,count,0);
    if(len==-1)
    {
      return -1;
    }
    else if(len==0)
    {
      continue;
    }
    buf+=len;
    count-=len;
  }
  return size;
}

int sendMsg(int fd,const char*msg,int len)
{
  if(fd<0||msg==NULL||len<=0)
  {
    return -1;
  }
  char*data=(char*)malloc(sizeof(char)*(len+4));
  int biglen=htonl(len);
  memcpy(data,&biglen,4);
  memcpy(data+4,msg,len);
  int ret;
  ret=writen(fd,data,len+4);
  if(ret==-1)
  {
    perror("send error");
    close(fd);
  }
  return ret;
}
int readn(int fd,char*buf,int size)
{
  char*pt=buf;
  int count =size;
  while(count>0)
  {
    int len=recv(fd,pt,count,0);
    if(len==-1)
    {
      return -1;
    }
    else if(len==0)
    {
      return size-count;
    }
    pt+=len;
    count-=len;
  }
  return size-count;
}
int recvMsg(int fd,char**msg)
{
  int len=0;
  readn(fd,(char*)&len,4);
  len=ntohl(len);
  printf("接收到的 数据块大小 %d",len);
  *msg=(char*)malloc(len+1);
  int Len=readn(fd,*msg,len);
  if(Len==0)
  { 
      printf("对方断开链接\n");
      close(fd);    
  }
  else if(len!=Len)
  {
    printf("数据接收失败\n");
  }
  *msg[len]='\0';
  return Len;
}














