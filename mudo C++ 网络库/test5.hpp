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
        holder_ = gettid();
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

#define MuteLockGuard(x) static_assert(false, "missing name");
//防止产生那个一个匿名对象 MuteLockGuard

//条件变量的防止封装
class Condition : boost::noncopyable
{

public:
    explicit Condition(Mutexlock &mutex) : mutex_(mutex)
    {
        pthread_cond_init(&pcond_, NULL);
    }
    ~Condition()
    {
        pthread_cond_destroy(&pcond_);
    }
    void wait()
    {
        pthread_cond_wait(&pcond_, mutex_.getPthreadMutex());
    }
    void notify()
    {
        pthread_cond_signal(&pcond_);
    }
    void notifyAll()
    {
        pthread_cond_broadcast(&pcond_);
    }

private:
    Mutexlock &mutex_;
    pthread_cond_t pcond_;
};

class CountDownLatch
{
public:
    CountDownLatch(int count)
    :mutex_(),condition_(mutex_),count_(count)
    {

    }

private:
    mutable Mutexlock mutex_;
    Condition condition_;
    int count_;
};

//该类并未分装完成，后续完成