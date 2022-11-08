#include "Poller.h"
#include"channel.h"
#include"EvenLoop.h"
__thread EventLoop *t_loopInThisThread = 0;

EventLoop::EventLoop()
    : looping_(false),quit_(false), threadId_(muduo::CurrentThread::tid()),poller_(new Poller(this))
{
    LOG_TRACE << "EventLoop created" << this << "in thread" << threadId_;
    if (t_loopInThisThread) //判断当前线程是否已创建了EventLoop，若已创建，FATAL，若没有创建，将this给t_loopInThisThread
    {
        LOG_FATAL << "Another Eventloop" << t_loopInThisThread << "exists in this thread" << threadId_;
    }
    else
    {
        t_loopInThisThread = this;
    }
}

EventLoop::~EventLoop()
{
    assert(!looping_);
    t_loopInThisThread = nullptr;
}

EventLoop *EventLoop::getEventLoopOfCurrentThread()
{
    return t_loopInThisThread;
}

void EventLoop::abortNotInLoopThread()
{
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
              << " was created in threadId_ = " << threadId_
              << ", current thread id = " << muduo::CurrentThread::tid();
}

// IO线程只能在某个特定线程调用，所以要判断
void EventLoop::loop()
{

    int  kPollTimeMs=5*1000;
    assert(!looping_);    //确保只开启一次
    assertInLoopThread(); //保证在当前线程
    looping_ = true;
    quit_=false;
    while(!quit_)
    {
        activeChannels_.clear();
        poller_->poll(kPollTimeMs,&activeChannels_);
        for(ChannelList::iterator it=activeChannels_.begin();it!=activeChannels_.end();++it)
        {
            //处理事件对应回调
            (*it)->handleEvent();
        }
    
    }


    LOG_TRACE << "EventLoop" << this << "stop looping";
    looping_ = false;
}

void EventLoop::quit()
{
    quit_=true;
}

void EventLoop::updatechannel(Channel*channel)
{
    assert(channel->ownerLoop()==this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}


//若在当前io线程，直接调用回调，若用户在其他线程调用runInLoop，cb会加入队列
void EventLoop:: runInLoop(const Functor &cb)
{
    if(isInLoopThread())
    {
        cb();
    }
    else
    {
        queueInLoop(cb);
    }

}


//将cb加入队列
void EventLoop::queueInLoop(const Functor&cb)
{
    {
        muduo::MutexLockGuard lock(mutex_);
        pendingFunctors_.push_back(cb);
    }
    if(!isInLoopThread()/*||callin*/)
    {
        //wak
    }

}