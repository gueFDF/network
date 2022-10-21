#include <iostream>
#include <memory>
#include <utility>
#include <string>
#include <mutex>
#include <map>
#include<functional>
class Stock
{
public:
    Stock(std::string p) :key(p){};
    std::string getkey()
    {
        return key;
    }
private:
    std::string key;
    int value;
};
class StockFactory : public std::enable_shared_from_this<StockFactory>
{
public:
    std::shared_ptr<Stock> get(const std::string &key) //从对象池中拿对象
    {
            std::shared_ptr<Stock> pStock;
            mutex_.lock();
            //std::lock_guard<std::mutex> lk (mutex_); 
            std::weak_ptr<Stock>& wkStock=stock_[key];  //通过key从对象池中拿取
            pStock=wkStock.lock();//获得提升的share_ptr()
            if(!pStock) //获取失败，说明该池自理没有想要的对象
            {
                //进行创建,创建新对象时，同时添加share_ptr的析构函数，目的是，在stock_中的成员析构时，可以从map中erase
                pStock.reset(new Stock(key),std::bind(&StockFactory::weakDeleteCallback,std::weak_ptr<StockFactory>(shared_from_this()),std::placeholders::_1)); //里面传入的是弱智能指针，
                //不延长他的生命周期，若已被析构，不执行其他操作
            }
            mutex_.unlock();
    }

private:
    static void weakDeleteCallback(const std::weak_ptr<StockFactory>&wkFactory,Stock*stock)
    {
        std::shared_ptr<StockFactory> factory(wkFactory.lock());//尝试提升
        if(factory)  //如果factory还在，就进行清理 stock_
        {
            factory->removeStock(stock);
        }
        delete stock;
    }

    void removeStock(Stock*stock)
    {
        if(stock)
        {
            mutex_.lock();
            stock_.erase(stock->getkey());
            mutex_.unlock();
        }
    }

private:
    mutable std::mutex mutex_;
    std::map<std::string, std::weak_ptr<Stock>> stock_;  //在map里存放对象的弱智能指针
};

int main()
{
    return 0;
}