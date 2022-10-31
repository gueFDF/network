#include "Poller.h"
#include"channel.h"
#include"EvenLoop.h"
#include<poll.h>
#include <muduo/base/Logging.h>
const int Channel::kNoneEvent = 0;                //不关注任何事件
const int Channel::kReadEvent = POLLIN | POLLPRI; //关注读事件
const int Channel::kWriteEvent = POLLOUT;         //关注渎事件

Channel::Channel(EventLoop *loop, int fdArg)
    : loop_(loop),
      fd_(fdArg),
      events_(0),
      revents_(0),
      index_(-1)
{
}

void Channel::update()
{
    loop_->updatechannel(this);
}

// channel的核心,会由EventLoop:loop调用,处理各种事件的回调函数
void Channel::handleEvent()
{
    if (revents_ & POLLNVAL)
    {
        LOG_WARN << "Channel::handle_event() POLLNVAL";
    }
    if (revents_ & (POLLERR | POLLNVAL))
    {
        if (errorCallback_)
            errorCallback_();
    }
    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP))
    {
        if (readCallback_)
            readCallback_();
    }
    if (revents_ & POLLOUT)
    {
        if (writeCallback_)
            writeCallback_();
    }
}