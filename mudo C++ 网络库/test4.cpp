#include <iostream>
#include <mutex>

class Mutex
{
public:
    std::mutex mutex_;
};

int main()
{
    Mutex a;

    Mutex b;

    a.mutex_.lock();
    b.mutex_.lock();
    std::cout<<"sdfsf"<<std::endl;
    a.mutex_.unlock();
    b.mutex_.unlock();

}

