#include <iostream>
#include "protobuf/echo_server.pb.h"
using namespace echo;
using namespace google::protobuf;

class EchoServiceImpl : public EchoService
{
public:
	void Echo(RpcController *controller,
			  EchoRequest *request,
			  EchoResponse *response,
			  Closure *done)
	{
		// 获取请求消息，然后在末尾加上信息：", welcome!"，返回给客户端
		response->set_message(request->message() + ", welcome!");
		done->Run();
	}

	void Add(RpcController *controller,
			 AddRequest *request,
			 AddResponse *response,
			 Closure *done)
	{
		// 获取请求数据中的 2 个整型数据
		int32_t a = request->a();
		int32_t b = request->b();

		// 计算结果，然后放入响应数据中
		response->set_result(a + b);

		done->Run();
	}
};
