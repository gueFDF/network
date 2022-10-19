#define _GNU_SOURCE 1
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
int main(int argc, char *argv[])
{
    if (argc <= 2)
    {
        printf("usage:%s ip_address port_number\n", basename(argv[0]));
        return 1;
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    int ret = bind(sock, (struct sockaddr *)&address, sizeof(address));
    assert(ret != -1);
    //  ret = listen(sock, 5);
    assert(ret != -1);
    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    // int connfd = accept(sock, (struct sockaddr *)&client, &client_addrlength);

    struct sockaddr_in copy;
    memcpy(&copy, &address, sizeof(address));

    copy.sin_port = htons(10000);

    while (1)
    {
        int connfd = connect(sock, (struct sockaddr *)&copy, sizeof(address));
        if (connfd == 0)
        {
            printf("conne success\n");
            break;
        }
        // printf("connectfd = %d\n", connfd);
        // printf("connfd = %d errno = %d\n", connfd, errno);
        // perror("connect");
    }

    // int pipefd[2];
    // assert(ret != -1);
    // ret = pipe(pipefd); /*创建管道*/
    // while (1)
    // {
    //     /*将connfd上流入的客户数据定向到管道中*/
    //     ret = splice(connfd, NULL, pipefd[1], NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
    //     if (ret <= 0)
    //     {
    //         printf("客户端关闭\n");
    //         goto flag;
    //     }
    //     printf("ret : %d\n", ret);
    //     /*将管道的输出定向到connfd客户连接文件描述符*/
    //     ret = splice(pipefd[0], NULL, connfd, NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
    //     if (ret <= 0)
    //     {
    //         printf("客户端关闭\n");
    //         goto flag;
    //     }
    // }
    char buf[1024];
    while (1)
    {
        // ret = read(STDIN_FILENO, buf, sizeof(buf));
        // printf("ret = %d\n", ret);
        // ret = write(sock, buf, ret);
        // printf("ret = %d\n", ret);
        // // write(cfd,buf,)
        // ret = read(sock, buf, sizeof(buf));
        // printf("ret = %d\n", ret);
        // if (ret <= 0)
        // {
        //     close(sock);
        //     return 0;
        // }

        int pid = fork();
        if (pid == 0) //父进程
        {
            //进行写
            ret = read(STDIN_FILENO, buf, sizeof(buf));
            ret = write(sock, buf, ret);
        }
        else
        {
            //进行读
            ret = read(sock, buf, sizeof(buf));
            write(STDOUT_FILENO, buf, ret);
        }
    }

flag:
    close(sock);
    return 0;
}
