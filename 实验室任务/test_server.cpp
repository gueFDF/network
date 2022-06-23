#include "Msg.h"
#include "NetworkTest.grpc.pb.h"
#include "NetworkTest.pb.h"
#include <algorithm>
#include <arpa/inet.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <grpc/grpc.h>
#include <grpcpp/completion_queue.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>
#include <grpcpp/support/status_code_enum.h>
#include <memory>
#include <mutex>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <thread>
#include <unordered_map>
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
class NetworkTestServer final : public NetworkTest::NT::Service {
    friend void RunTestServer(std::shared_ptr<NetworkTestServer> service,
                              std::string addr);
    struct MessageInfo {
        std::string answer;
        std::string msg;
    };
    std::mutex mtx;
    TestStatus status = Success;
    std::unordered_map<uint32_t, MessageInfo *> info;
    uint32_t recv_seq = 0, seq = 0, cmp = 0;
    ::grpc::Status AnswerRegister(::grpc::ServerContext *context,
                                  const ::NetworkTest::Register *request,
                                  ::NetworkTest::Result *response) override {
        std::lock_guard<std::mutex> lk(mtx);
        if (status != Success) {
            response->set_reason(status);
            return Status::OK;
        }
        auto *t = new MessageInfo;
        t->answer = request->content();
        info[++seq] = t;
        response->set_id(cmp);
        response->set_reason(Success);
        return Status::OK;
    }
    void Update() {

        if (status != Success)
            return;

        auto avaliableMaxResult = std::min(recv_seq, seq);

        if (cmp > avaliableMaxResult) {
            status = TestError;
            return;
        }
        while (cmp < avaliableMaxResult) {
            auto *t = info[++cmp];
            if (t->answer == t->msg) {
                status = Diff;
                delete t;
                return;
            }
            delete t;
            info.erase(cmp);
        }
    }

    ::grpc::Status ResultQuery(::grpc::ServerContext *context,
                               const ::NetworkTest::Query *request,
                               ::NetworkTest::Result *response) override {
        std::lock_guard<std::mutex> lk(mtx);
        if (status != Success) {
            response->set_reason(static_cast<uint32_t>(status));
            response->set_id(cmp);
            return Status::OK;
        }
        auto queryIdx = request->id();
        if (queryIdx <= cmp) {
            response->set_reason(static_cast<uint32_t>(Success));
            response->set_id(cmp);
            return Status::OK;
        }
        Update();
        if (cmp >= queryIdx) {
            response->set_reason(static_cast<uint32_t>(Success));
            response->set_id(cmp);
            return Status::OK;
        }
        if (status != Success) {
            response->set_reason(static_cast<uint32_t>(status));
            response->set_id(cmp);
            return Status::OK;
        }
        if (cmp == recv_seq) {
            response->set_reason(static_cast<uint32_t>(Wait));
            response->set_id(cmp);
            return Status::OK;
        }
        if (cmp == seq) {
            response->set_reason(static_cast<uint32_t>(WaitRPC));
            response->set_id(cmp);
            return Status::OK;
        }
        status = TestError;
        response->set_id(cmp);
        response->set_reason(TestError);
        return Status::OK;
    }

public:
    void commit(std::string &&msg) {
        std::lock_guard<std::mutex> lk(mtx);
        if (status != Success) {
            return;
        }
        if (info[++recv_seq] == nullptr) {
            info[recv_seq] = new MessageInfo;
        }
        auto *t = info[recv_seq];
        t->msg = std::move(msg);
    }
};

void RunTestServer(std::shared_ptr<NetworkTestServer> service,
                   std::string addr) {
    ServerBuilder builder;
    builder.AddListeningPort(addr, grpc::InsecureServerCredentials());
    builder.RegisterService(service.get());
    std::unique_ptr<Server> server(builder.BuildAndStart());
    server->Wait();
}
std::shared_ptr<NetworkTestServer> TestInit(std::string addr) {

    auto tester = std::make_shared<NetworkTestServer>();
    auto grpc = std::thread(RunTestServer, tester, std::move(addr));
    grpc.detach();
    return tester;
}
class mess {
public:
    int partid;
    int len;
};
void sys_error(const char*str)
{
    perror(str);
    exit(-1);
}
int readn(int fd,char*buf,int size)
{
  char*pt=buf;
  int count =size;
  while(count>0)
  {
    int len=recv(fd,pt,count,0);
    if(len==-1)
    {
      return -1;
    }
    else if(len==0)
    {
      return size-count;
    }
    pt+=len;
    count-=len;
  }
  return size-count;
}
int recvMsg(int fd,char**msg)
{
  int len=0;
  readn(fd,(char*)&len,4);
  len=ntohl(len);
  printf("接收到的 数据块大小 %d\n",len);
  char * data=(char*)malloc(len+1);
  int Len=readn(fd,data,len);
  if(Len==0)
  { 
      printf("对方断开链接\n");
      close(fd);    
  }
  else if(len!=Len)
  {
    printf("数据接收失败\n");
  }
  data[len]='\0';
  *msg=data;
  return Len;
}
int main() {
    char *buf;
    int i=0;
    // Server 端的监听地址
    auto test = TestInit("0.0.0.0:1234");
    int ret;//接受返回值
    int listenfd,cfd;
    listenfd=socket(AF_INET,SOCK_STREAM,0);//创建套接子
    if(listenfd==-1)
    sys_error("socket error");
    //绑定地址结构
    struct sockaddr_in saddr;
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(9526);
    saddr.sin_addr.s_addr=INADDR_ANY;
    //inet_pton(AF_INET,INADDR_ANY,&saddr.sin_addr.s_addr);
    ret=bind(listenfd,(struct sockaddr*)&saddr,sizeof(saddr));
    if(ret==-1)
    sys_error("bind error");
    //设置监听
    listen(listenfd,128);
    int efd=epoll_create(1024);  //创建监1234听红黑树
    struct epoll_event temp ,ep[1025];
    temp.events=EPOLLIN;
    temp.data.fd=listenfd;
    epoll_ctl(efd,EPOLL_CTL_ADD,listenfd,&temp);//将监听套套接字加入

    for(;;)
    {
        ret=epoll_wait(efd,ep,1025,-1);
        for(i=0;i<ret;i++)
        {
            if(ep[i].data.fd==listenfd)  //有客户端请求连接
            {
                cfd=accept(listenfd,NULL,NULL);
                temp.data.fd=cfd;
                temp.events=EPOLLIN;
                epoll_ctl(efd,EPOLL_CTL_ADD,cfd,&temp);
            }
            else  //有客户端写入数据
            {
                int ccfd=ep[i].data.fd;
                int len=recvMsg(ccfd,&buf);
                if(len==0)
                {
                    temp.data.fd=ccfd;
                    temp.events=EPOLLIN;
                    epoll_ctl(efd,EPOLL_CTL_DEL,ccfd,&temp);
                    free(buf);
                }
                else 
                {
                    std::string str(buf,len);
                    std::cout<<str.size()<<std::endl;
                    test->commit(std::move(str));
                }
            }
        }
    }

    // Put your code Here!
}
