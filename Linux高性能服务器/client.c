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
    int ret = connect(cfd, (struct sockaddr *)&address, sizeof(address));
    if (ret == -1) //连接失败
    {
        perror("connect fale");
        exit(1);
    }
    else //连接成功
    {
        while (1)
        {
            printf("sdsdf\n");
            ret = read(STDIN_FILENO, buf, sizeof(buf));
            printf("ret = %d\n", ret);
            ret = write(cfd, buf, ret);
            printf("ret = %d\n", ret);
            // write(cfd,buf,)
            ret = read(cfd, buf, sizeof(buf));
            printf("ret = %d\n", ret);
            if (ret <= 0)
            {
                close(cfd);
                return 0;
            }
        }
    }
}