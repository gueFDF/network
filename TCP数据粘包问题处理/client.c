#include"socket.h"
int main()
{
  char temp[1000];
  int fd=createsocket();
  if(fd==-1)
    return -1;
  int ret=connectToHost(fd,"127.0.0.1",9527);
  if(ret==-1)
  {
    return -1;
  }
  int length=0;
  int fd1=open("english.txt",O_RDONLY);
  while((length=read(fd1,temp,rand()%1000))>0)
  {
    sendMsg(fd,temp,length);
    memset(temp,0,sizeof(temp));;
    usleep(300);
  }
  sleep(10);

}
