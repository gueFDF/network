#include "EvenLoop.h"
#include <cstdio>
#include <sys/types.h>
#include <unistd.h>
#include "muduo/base/Thread.h"
#include"channel.hpp"

// test1
//  void ThreadFunc()
//  {
//      printf("threadFunc():pid=%d,tid=%d\n",getpid(),muduo::CurrentThread::tid());
//      EventLoop loop;
//      loop.loop();
//  }

// int main()
// {
//     printf("threadFunc():pid=%d,tid=%d\n",getpid(),muduo::CurrentThread::tid());
//     EventLoop loop;
//     muduo::Thread thread(ThreadFunc);
//     thread.start();
//     loop.loop();
//     pthread_exit(NULL);
//     return 0;
// }

// test2,负面测试

EventLoop *g_loop;

void threadFunc()
{
    g_loop->loop();
}

int main()
{
    EventLoop loop;
    g_loop = &loop;
    muduo::Thread thread(threadFunc);
    thread.start();
    loop.loop();
    thread.join();
}