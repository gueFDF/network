#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <string>
void onMessage(const muduo::net::TcpConnectionPtr &conn,
               muduo::net::Buffer *buf,
               muduo::Timestamp time)

{

    std::string message(buf->retrieveAllAsString());
    printf("%s", message.c_str());
    conn->send(message);
}


void onMessagecomple(const muduo::net::TcpConnectionPtr &conn)
{
    conn->send("发送完成");
}
int main(int argc, char *argv[])
{
    muduo::net::EventLoop loop;
    muduo::net::InetAddress listenaddr(9981);
    muduo::net::TcpServer server(&loop, listenaddr, "echoserver");
    
    if (argc > 1)
    {
        server.setThreadNum(atoi(argv[1]));
    }
    server.setMessageCallback(onMessage);
    server.setWriteCompleteCallback(onMessagecomple);
    server.start();
    loop.loop();
    return 0;
}