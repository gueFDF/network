#include <iostream>
#include <mutex>
#include <unistd.h>
#include <set>
#include <thread>
#include <cstdio>


#include <unistd.h>
#include <sys/syscall.h>
pid_t gettid()
{
    return static_cast<pid_t>(syscall(SYS_gettid));
}
class Request;
class Inventory
{
public:
    void add(Request *req)
    {
        printf("thread %d want get the lock\n", gettid());
        std::lock_guard<std::mutex> lk(mutex_);
        printf("thread %d get the lock\n", gettid());
        request_.insert(req);
    }
    void remove(Request *req)
    {
        printf("thread %d want get the lock\n",gettid());
        std::lock_guard<std::mutex> lk(mutex_);
        printf("thread %d get the lock\n", gettid());
        request_.erase(req);
    }
    void printAll() const;

private:
    mutable std::mutex mutex_;
    std::set<Request *> request_;
};

Inventory g_inventory; //全局变量

class Request
{
public:
    void process()
    {
        printf("thread %d want get the clock\n", gettid());
        std::lock_guard<std::mutex> lk(mutex_);
        printf("thread %d get the clock\n", gettid());
        g_inventory.add(this);
    }
    ~Request() __attribute__((noinline)) //组织编译器进行内联
    {
        printf("thread %d want get the clock\n", gettid());
        std::lock_guard<std::mutex> lk(mutex_);
        printf("thread %d get the clock\n",gettid());
        sleep(1);
        g_inventory.remove(this);
    }
    void print() const __attribute__((noinline))
    {
        printf("thread %d want get the clock\n", gettid());
        std::lock_guard<std::mutex> lk(mutex_);
        printf("thread %d get the clock\n", gettid());
    }

private:
    mutable std::mutex mutex_;
};

void Inventory::printAll() const
{
    printf("thread %d want get the lock\n", gettid());
    std::lock_guard<std::mutex> lk(mutex_);
    printf("thread %d get the lock\n", gettid());
    sleep(1);
    for (auto it = request_.begin(); it != request_.end(); ++it)
    {
        (*it)->print();
    }
    printf("Inventory::printfAll() unlock\n");
}
void threadFunc()
{
    Request *req = new Request;
    req->process();
    delete req;
}

int main()
{
    std::thread thread_(threadFunc);
    usleep(500 * 1000);
    g_inventory.printAll();
    thread_.join();
}