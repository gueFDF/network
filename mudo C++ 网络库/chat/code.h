#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpConnection.h>
#include <functional>

class LengthHeaderCodec
{
public:
  typedef std::function<void(const muduo::net::TcpConnectionPtr &,
                             const muduo::string &message,
                             muduo::Timestamp)>
      StringMessageCallback;

  explicit LengthHeaderCodec(const StringMessageCallback &cb)
      : messageCallback_(cb)
  {
  }


    void send(muduo::net::TcpConnection *conn,
              const muduo::StringPiece &message);
    void onMessage(const muduo::net::TcpConnectionPtr &conn,
                   muduo::net::Buffer *buf,
                   muduo::Timestamp time);

private:
    StringMessageCallback messageCallback_;
    const static size_t kHeaderLen = sizeof(int32_t);
};