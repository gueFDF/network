#include<muduo/net/TcpServer.h>
#include<muduo/net/TcpClient.h>
#include<iostream>
#include <boost/utility.hpp>
#include<muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>
#include<functional>
using namespace muduo;
using namespace muduo::net;
class TimeClient:boost::noncopyable
{
public:
    TimeClient(EventLoop*loop,const InetAddress&serverAddr)
        :loop_(loop),client_(loop,serverAddr,"TimeClient")
        {
            client_.setConnectionCallback(std::bind(&TimeClient::onConnection,this,_1));
            client_.setMessageCallback(std::bind(&TimeClient::onMessage,this,_1,_2,_3));
        }
        void connect()
        {
            client_.connect();
        }
private:
    EventLoop*loop_;
    TcpClient client_;
private:
    void onConnection(const TcpConnectionPtr&conn)
    {
        LOG_INFO<<conn->localAddress().toIpPort()<<"->"<<conn->peerAddress().toIpPort()<<"is"<<(conn->connected()?"UP":"DOWN");
        if(!conn->connected())
        {
            loop_->quit();
        }
    }
    void onMessage(const TcpConnectionPtr&conn,Buffer*buf,Timestamp receiveTime)
    {
        if(buf->readableBytes()>=sizeof(int32_t)) //判断将数据全部收完
        {
            const void*data=buf->peek();
            int32_t be32=*static_cast<const int32_t*>(data);
            buf->retrieve(sizeof(int32_t));
            time_t time=sockets::networkToHost32(be32);
            Timestamp ts(time*Timestamp::kMicroSecondsPerSecond);
            LOG_INFO<<"server time = "<<time<<","<<ts.toFormattedString();
        }
        else 
        {
            LOG_INFO<<conn->name()<<"no enough data"<<buf->readableBytes()<<"at"<<receiveTime.toFormattedString();
        }
    }
};

int main(int argc,char*argv[])
{
    LOG_INFO<<"pid = "<<getpid();
    if(argc>1)
    {
        EventLoop loop;
        InetAddress serverAddr(argv[1],2037);
        TimeClient TimeClient(&loop,serverAddr);
        TimeClient.connect();
        loop.loop();
    }
    else
    {
        printf("Usage:%s host_ip\n",argv[0]);
    }
}