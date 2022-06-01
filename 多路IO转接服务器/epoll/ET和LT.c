#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <stdlib.h>
#define MAXLINE 10
int main()
{
    int ret;
    int efd, i;
    int pfd[2];
    char buf[BUFSIZ], ch = 'a';
    pipe(pfd);
    int pid = fork();
    if (pid < 0)
    {
        perror("fork error");
        exit(1);
    }
    else if (pid > 0) //父进程
    {

        close(pfd[0]); //关闭写端
        while (1)
        {
            for (i = 0; i < MAXLINE / 2; i++)
                buf[i] = ch;
            buf[i - 1] = '\n';
            ch++;
            if (ch > 'z')
                ch = 'a';
            for (; i < MAXLINE; i++)
                buf[i] = ch;
            buf[i - 1] = '\n';
            ch++;
            if (ch > 'z')
                ch = 'a';
            write(pfd[1], buf, MAXLINE);
            sleep(3);
        }
        close(pfd[1]);
    }
    else if (pid == 0) //子进程
    {
        close(pfd[1]);
        struct epoll_event temp;
        struct epoll_event resevent[10];
        efd = epoll_create(10);
        temp.events = EPOLLIN; // LT触发
        // temp.events = EPOLLIN | EPOLLET; // ET触发
        temp.data.fd = pfd[0];
        epoll_ctl(efd, EPOLL_CTL_ADD, pfd[0], &temp);
        while (1)
        {
            ret = epoll_wait(efd, resevent, 10, -1);
            if (resevent[0].data.fd == pfd[0])
            {
                int len = read(pfd[0], buf, MAXLINE / 2);
                write(1, buf, len);
            }
        }
        close(pfd[0]);
        close(efd);
    }

    return 0;
}