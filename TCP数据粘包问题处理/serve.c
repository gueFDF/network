#include"socket.h"
#define MAXLEN 1000
int main()
{
  int i;
  int ret=-1;//检测返回值
  int cfd;
  char *buf;
  int listenfd=createsocket();
  if(listenfd==-1)
    return -1;
 struct sockaddr_in saddr;
 saddr.sin_family=AF_INET;
 saddr.sin_port=htons(9526);
 saddr.sin_addr.s_addr=INADDR_ANY;
 ret=bind(listenfd,(struct sockaddr*)&saddr,sizeof(saddr));
 if(ret==-1)
 {
   perror("bind error");
   exit(-1);
 }
 ret=listen(listenfd,128);
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
      printf("客户端%d连接成功\n",cfd);
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
        printf("%s\n\n\n\n",buf);
        free(buf);
      }
      sleep(1);
    }
  }
}

 close(listenfd);
  return 0;
}
