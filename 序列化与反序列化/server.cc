#include"Sock.hpp"
#include"message.hpp"
int main()
{
   int sockfd=Sock::Socket();
   Sock::Bind(sockfd,9999);
   Sock::Listen(sockfd);
   message temp;
   int cfd= Sock::Accept(sockfd);
   //cout<<"收到一条消息"<<endl;
   while(1)
   {
       char buf[1024];
       int ret=read(cfd,buf,sizeof(buf));
       if(ret==0)
       {
           printf("对象已下线\n");
           close(cfd);
           close(sockfd);
           return 0;
       }
       string json=buf;
       bzero(&buf,sizeof(buf));
       temp.receive(json);
       temp.print();
   }
}