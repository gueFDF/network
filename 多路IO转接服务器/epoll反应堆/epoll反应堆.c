#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#define MAX_EVENTS 1024 //监听上限
#define BUFLEN 4096
#define SERV_PORT 8081
void recvdata(int fd, int events, void *arg);
void senddata(int fd, int events, void *arg);
//错误处理函数
void sys_error(const void *str)
{
    perror(str);
    exit(1);
}
//描述就绪文件相关信息
struct myevent_s
{
    int fd;                                           //要监听的文件描述符
    int events;                                       //对应监听事件
    int *arg;                                         //泛型参数
    void (*call_back)(int fd, int events, void *arg); //回调函数
    int status;                                       //是否在监听:1->在红黑树上(监听)，0->不在(监听)
    char buf[BUFLEN];
    int len;
    long last_active; //记录每次加入红黑树g_efd的时间值
};
int g_efd; //全局变量，保存监听红黑树的文件描述符
struct myevent_s g_events[MAX_EVENTS + 1];

//将结构体myevent_s成员变量初始化
void eventset(struct myevent_s *ev, int fd, void (*call_back)(int, int, void *), void *arg)
{
    ev->fd = fd;
    ev->call_back = call_back;
    ev->events = 0;
    ev->arg = arg;
    ev->status = 0;
    if(ev->len <= 0)
    {
        memset(ev->buf, 0, sizeof(ev->buf));
        ev->len = 0;
    }
    ev->last_active = time(NULL); //调用该函数的时间（时间戳）
}

//向监听红黑树上添加一根文件描述符
void eventadd(int efd, int events, struct myevent_s *ev)
{
    struct epoll_event epv = {0, {0}};
    int op;
    epv.data.ptr = ev;
    epv.events = ev->events = events; // EPOLLIN或EPOLLOUT
    if (ev->status == 0)
    {
        op = EPOLL_CTL_ADD; //将其加入红黑树g_efd,并将ststus置1
        ev->status = 1;
    }
    if (epoll_ctl(efd, op, ev->fd, &epv) < 0)
        printf("event add failed [fd=%d],events[%d]\n", ev->fd, events);
    else
        printf("event add OK [fd=%d],op=%d,events[%d]\n", ev->fd, op, events);
    return;
}

//从监听红黑树上摘除一个节点
void eventdel(int efd, struct myevent_s *ev)
{
    struct epoll_event epv = {0, {0}};
    if (ev->status != 1) //不在红黑树上
        return;
    epv.data.ptr = NULL;
    ev->status = 0;
    epoll_ctl(efd, EPOLL_CTL_DEL, ev->fd, &epv);
    return;
}

void senddata(int fd, int events, void *arg)
{
    struct myevent_s *ev = (struct myevent_s *)arg;
    int len;
    len = send(fd, ev->buf, ev->len, 0);
    eventdel(g_efd, ev);
    if (len > 0)
    {
        printf("send[fd=%d],[%d]%s\n", fd, len, ev->buf);
        eventset(ev, fd, recvdata, ev); //设置该fd对应的回调函数为senddata
        eventadd(g_efd, EPOLLIN, ev);   //将fd加入红黑树g_efd中，监听其写事件
    }
    else
    {
        close(ev->fd);
        printf("send[fd=%d] error[%d]:%s\n", fd, errno, strerror(errno));
    }
    return;
}

void recvdata(int fd, int events, void *arg)
{
    struct myevent_s *ev = (struct myevent_s *)arg;
    int len;
    len = recv(fd, ev->buf, sizeof(ev->buf), 0); //读文件描述符，数据存入myevent_s成员buf中
    eventdel(g_efd, ev);                         //将该节点从红黑树上摘除
    if (len > 0)
    {
        ev->len = len;
        ev->buf[len] = '\0';
        printf("recv[%d]:%s\n", fd, ev->buf);
        eventset(ev, fd, senddata, ev); //设置该fd对应的回调函数为senddata
        eventadd(g_efd, EPOLLOUT, ev);  //将fd加入红黑树g_efd中，监听其写事件
    }
    else if (len == 0)
    {
        close(ev->fd);
        // ev-g_events地址相减得到偏移元素
        printf("[fd=%d] pos [%ld],closed\n", fd, ev - g_events);
    }
    else
    {
        close(ev->fd);
        printf("recv[fd=%d] error[%d]:%s\n", fd, errno, strerror(errno));
    }
    return;
}

void acceptconn(int lfd, int events, void *arg)
{
    struct sockaddr_in cin; //客户端的地址结构
    socklen_t len = sizeof(cin);
    int cfd, i;
    if ((cfd = accept(lfd, (struct sockaddr *)&cin, &len)) == -1)
    {
        if (errno != EAGAIN && errno != EINTR)
        {
            //暂时不做任何处理
        }
        printf("accept error ,%s\n", strerror(errno));
        return;
    }
    do
    {
        for (i = 0; i < MAX_EVENTS; i++) //从全局数组g_events数组中找到一个空闲位置
            if (g_events[i].status == 0)
                break;
        if (i == MAX_EVENTS)
        {
            printf("max connect linnit[%d]\n", MAX_EVENTS); //已达到连接上限，全局数组已用完
            break;
        }
        int flag = 0;
        if (flag = fcntl(cfd, F_SETFL, O_NONBLOCK) < 0) //将cfd也设置为非阻塞
        {
            printf("fcntl nonblocking failed,%s\n", strerror(errno));
            break;
        }
        //给cfd设置一个myevent_s结构体，回调函数设置为recvdata
        eventset(&g_events[i], cfd, recvdata, &g_events);
        eventadd(g_efd, EPOLLIN, &g_events[i]);
    } while (0);
}

//初始化socket ，初始化lfd
void initlistensocket(int efd, short port)
{
    int ret; //接收返回值
    struct sockaddr_in sin;
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
        sys_error("socet error");
    fcntl(lfd, F_SETFL, O_NONBLOCK); //将socket设置为非阻塞
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    ret = bind(lfd, (struct sockaddr *)&sin, sizeof(sin));
    if (ret == -1)
        sys_error("bind error");
    listen(lfd, 20);
    eventset(&g_events[MAX_EVENTS], lfd, acceptconn, &g_events[MAX_EVENTS]);
    eventadd(efd, EPOLLIN, &g_events[MAX_EVENTS]);
    return;
}

int main(int argc, char *argv[])
{
    unsigned short port = SERV_PORT;
    if (argc == 2) //传入的有参数
        port = atoi(argv[1]);

    g_efd = epoll_create(MAX_EVENTS + 1); //创建监听红黑树
    if (g_efd <= 0)
        printf("creat efd in epoll_creat err %s\n", strerror(errno));
    initlistensocket(g_efd, port);             //初始化监听套接字(将socket ,bind,listn...进行封装)
    struct epoll_event events[MAX_EVENTS + 1]; //保存已经满足就绪事件的文件描述符数组
    printf("server running :port[%d]\n", port);
    int checkpos = 0, i;
    while (1)
    {
        //超时验证，每次测试100个链接，不测试listenfd 当客户端60秒内没有和服务器通信，则关闭此客户端连接
        /*long now = time(NULL);
        for (i = 0; i < 100; i++, checkpos++)
        {
            if (checkpos == MAX_EVENTS)
                checkpos = 0;
            if (g_events[checkpos].status != 1) //不在红黑树上
                continue;
            long duration = now - g_events[checkpos].last_active; //客户端不活跃的时间
            if (duration >= 60)
            {
                close(g_events[checkpos].fd); //超时关闭与该客户端的链接
                printf("[fd=%d] timeout\n", g_events[checkpos].fd);
                eventdel(g_efd, &g_events[checkpos]); //将该客户端从红黑树g_efd移除
            }
        }*/
        int nfd = epoll_wait(g_efd, events, MAX_EVENTS + 1, 1000);
        if (nfd < 0)
        {
            printf("epoll_wait error,exit\n");
            break;
        }
        for (i = 0; i < nfd; i++)
        {
            //使用自定义结构体myevent_s类型的指针，接收联合体data的void*ptr成员
            struct myevent_s *ev = (struct myevent_s *)events[i].data.ptr;
            if ((events[i].events & EPOLLIN) && ev->events & EPOLLIN) //读操作就绪
                ev->call_back(ev->fd, events[i].events, ev->arg);
            if ((events[i].events & EPOLLOUT) && ev->events & EPOLLOUT) //写操作就绪
                ev->call_back(ev->fd, events[i].events, ev->arg);
        }
    }
    //退出前释放所有资源
    return 0;
}