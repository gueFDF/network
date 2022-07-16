#include <json/json.h>
#include <string>
#include<fstream>
#include <iostream>
using namespace std;
using namespace Json;
/*
struct message
{
    我的昵称：路飞
    我的头像：路飞.png
    我的消息：你吃饭了吗？
    消息时间：2022-07-16 14:35
}
*/
void writejson()
{
    Value root;
    root["我的昵称"] = "路飞";
    root["我的头像"] = "路飞.png";
    root["我的消息"] = "你吃饭了吗";
    root["消息时间"] = "2022-07-16 14:35";
#if 0
    //有格式字符串
    string json = root.toStyledString();
#else
    FastWriter f;
    string json = f.write(root);
#endif
    ofstream ofs("test.json");
    ofs<<json;
    ofs.close();
}
void readjson()
{
    ifstream ifs("test.json");
    Value root;
    Reader r;
    r.parse(ifs,root);
    string json=root.toStyledString();
    cout<<json<<endl;
}
int main()
{
    writejson();
    readjson();
    return 0;
}