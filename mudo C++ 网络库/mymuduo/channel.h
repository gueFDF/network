#pragma once 

#include <functional>
//每一个fd都会封装成一个channel

class Poller;
class EventLoop;
class Channel 
{

public:
    typedef std::function<void()> EventCallback;
    Channel(EventLoop *loop, int fd);

    void handleEvent(); //回调事件处理函数
    //设置回调的三个函数
    void setReadCallback(const EventCallback &cb)
    {
        readCallback_ = cb;
    }
    void setwriteCallback(const EventCallback &cb)
    {
        writeCallback_ = cb;
    }
    void seterrorCallback(const EventCallback &cb)
    {
        errorCallback_ = cb;
    }

    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_revents(int revt) { revents_ = revt; }
    //判断是否有关注的事件
    bool isNoneEvent() const { return events_ == kNoneEvent; }

    //加入读事件关注
    void enableReading()
    {
        events_ |= kReadEvent;
        update();
    }
    //加入写事件关注
    void enableWriting()
    {
        events_ |= kWriteEvent;
        update();
    }

    // for poller
    int index() { return index_; }
    void set_index(int idx) { index_ = idx; }

    EventLoop *ownerLoop() { return loop_; }

private:
    void update();
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_; //指向一个loop
    const int fd_;    //该channel绑定的fd
    int events_;      //关心的事件
    int revents_;     //实际发生的事件
    int index_;

    //三个回调函数
    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
};