#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<stdlib.h>
ssize_t Readn(int fd,void*vptr,size_t n)
{
    size_t nleft;  //剩余未读取的字节数
    ssize_t nread; //int 实际读取到的字节数
    char*ptr;
    ptr=(char*)vptr;
    nleft=n;
    while(nleft>0)
    {
        if(nread=read(fd,ptr,nleft)<0)
        {
            if(errno==EINTR)
            nread=0;
            else
            return -1;

        }
        else if(nread==0)
        break;
        nleft-=nread;
        ptr+=nread;
    }
    return n-nleft;
}
static ssize_t my_read