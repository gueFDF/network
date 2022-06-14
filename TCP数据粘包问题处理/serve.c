#include"socket.h"
#define MAXLEN 1000
int main()
{
  int i;
  int ret;//检测返回值
  int cfd;
  char *buf;
  int fd=createsocket();
  if(fd==-1)
    return -1;
 int listenfd=0;
 listenfd=setListen(fd,9527);
 int epfd=epoll_create(MAXLEN);  //创建监听红黑树
 struct epoll_event temp,ep[1024];
 temp.data.fd=listenfd;
 temp.events=EPOLLIN;
 epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&temp);//将监听套接字加入监听红黑树
while(1)
{
  ret=epoll_wait(epfd,ep,1024,-1);
  for(i=0;i<ret;i++)
  {
    if(ep[i].data.fd==listenfd)
    {
      cfd=acceptConn(listenfd,NULL);
      temp.data.fd=cfd;
      temp.events=EPOLLIN;
      epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&temp);
    }
    else 
    {
      int ccfd=ep[i].data.fd;
      int len=recvMsg(ccfd,&buf);
      if(len==0)
      {
        temp.data.fd=ccfd;
        temp.events=EPOLLIN;
        epoll_ctl(epfd,EPOLL_CTL_DEL,ccfd,&temp);
        free(buf);
      }
      else 
      {
        printf("%s",buf);
        free(buf);
      }
    }
  }
}

 close(listenfd);

  return 0;
}
