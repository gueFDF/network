#include"socket.h"
int main()
{
  char temp[1001];
  int fd=socket(AF_INET,SOCK_STREAM,0);
  struct sockaddr_in caddr;
  caddr.sin_family=AF_INET;
  caddr.sin_port=htons(9526);
  inet_pton(AF_INET,"127.0.0.1",&caddr.sin_addr.s_addr);
  connect(fd,(struct sockaddr*)&caddr,sizeof(caddr));
  printf("客户端连接成功\n");
  if(fd==-1)
    return -1;
  
  int length=0;
  int fd1=open("english.txt",O_RDONLY);
  while((length=read(fd1,temp,rand()%1000))>0)
  {
    sendMsg(fd,temp,length);
    memset(temp,0,sizeof(temp));
    usleep(300);
  }
  sleep(10);

}
