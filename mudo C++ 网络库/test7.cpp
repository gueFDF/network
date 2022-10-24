#include<iostream>
#include"test5.hpp"
#include<stdlib.h>
void someFunctionMayCallExit()
{
    exit(1);
}
class GlobalObject
{
public:
    void doit()
    {
        MuteLockGuard lock(mutex_);
        someFunctionMayCallExit();
    }
    ~GlobalObject()
    {
        printf("调用析构函数\n");
        MuteLockGuard lock(mutex_);
        //clean up
        printf("析构完成\n");
    }
private:
    Mutexlock mutex_;
};







GlobalObject g_obj;
int main()
{
    g_obj.doit();
}