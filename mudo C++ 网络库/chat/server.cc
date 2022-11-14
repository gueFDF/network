#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpConnection.h>
#include <functional>
#include <set>
#include "code.h"
#include <stdlib.h>

class Chatserver
{
public:
    Chatserver(muduo::net::EventLoop *loop, const muduo::net::InetAddress &listenAddr)
        : loop_(loop),
          server_(loop_, listenAddr, "chatserver"),
          codec_(std::bind(&Chatserver::onMessage, this,
                           std::placeholders::_1,
                           std::placeholders::_2,
                           std::placeholders::_3))
    {
        server_.setConnectionCallback(std::bind(&Chatserver::onConnection,
                                                this,
                                                std::placeholders::_1));
        server_.setMessageCallback(std::bind(&LengthHeaderCodec::onMessage, &codec_,
                                             std::placeholders::_1,
                                             std::placeholders::_2,
                                             std::placeholders::_3));
        //设置线程数量
        server_.setThreadNum(5);
    }

    void start()
    {
        server_.start();
    }

private:
    void onMessage(const muduo::net::TcpConnectionPtr &,
                   const muduo::string &message,
                   muduo::Timestamp)
    {
        for (connectionList::iterator it = connections_.begin(); it != connections_.end(); it++)
        {
            codec_.send(muduo::get_pointer(*it), message);
        }
    }

    void onConnection(const muduo::net::TcpConnectionPtr &conn)
    {
        if (conn->connected())
        {
            connections_.insert(conn);
        }
        else
        {
            connections_.erase(conn);
        }
    }

private:
    typedef std::set<muduo::net::TcpConnectionPtr> connectionList; //存放客户端
    muduo::net::EventLoop *loop_;
    muduo::net::TcpServer server_;
    LengthHeaderCodec codec_;
    connectionList connections_;
};

int main(int argc, char *argv[])
{
    muduo::net::EventLoop loop;
    uint16_t port = 8888;
    if (argc >= 1)
    {
        port = static_cast<uint16_t>(::atoi(argv[1]));
    }
    muduo::net::InetAddress listenaddr(port);
    Chatserver server(&loop, listenaddr);
    server.start();
    loop.loop();
}