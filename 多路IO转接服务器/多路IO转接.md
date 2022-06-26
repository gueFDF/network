# select
```c
       int select(int nfds, fd_set *readfds, fd_set *writefds,fd_set *exceptfds, struct timeval *timeout);
```
```

nfds:监听的多有文件描述符中，最大的文件描述符+1
readfds:读文件描述符监听合集(传入传出参数)
writefds：写文件描述符监听集合(传入传出参数)
exceptfds:异常文描述符监听集合(传入传出参数)
timeout  >0 设置监听超时时长
        NULL 阻塞监听
         =0 非阻塞监听，轮查询
返回值：
     >0 所有监听集合(3个),满足对应事件的总数
     0 没有满足监听条件的文件描述符、
     
     -1 errno
```

# select优缺点：
缺点:监听上限受文件描述符限制，最大1024
    检测满足条件的fd，自己添加业务逻辑提高小，提高编码难度。
优点:跨平台。win,linux,macOS,Unix,等等.
