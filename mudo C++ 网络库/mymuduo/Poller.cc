#pragma once
#include "Poller.hpp"
namespace mymuduo
{
    Poller::Poller(EventLoop *loop) : ownerLoop_(loop) {}
    Poller::~Poller(){};

    //核心:调用poll(2)获得当前活动的得IO事件，然后填充调用方传入的activeChannels，并返回poll(2)return的时刻
    //&*pollfds_.begin()就相当于首元素地址
    muduo::Timestamp Poller::poll(int timeoutMs, ChannelList *activeChannels)
    {
        // poll等待事件发生
        int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);
        muduo::Timestamp now(muduo::Timestamp::now()); //获取当前时间
        if (numEvents > 0)
        {
            LOG_TRACE << numEvents << "events happended";
            //将发生事件的channel填充到vector中
            fillActiveChannels(numEvents, activeChannels);
        }
        else if (numEvents == 0)
        {
            LOG_TRACE << "nothing happended";
        }
        else
        {
            LOG_SYSERR << "Poller::poll()";
        }
        return now;
    }

    void Poller::fillActiveChannels(int numEvents, ChannelList *activechannels) const
    {
        for (PollFdList::const_iterator pfd = pollfds_.begin(); pfd != pollfds_.end() && numEvents > 0; ++pfd)
        {
            if(pfd->revents>0)
            {
                --numEvents;
                ChannelMap::const_iterator ch=Channels_.find(pfd->fd);
                assert(ch!=Channels_.end());
                Channel*channel=ch->second;
                assert(channel->fd()==pfd->fd);
                channel->set_revents(pfd->revents);
                activechannels->push_back(channel);
            }
        }
    }
}