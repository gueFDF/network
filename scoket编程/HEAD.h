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
ssize_t Writen(int fd,const void *vptr,size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    const char*ptr;
    ptr=(char*)vptr;
    nleft=n;
    while(nleft>0)
    {
        if((nwritten=write(fd,ptr,nleft))<=0)
        {
            if(nwritten<0&&errno==EINTR)
            nwritten=0;
            else 
            return -1;
        }
        nleft-=nwritten;
        ptr+=nwritten;
    }
    return n;
}
ssize_t Readline(int fd,void*vptr,size_t maxlen)
{
    ssize_t n,rc;
    char c,*ptr;
    ptr=(char*)vptr;
    for(n=1;n<maxlen;n++)
    {
        again:
        if((rc=read(fd,&c,1))==1)
        {
            *ptr++=c;
            if(c=='\n')
            break;
        }
        else if(rc==0)
        {
            *ptr=0;
            return (n-1);
        }
        else
        {
            if(errno==EINTR)
            goto again;
              return(-1);
        }
    }
    *ptr=0;
    return n;
}