#include <iostream>
#include "addressbook.pb.h"
using namespace std;

int main()
{
    tutorial::AddressBook p;

    tutorial::Person *peolpe = p.add_people();
    peolpe->set_name("guotianyu");
    peolpe->set_id(123456);
    peolpe->set_email("3237455241@qq.com");
    tutorial::Person_PhoneNumber*m= peolpe->add_phones();
    
    peolpe = p.add_people();
    peolpe->set_name("guotianyu");
    peolpe->set_id(123456);
    peolpe->set_email("3237455241@qq.com");


    cout<<p.people_size()<<endl;
    cout << p.DebugString() << endl;
}