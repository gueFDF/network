#include <iostream>
#include "protobuf/echo_server.pb.h"

using namespace google::protobuf;

using namespace echo;


void EchoService_Stub::Echo(RpcController *controller,
                            const EchoRequest *request,
                            EchoResponse *response,
                            Closure *done)
{
    channel_->CallMethod(descriptor()->method(0),
                         controller,
                         request,
                         response,
                         done);
}

void EchoService_Stub::Add(RpcController *controller,
                           const AddRequest *request,
                           AddResponse *response,
                           Closure *done)
{
    channel_->CallMethod(descriptor()->method(1),
                         controller,
                         request,
                         response,
                         done);
}