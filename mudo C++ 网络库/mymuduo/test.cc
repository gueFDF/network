#include "EvenLoop.h"
#include <cstdio>
#include <sys/types.h>
#include <unistd.h>
#include "muduo/base/Thread.h"
#include "channel.h"

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

// EventLoop *g_loop;

// void threadFunc()
// {
//     g_loop->loop();
// }

// int main()
// {
//     EventLoop loop;
//     g_loop = &loop;
//     muduo::Thread thread(threadFunc);
//     thread.start();
//     loop.loop();
//     thread.join();
// }

// test3测试
#include <sys/timerfd.h>
#include "Poller.h"

EventLoop *g_loop;

void timeout()
{
    printf("Timeout!\n");
    g_loop->quit();
}

int main()
{
    EventLoop loop;
    g_loop = &loop;
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(&loop, timerfd);
    channel.setReadCallback(timeout);
    channel.enableReading();
    struct itimerspec howlong;
    bzero(&howlong, sizeof(howlong));
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd, 0, &howlong, NULL);
    loop.loop();
    ::close(timerfd);

    return 0;
}