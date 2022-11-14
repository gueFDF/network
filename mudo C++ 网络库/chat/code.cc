#include"code.h"
void LengthHeaderCodec::send(muduo::net::TcpConnection *conn,
          const muduo::StringPiece &message)
{

    //处理粘包问题
    muduo::net::Buffer buf;
    buf.append(message.data(), message.size());
    int32_t len = static_cast<int32_t>(message.size());
    int32_t be32 = muduo::net::sockets::hostToNetwork32(len);
    //buf的前4节预留空间，存放消息长度
    buf.prepend(&be32, sizeof(be32));
    conn->send(&buf);
}

void LengthHeaderCodec::onMessage(const muduo::net::TcpConnectionPtr &conn,
               muduo::net::Buffer *buf,
               muduo::Timestamp time)
{
    while(buf->readableBytes()>=4)
    {
        const void*data=buf->peek();
        int32_t be32=*static_cast<const int32_t*>(data);
        //网络字节序转换为本地字节序
        const int32_t len=muduo::net::sockets::networkToHost32(be32);
        if(len>65536||len<0)
        {
            printf("error len\n");
            conn->shutdown();
            break;
        }
        else if(buf->readableBytes()>=len+4)
        {
            buf->retrieve(4);
            muduo::string message(buf->peek(),len);
            /*收到消息后处理，暂时省略*/

            messageCallback_(conn,message,time);

            ////////
            buf->retrieve(len);
        }
    }
}