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
#include <muduo/base/Timestamp.h>
#include <muduo/net/TimerId.h>
#include <functional>
#include <muduo/base/Mutex.h>
#include "TimerQueue.h"
// class Channel;
// class Poller;
class EventLoop
{
public:
    typedef std::function<void()> Functor;
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
    void queueInLoop(const Functor &cb);

    // TimerId runAt(const muduo::Timestamp &time, const TimerCallback &cb)
    // {
    //     return timerQueue_->addTimer(cb, time, 0.0);
    // }
    // TimerId runAfter(double delay, const TimerCallback &cb)
    // {
    //     muduo::Timestamp time(muduo::addTime(muduo::Timestamp::now(), delay));
    //     return runAt(time, cb);
    // }
    // TimerId runEvery(double interval, const TimerCallback &cb)
    // {
    //     muduo::Timestamp time(muduo::addTime(muduo::Timestamp::now(), interval));
    //     return timerQueue_->addTimer(cb,time,interval);
    // }

    void runInLoop(const Functor &cb);

private:
    typedef std::vector<Channel *> ChannelList;
    void abortNotInLoopThread();
    bool looping_;
    bool quit_;
    const pid_t threadId_;

    boost::scoped_ptr<Poller> poller_;
    ChannelList activeChannels_;
    // TimerQueue *timerQueue_;

    void handleRead(); //唤醒
    void doPendingFunctors();
    muduo::Timestamp pollReturnTime_;
    boost::scoped_ptr<TimerQueue> timerQueue_;
    int wakeupFd_;
    boost::scoped_ptr<Channel> wakeupChannel_;
    muduo::MutexLock mutex_;
    std::vector<Functor> pendingFunctors_;
};