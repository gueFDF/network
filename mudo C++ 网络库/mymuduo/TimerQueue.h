#include <vector>
#include <set>
#include "muduo/base/Timestamp.h"
#include <muduo/net/TimerId.h>
#include<muduo/net/Callbacks.h>
#include "channel.h"
class EventLoop;
typedef muduo::net::TimerId TimerId;
typedef muduo::net::Timer Timer;
typedef muduo::net::TimerCallback TimerCallback;

class TimerQueue
{
public:
    TimerQueue(EventLoop *loop);
    ~TimerQueue();
    TimerId addTimer(const TimerCallback &cb, muduo::Timestamp when, double interval); //向队列里添加定时器

private:
    typedef std::pair<muduo::Timestamp, Timer *> Entry; //存放到期时间和指向其的定时器
    typedef std::set<Entry> TimerList;
    void handlRead();
    std::vector<Entry> getExpired(muduo::Timestamp now); //返回超时的定时器列表

    bool insert(Timer *timer);
    EventLoop *loop_;
    const int timerfd_;
    Channel timerfdChannel_;
    TimerList timers_;
};