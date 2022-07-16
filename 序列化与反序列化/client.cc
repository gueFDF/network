#include"Sock.hpp"
#include"message.hpp"
int main()
{
    int clifd=Sock::Socket();
    Sock::Connect(clifd,"127.0.0.1",9999);
    char buf[1024];
    string json;
    message temp;
    while(1)
    {
        temp.setinformation();
        json=temp.enter();
        write(clifd,json.c_str(),json.size());
    }
}