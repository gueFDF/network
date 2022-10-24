#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <map>
using namespace muduo;
using namespace muduo::net;

typedef std::map<string, string> UserMap;
UserMap users;

string getUser(const string &user)
{
    string result = "No Such user";
    UserMap::iterator it = users.find(user);
    if (it != users.end()) //找到了
    {
        result = it->second;
    }
    return result;
}

void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp receiveTime)
{
    const char *crlf = buf->findCRLF();
    if (crlf)
    {
        string user(buf->peek(), crlf);
        conn->send(getUser(user) + "\r\n");
        buf->retrieveUntil(crlf + 2);
        conn->shutdown();
    }
}

void onConnection(const TcpConnectionPtr &conn) //接受新链接后主动断开
{
    if (conn->connected())
    {

        conn->shutdown();
    }
}

int main()
{
    users["guotianyu"] = "Happy and well";
    EventLoop loop;
    TcpServer server(&loop, InetAddress(9999), "Finger"); // 创建服务器.端口1079
                                                          //  server.setConnectionCallback(onConnection);           //设置回调函数
    server.setMessageCallback(onMessage);

    server.start(); //启动
    loop.loop();
    return 0;
}

