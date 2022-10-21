//对MutexLock和MutexLockGruard ，Condition进行了封装

#include <thread>
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include <sys/syscall.h>

#include <boost/utility.hpp>
pid_t gettid()
{
    return static_cast<pid_t>(syscall(SYS_gettid));
}
class Mutexlock : boost::noncopyable
{
public:
    Mutexlock() : holder_(0)
    {
        pthread_mutex_init(&mutex_, NULL); //初始化互斥锁
    }
    ~Mutexlock()
    {
        assert(holder_ == 0);
        pthread_mutex_destroy(&mutex_); //销毁
    }
    bool isLockedByThisThread()
    {
        return holder_ == gettid();
    }
    void assertLocked()
    {
        assert(isLockedByThisThread());
    }
    void lock()
    {
        pthread_mutex_lock(&mutex_);
        holder_=gettid();
    }
    void unlock()
    {
        holder_ = 0;
        pthread_mutex_unlock(&mutex_);
    }
    pthread_mutex_t *getPthreadMutex()
    {
        return &mutex_;
    }

private:
    pthread_mutex_t mutex_;
    pid_t holder_;
};

// MutexLockGruard的封装

class MuteLockGuard : boost::noncopyable
{
public:
    explicit MuteLockGuard(Mutexlock &mutex) : mutex_(mutex)
    {
        mutex_.lock();
    }
    ~MuteLockGuard()
    {
        mutex_.unlock();
    }

private:
    Mutexlock &mutex_;
};

#define MuteLockGuard(x) static_assert(false,"missing name");
//防止产生那个一个匿名对象 MuteLockGuard