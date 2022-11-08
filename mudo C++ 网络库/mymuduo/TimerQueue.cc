#include"TimerQueue.h"

std::vector<TimerQueue::Entry> TimerQueue::getExpired(muduo::Timestamp now)
{
    std::vector<Entry> expired;
    Entry sentry =std::make_pair(now,reinterpret_cast<Timer*>(UINTPTR_MAX));
    //从数组的begin位置到end-1位置二分查找第一个大于或等于num的数字
    TimerList::iterator it=timers_.lower_bound(sentry);  //查找超时定时器
    assert(it==timers_.end()||now<it->first);
    std::copy(timers_.begin(),it,std::back_inserter(expired));
    return expired;
}