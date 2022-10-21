// #include<iostream>
// #include<memory>
// #include<map>
// #include<string>
// using namespace std;
// void test(std::shared_ptr<int>&p)
// {
//     (*p)++;
//     std::cout<<p.use_count()<<std::endl;
// }

// class Person
// {
// public:
//     int a;
//     Person():a(0){};
// };
// class _map
// {
// public:

//     map<string,shared_ptr<Person>> mp;

// };
// int main()
// {
//     // std::shared_ptr<int> ptr(new int(10));
//     // std::cout<<ptr.use_count()<<std::endl;
//     // test(ptr);
//     // std::cout<<ptr.use_count()<<std::endl;
//     // std::cout<<*ptr<<std::endl;
//     shared_ptr<Person> a1(new Person);
//     _map arr;
//     arr.mp.insert(make_pair("1234",a1));
//     return 0;
// }
#include <iostream>
#include <map>
#include <memory>
#include <cstdio>
#include <utility>
#include <cstdlib>
#include<functional>
using namespace std;
int b;
class abc : public enable_shared_from_this<abc>
{
public:
    int a;
    abc(int b) : a(b) {}
    shared_ptr<abc> show()
    {
        cout << a << endl;
        return shared_from_this();
    }
};

void print(unique_ptr<string> &a)
{
    cout << *a << endl;
}
void test1()
{

    string st("1234");
    auto a = make_shared<abc>(1);
    cout << a.use_count() << endl;
    weak_ptr<abc> p = a;

    cout << a.use_count() << endl;

    // {

    //     map<string, shared_ptr<abc>> m;

    //     b = a.use_count();

    //     cout << a.use_count() << endl;

    //     m.insert(make_pair(st, a));

    //     b = a.use_count();

    //     cout << a.use_count() << endl;
    // }
    // cout << a.use_count() << endl;

    auto y = make_unique<string>("adasdsa");
    // auto c=move(y);

    print(y);
    cout << *y << endl;
}

double callableFunc (double x, double y) {return x/y;}
int main()
{

//     char *p = new char('a');
//     // char *p;

//     printf("%p\n", p);
//     //  cout << p << endl;
//     delete p;
//     p = nullptr;
//    free(p);
    // test1();
    
auto NewCallable = std::bind (callableFunc, std::placeholders::_1,2);  
std::cout << NewCallable (10) << '\n'; 

}