在通信过程中，套接字都是成对出现的              
一端的发送缓冲区对应对端的接收缓冲区。我们使用的是同一个文件描述符索引发送缓冲区和接受缓冲区。
>一个文件名描述符指向一个缓冲区（该套接字内部由内核借助两个缓冲区实现） 
# 网络字节序
小端法：(pc本地存储)高位存高地址，低位存低地址
大端法：()高位存低地址，低位存高地址
网络数据流应采用大端法字节
为了网络程序具有可移植性，使用同样的c代码在大端和小端计算机上编译后都正常运行，可以使用一下库函数做网络编程和主机字节序的转换
>
```c
#incluide<arpa/inet.h>
uint32_t htonl(uint32_t hostlong)  //本地->网络(IP)
uint16_t htons(uint16_t hostshort) //本地->网络(port)
uint32_t ntohl(uint32_t netlong)   //网络->本地(IP)
uint16_t ntohs(uint16_t netshort)  //网络->本地(port)
```
h表示host，n表示network，l表示32位长整数,s表示16位短数
如果主机是小端字序，这些函数将参数作相应的大小端转换后返回，如果主机是大端字节序，这些函数不做转换，将参数原封不动地返回

# IP地址转换函数
```c
#include<arpa/inet.h>
int inet_pton(int af,vhar*src,void*dst);
//本地字节序(string IP)--->网络字节序
af:AF_INET,AF_NET6
src:传入参数，IP地址(点分十进制)
dst：传出参数，网络字节序的IP
const char *inet_ntop(int af,const void *src,char *dst,socklen_t size);
//网络字节序--->本地字节序(string IP)
af:AF_INET,AF_NET6
src:传入参数，网络字节序IP地址
dst：传出参数，本地字节序
size：dst的大小
```
上面函数的返回值
成功返回1
失败返回-1
异常返回0  

# sockaddr地址结构：
```c
struct sockaddr_in addr;
addr.sin_family=AF_INET/AF_INET6;
addr.sin_port=htons(9527);
//int dst;
//inet_pton(AF_INET,"192.157.22.45",(void*)&dst);
addr.sin_addr.s_addr=dst;//但通常不这样用
addr.sin_addr.s_addr=ptonl(INADDE_ANY);//取出系统中有效的任意IP地址。(二进制类型)
bind(fd,(struct sockaddr*)&addr,size);
```
# socket
```c
#include<sys/socket.h>
socket(AF_ITET)