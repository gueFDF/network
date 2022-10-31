#include <muduo/net/TcpServer.h>
#include <muduo/net/TcpClient.h>
#include <iostream>
#include <boost/utility.hpp>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>
#include <functional>

using namespace muduo;
using namespace muduo::net;
class TimeServer
{
public:
    TimeServer(muduo::net::EventLoop *loop,
               const muduo::net::InetAddress &listenAddr);

    void start();

private:
    void onConnection(const muduo::net::TcpConnectionPtr &conn);

    void onMessage(const muduo::net::TcpConnectionPtr &conn,
                   muduo::net::Buffer *buf,
                   muduo::Timestamp time);

    muduo::net::TcpServer server_;
};

TimeServer::TimeServer(muduo::net::EventLoop *loop,
                       const muduo::net::InetAddress &listenAddr)
    : server_(loop, listenAddr, "TimeServer")
{
    server_.setConnectionCallback(
        std::bind(&TimeServer::onConnection, this, _1));
    server_.setMessageCallback(
        std::bind(&TimeServer::onMessage, this, _1, _2, _3));
}

void TimeServer::start()
{
    server_.start();
}

void TimeServer::onConnection(const muduo::net::TcpConnectionPtr &conn)
{
    LOG_INFO << "TimeServer - " << conn->peerAddress().toIpPort() << " -> "
             << conn->localAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");
    if (conn->connected())
    {
        time_t now = ::time(NULL);
        int32_t be32 = sockets::hostToNetwork32(static_cast<int32_t>(now));
        conn->send(&be32, sizeof(be32));
        conn->shutdown();
    }
}

void TimeServer::onMessage(const muduo::net::TcpConnectionPtr &conn,
                           muduo::net::Buffer *buf,
                           muduo::Timestamp time)
{
    // 接收到所有的消息，然后回显
    muduo::string msg(buf->retrieveAllAsString());
    LOG_INFO << conn->name() << " TimeServer " << msg.size() << " bytes, "
             << "data received at " << time.toString();

    //   conn->send(msg);
}

int main()
{
    LOG_INFO << "pid = " << getpid();
    muduo::net::EventLoop loop;
    muduo::net::InetAddress listenAddr(2037);  //进行地址绑定，ip模认为0.0.0.0
    TimeServer server(&loop, listenAddr);
    server.start();
    loop.loop();
}
