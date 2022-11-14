#include <muduo/net/TcpServer.h>
#include <muduo/net/TcpClient.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/base/Mutex.h>
#include <functional>
#include<muduo/net/EventLoopThread.h>
#include <set>
#include "code.h"
#include <stdlib.h>
#include<iostream>
using namespace std::placeholders;
class chatClient
{
public:
    chatClient(muduo::net::EventLoop*loop,const muduo::net::InetAddress&serverAddr)
    :loop_(loop),
    client_(loop,serverAddr,"ChatClient"),
    codec_(std::bind(&chatClient::onStringMessage,this,_1,_2,_3))
    {
        client_.setConnectionCallback(std::bind(&chatClient::onConnection,this,_1));
        client_.setMessageCallback(std::bind(&LengthHeaderCodec::onMessage,&codec_,_1,_2,_3));
        client_.enableRetry();
    }

    void connect()
    {
        client_.connect();
    }
    void disconnect()
    {
        //
    }

    void write(const muduo::StringPiece&message)
    {
        muduo::MutexLockGuard lock(mutex_);
        if(connection_)
        {
            codec_.send(muduo::get_pointer(connection_),message);
        }

    }
private:
    void onConnection(const muduo::net::TcpConnectionPtr &conn)
    {
        if (conn->connected())
        {
            connection_ = conn;
        }
        else
        {
            connection_.reset();
        }
    }
    void onStringMessage(const muduo::net::TcpConnectionPtr &,
                         const muduo::string &message,
                         muduo::Timestamp)
    {
        printf("<<<< %s\n",message.c_str());
    }

private:
    muduo::net::EventLoop *loop_;
    muduo::net::TcpClient client_;
    LengthHeaderCodec codec_;
    muduo::MutexLock mutex_;
    muduo::net::TcpConnectionPtr connection_;
};

int main(int argc,char*argv[])
{
    muduo::net::EventLoopThread loopThread;
    uint16_t port=static_cast<uint16_t>(::atoi(argv[2]));
    muduo::net::InetAddress serverAddr(argv[1],port);
    chatClient client(loopThread.startLoop(),serverAddr);
    client.connect();
    std::string line;
    while(std::getline(std::cin,line))
    {
        client.write(line);
    }
    client.disconnect();

}