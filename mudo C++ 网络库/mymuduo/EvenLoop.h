#pragma once
#include <iostream>
#include "noncopyable.hpp"
#include <muduo/base/CurrentThread.h>
#include <muduo/base/Logging.h>
#include <poll.h>
#include <boost/utility.hpp>
#include <boost/scoped_ptr.hpp>
#include <memory>
#include <vector>
class Channel;
class Poller;
class EventLoop 
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
    void updatechannel(Channel *);
    void quit();

private:
    typedef std::vector<Channel *> ChannelList;
    void abortNotInLoopThread();
    bool looping_;
    bool quit_;
    const pid_t threadId_;


    boost::scoped_ptr<Poller> poller_;
    ChannelList activeChannels_;

};