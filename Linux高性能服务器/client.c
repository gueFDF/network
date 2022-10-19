#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char buf[1024];
    assert(argc == 3);
    const char *ip = argv[1]; // ip地址
    int port = atoi(argv[2]); //端口号
    int cfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, ip, &address.sin_addr.s_addr);
    bind(cfd, (struct sockaddr *)&address, sizeof(address));

    struct sockaddr_in copy;
    memcpy(&copy, &address, sizeof(address));
    int ret;
    copy.sin_port = htons(9999);
    inet_pton(AF_INET, "192.168.30.139", &copy.sin_addr.s_addr);
    while (1)
    {
        ret = connect(cfd, (struct sockaddr *)&copy, sizeof(address));
        if (ret == 0) //连接成功
        {
            printf("conne success");
            break;
        }
         printf("connfd = %d errno = %d\n", ret, errno);
         perror("connect");
    }
    //连接成功
    {
        while (1)
        {

            // ret = read(STDIN_FILENO, buf, sizeof(buf));
            // printf("ret = %d\n", ret);
            // ret = write(cfd, buf, ret);
            // printf("ret = %d\n", ret);
            // // write(cfd,buf,)
            // ret = read(cfd, buf, sizeof(buf));
            // printf("ret = %d\n", ret);
            // if (ret <= 0)
            // {
            //     close(cfd);
            //     return 0;
            // }

            int pid = fork();
            if (pid == 0) //父进程
            {
                //进行写
                ret = read(STDIN_FILENO, buf, sizeof(buf));
                ret = write(cfd, buf, ret);
            }
            else
            {
                //进行读
                ret = read(cfd, buf, sizeof(buf));
                write(STDOUT_FILENO, buf, ret);
            }
        }
    }
}