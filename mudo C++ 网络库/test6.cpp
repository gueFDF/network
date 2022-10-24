#include <iostream>
#include <thread>
#include <pthread.h>
#include <boost/utility.hpp>
template <typename T>
class Singleton : boost::noncopyable
{
public:
    static T &instance()
    {
        pthread_once(&ponce_, Singleton::init());
    }

private:
    Singleton();
    ~Singleton();
    static void init()
    {
        value_ = new T();
    }

private:
    static pthread_once_t ponce_;
    static T *value_;
};
//静态成员变量的初始化
template<typename T>
pthread_once_t Singleton<T>::ponce_=PTHREAD_ONCE_INIT;
template<typename T>
T*Singleton<T>::value_=NULL;


