/*Poller class 是IO多路的封装，它现在是一个具体类，而在muduo中是个抽象基类，
因为muduo库同时支持poll和epoll两张IO多路复用机制。
poller是EventLoop的间接成员，只提供其owner EvenLoop在IO线程调用，因此无需加锁*/
#pragma once

#include <muduo/base/Timestamp.h>
#include <vector>
#include <map>
#include<muduo/base/Logging.h>
#include<poll.h>
#include "Poller.h"
#include"channel.h"
#include"EvenLoop.h"
struct pollfd;
struct Channel;
struct EventLoop;
    // class Channel;
    class Poller 
    {
    public:
        typedef std::vector<Channel *> ChannelList;
        Poller(EventLoop *loop);
        ~Poller();

        // Polls the I/O events

        muduo::Timestamp poll(int timeoutMs, ChannelList *activeChannels);

        void updateChannel(Channel *channel);

        //保证在当前IO线程
        void assertInLoopThread() { ownerLoop_->assertInLoopThread(); }

    private:
        void fillActiveChannels(int numEvents, ChannelList *activechannels) const;
        typedef std::vector<struct pollfd> PollFdList;
        typedef std::map<int, Channel *> ChannelMap;

        EventLoop *ownerLoop_;
        PollFdList pollfds_;
        ChannelMap Channels_;
    };

