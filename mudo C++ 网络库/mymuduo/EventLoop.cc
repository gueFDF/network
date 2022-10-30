#include "EvenLoop.h"
__thread EventLoop *t_loopInThisThread = 0;

EventLoop::EventLoop()
    : looping_(false), threadId_(muduo::CurrentThread::tid())
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
    assert(!looping_);    //确保只开启一次
    assertInLoopThread(); //保证在当前线程
    looping_ = true;
    ::poll(NULL, 0, 5 * 1000); //开始等待请求到来(在这暂时还未写入操作)

    LOG_TRACE << "EventLoop" << this << "stop looping";
    looping_ = false;
}
