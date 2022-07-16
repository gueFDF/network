#include <iostream>
#include <string>
#include <cstdlib>
#include <json/json.h>
#include <ctime>
#include<cstdio>
using namespace std;
using namespace Json;
struct message
{
private:
    string name;        //昵称
    string picture;     //暂时用string来代替图片
    string information; //消息
    string my_time;        //时间
public:
    message(string name1 = "封宇腾", string picture1 = "小企鹅.png") : name(name1), picture(picture1){};
    static string gettime()
    {
        time_t rawtime;
        struct tm *ptm;
        time(&rawtime);
        ptm = localtime(&rawtime);
        char data[60];
        sprintf(data, "%d-%02d-%02d %02d:%02d:%02d", (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday, (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
        move(string(data));
        return data;
    }
    void setinformation()
    {
        cin>>information;
    }
    string enter()
    {
        my_time=gettime();
        Value root;
        root.append(name);
        root.append(picture);
        root.append(information);
        root.append(my_time);
        FastWriter f;
        string json =f.write(root);
        return json;
    }
    void receive(string json)
    {
        Value root;
        Reader r;
        r.parse(json,root);
        int i=0;
        name=root[i++].asString();
        picture=root[i++].asString();
        information=root[i++].asString();
        my_time=root[i++].asString();
    
       
    }
    void print()
    {
        cout<<"昵称："<<name<<endl;
        cout<<"头像："<<picture<<endl;
        cout<<"时间："<<my_time<<endl;
        cout<<"消息："<<information<<endl; 
    }
};

