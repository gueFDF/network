#include<functional>
#include<poll.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
class Acceptor
{
public:
    typedef std::function<void(int sockfd,const)>
}