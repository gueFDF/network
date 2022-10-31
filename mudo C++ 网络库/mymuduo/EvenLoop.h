#pragma once 
#include <iostream>
#include "noncopyable.hpp"
#include <muduo/base/CurrentThread.h>
#include <muduo/base/Logging.h>
#include <poll.h>
#include <boost/utility.hpp>
class Channel;
class EventLoop : noncopyable
{
public:
    EventLoop();
    ~EventLoop();
    void loop(); //运行事件循环
    void assertInLoopThread()
    {
        if (!isInLoopThread()) //如果不在当前线程
        {
            abortNotInLoopThread();
        }
    }
    bool isInLoopThread() const { return threadId_ == muduo::CurrentThread::tid(); };
    EventLoop *getEventLoopOfCurrentThread(); //返回当前的eventloop对象
    void updatechannel(Channel*);
private:
    void abortNotInLoopThread();
    bool looping_;
    const pid_t threadId_;
};