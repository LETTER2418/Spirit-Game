#include "SocketManager.h"

SocketManager::SocketManager()
{
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		std::cout << "WSAStartup Fail\n";
	}

}
SocketManager::~SocketManager()
{

	if (ClientSocket != INVALID_SOCKET)
	{
		closesocket(ClientSocket);
	}
	if (ServerSocket != INVALID_SOCKET)
	{
		closesocket(ServerSocket);
	}
	WSACleanup();
}
void SocketManager::ServerTest()
{
	Json::Value msg = CreateMsg();
	for (auto Client : ClientSockets)
	{
		ServerSendMsg(Client, msg);
	}
}
void SocketManager::ClientTest()
{
	Json::Value msg = CreateMsg();
	ClientSendMsg(msg);
}
void SocketManager::Test()//Server send to Server Fail!!!!!!!!!!!!!!!!!!
{
	Json::Value msg = CreateMsg();
	std::string str_msg = msg.toStyledString();
	int size = send(ServerSocket, str_msg.c_str(), str_msg.length(), 0);
	//std::cout << "服务端向" << ServerSocket << "发送数据" << str_msg.c_str() << "\n";
	if (size <= 0)
	{
		if (GetLastError() == WSAEWOULDBLOCK)
		{
			std::cout << "Send data failure！\n";
		}
		else
		{
			std::cout << ServerSocket << " is closed" << "\n";
		}
		error = 1;
		return;
	}
	ServerRecMsg(ServerSocket);
	return;
}
Json::Value SocketManager::CreateMsg()
{
	Json::Value object;
	Json::Reader reader;
	Json::Value array;
	array.append(1);
	object["name"] = "LETTTER";
	object["age"] = "18";
	object["array"] = array;
	return object;
}
bool SocketManager::StartServer()
{
	//创建套接字
	if ((ServerSocket = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
	{
		std::cout << "Server创建套接字失败！错误代码：" << WSAGetLastError() << "\n";
		return false;
	}

	//设置缓冲区大小
	if (setsockopt(ServerSocket, SOL_SOCKET, SO_SNDBUF, (const char*)&SendBufferSize, sizeof(SendBufferSize)) < 0) {
		std::cout << "Error setting send buffer size\n";
	}
	if (setsockopt(ServerSocket, SOL_SOCKET, SO_RCVBUF, (const char*)&RecBufferSize, sizeof(RecBufferSize)) < 0) {
		std::cout << "Error setting receive buffer size\n";
	}

	//检查实际设置的缓冲区大小
	int actual_send_buf_size, actual_recv_buf_size;
	socklen_t optlen = sizeof(actual_send_buf_size);
	getsockopt(ServerSocket, SOL_SOCKET, SO_SNDBUF, (char*)&actual_send_buf_size, &optlen);
	std::cout << "Server actual send buffer size: " << actual_send_buf_size << " bytes" << "\n";

	getsockopt(ServerSocket, SOL_SOCKET, SO_RCVBUF, (char*)&actual_recv_buf_size, &optlen);
	std::cout << "Server actual receive buffer size: " << actual_recv_buf_size << " bytes" << "\n";

	//设置为非阻塞方式
	u_long mode = 1;
	if (ioctlsocket(ServerSocket, FIONBIO, &mode) == SOCKET_ERROR)
	{
		std::cout << "ioctlsocket failure, error:" << GetLastError() << "\n";
		return false;
	}

	//绑定端口和Ip
	sockaddr_in addr;
	int addr_len = sizeof(struct sockaddr_in);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SERVER_PORT);
	addr.sin_addr.s_addr = INADDR_ANY;
	//	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);//绑定本机的环回地址

	if (SOCKET_ERROR == bind(ServerSocket, (SOCKADDR*)&addr, sizeof(sockaddr_in)))
	{
		//第二次StartServer后就会地址绑定失败
		//std::cout << "地址绑定失败！错误代码：" << WSAGetLastError() << "\n";
		closesocket(ServerSocket);//modify
		return false;
	}

	//将套接字设为监听状态
	int size = listen(ServerSocket, 3);//第二个参数指定等待连接队列的最大长度
	std::cout << "server start listening\n";
	if (size != 0)
	{
		std::cout << "listen函数调用失败！\n";
		closesocket(ServerSocket);
		return false;
	}
	return true;
}
bool SocketManager::ServerAcceptClient()
{
	//初始化为空集
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);

	//添加一个新的文件描述符
	FD_SET(ServerSocket, &readfds);
	FD_SET(ServerSocket, &writefds);
	FD_SET(ServerSocket, &exceptfds);

	//select函数的阻塞性依赖于是非阻塞还是阻塞套接字, 通常设置非阻塞
	//使用select函数检查客户端SOCKET的可读性,否则GetLastError() == WSAEWOULDBLOCK GG
	//select函数可以实现I/O多路复用
	select(0, &readfds, &writefds, nullptr, &timeout);

	if (!FD_ISSET(ServerSocket, &readfds))//返回1当有新的连接请求到达或已连接的客户端向服务端发送数据
	{
		return 0;
	}
	std::cout << "server accept success\n";
	sockaddr_in  client_addr;
	int addr_len = sizeof(client_addr);
	auto ClientSocket = accept(ServerSocket, (struct sockaddr*)&client_addr, &addr_len);
	if (ClientSocket == INVALID_SOCKET)
	{
		if (GetLastError() == WSAEWOULDBLOCK)
		{
			std::cout << "Server accept error！" << "\n";
		}
		else
		{
			std::cout << "accept函数调用失败！网络异常程序退出" << "\n";
			closesocket(ServerSocket);
		}
		return 0;
	}
	else
	{
		std::cout << "服务器成功和第" << (int)ClientSockets.size() + 1 << "个客户端建立连接！" << "\n";
		ClientSockets.push_back(ClientSocket);
		return 1;
	}
}
bool  SocketManager::ServerSendMsg(SOCKET Client, Json::Value msg)
{
	//待优化,可以设置一个消息池,使用select函数检查消息池的客户端的可读/写性

	 //牢记:select用于ServerSocket时只能用于判断是否有连接请求,不能用于判断服务端是否能数据读写
	//发送的消息出现问题 ****烫烫烫(Msg缓冲区超出Client缓冲区大小)
	if (msg.isObject() && msg["type"] == 2)//Json有多种数据类型,只有对象才能使用[""]访问成员!!!否则程序会直接退出
	{
		//std::cout << msg << "\n";
	}
	std::string str_msg = msg.toStyledString();
	int size = send(Client, str_msg.c_str(), SendBufferSize, 0);
	/*char msg1[SendBufferSize] = "LETTER";
	int size = send(Client, msg1, SendBufferSize, 0);
	Sleep(1000);
	static int cnt = 0;
	std::cout << cnt++ << "\n";*/
	if (size <= 0)
	{
		//现在发送失败，试一下是不是json的问题
		std::cout << "ServerSendMsg error" << GetLastError() << "\n";
		error = 1;
		return 0;
	}
	return 1;
}
Json::Value SocketManager::ServerRecMsg(SOCKET Client)
{
	//初始化为空集
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);

	//添加一个新的文件描述符
	FD_SET(ServerSocket, &readfds);
	FD_SET(ServerSocket, &writefds);
	FD_SET(ServerSocket, &exceptfds);

	select(0, &readfds, &writefds, nullptr, &timeout);

	if (!FD_ISSET(ServerSocket, &readfds))
	{
		return Json::nullValue;
	}
	int size = recv(Client, RecMsgBuffer, sizeof(RecMsgBuffer), 0);
	//size==-1可能是非阻塞模式下没有收到数据
	if (size == 0)
	{
		std::cout << "Server closed\n";
	}
	if (size > 0)
	{
		//std::cout << Client << "说：" << RecMsgBuffer << "\n";
	}
	Json::Value val;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(RecMsgBuffer, val);
	if (parsingSuccessful)
	{
		return val;
	}
	else
	{
		return Json::nullValue;
	}
}
void SocketManager::ServerRecClientsMsg()
{
	for (auto it = ClientSockets.begin(); it != ClientSockets.end();)
	{
		//先收到客户端的消息再发送指令
		Json::Value msg = ServerRecMsg(*it);
		if (error == 0)
		{
			it++;
		}
		else
		{
			it = ClientSockets.erase(it);
		}
	}
}
std::vector<SOCKET> SocketManager::GetClientSockets()
{
	if (!ClientSockets.size())
	{
		std::cout << "get a empty ClientSocket!!! \n";
	}
	return ClientSockets;
}
void SocketManager::StartClient()
{
	//目前来看connect失败

	// 创建客户端套接字
	ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ClientSocket == INVALID_SOCKET) {
		std::cout << "Client创建套接字失败！错误代码：" << WSAGetLastError() << "\n";
		return;
	}
	if (setsockopt(ClientSocket, SOL_SOCKET, SO_SNDBUF, (const char*)&SendBufferSize, sizeof(SendBufferSize)) < 0) {
		std::cout << "Error setting send buffer size\n";
	}
	if (setsockopt(ClientSocket, SOL_SOCKET, SO_RCVBUF, (const char*)&RecBufferSize, sizeof(RecBufferSize)) < 0) {
		std::cout << "Error setting receive buffer size\n";
	}
	//检查实际设置的缓冲区大小
	int actual_send_buf_size, actual_recv_buf_size;
	socklen_t optlen = sizeof(actual_send_buf_size);
	getsockopt(ClientSocket, SOL_SOCKET, SO_SNDBUF, (char*)&actual_send_buf_size, &optlen);
	std::cout << "Client actual send buffer size: " << actual_send_buf_size << " bytes\n";

	getsockopt(ClientSocket, SOL_SOCKET, SO_RCVBUF, (char*)&actual_recv_buf_size, &optlen);
	std::cout << "Client actual receive buffer size: " << actual_recv_buf_size << " bytes\n";

	/* 客户端好像没必要设置非阻塞,而且非阻塞会bind失败
	 设置客户端套接字为非阻塞模式
	 unsigned long mode = 1;
	 if (ioctlsocket(ClientSocket, FIONBIO, &mode) != 0) {
		 printf("ioctlsocket failed with error code : %d\n", WSAGetLastError());
		 closesocket(ClientSocket);
		 WSACleanup();
		 return ;
	 }*/

	 // 设置服务器地址
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);//绑定本机的环回地址

	// 连接到服务器
	int connectResult = connect(ClientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (connectResult == SOCKET_ERROR) {
		int errorCode = WSAGetLastError();
		if (errorCode != WSAEWOULDBLOCK) {
			printf("Connect failed with error code : %d\n", errorCode);
			//10061可能原因:端口不一致；没有开启服务端
			return;
		}
	}
	std::cout << "StartClient success\n";
}

Json::Value SocketManager::ClientRecMsg()
{
	//std::cout << " ClientRecMsg begin and recv function begin\n";
	//int state = recv(ClientSocket, RecMsgBuffer, sizeof(RecMsgBuffer), 0);
	memset(RecMsgBuffer, '\0', sizeof(RecMsgBuffer));
	int state = recv(ClientSocket, RecMsgBuffer, 30, 0);
	Json::Value val;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(RecMsgBuffer, val);
	//std::cout << " recv function end\n";
	std::cout << "Client rec " << val << " " << RecMsgBuffer <<" "<< parsingSuccessful << "\n";
	Sleep(2000);
	if (state == 0)
	{
		std::cout << "服务端已关闭连接\n";
		return Json::nullValue;
	}
	else if (state != SOCKET_ERROR)
	{
		Json::Value val;
		Json::Reader reader;
		bool parsingSuccessful = reader.parse(RecMsgBuffer, val);
		if (parsingSuccessful)
		{
			//std::cout << "Clien" << ClientSocket << " received " << val["type"] << "\n";
			//std::cout << " ClientRecMsg end\n";
			return val;
		}
	}
	return Json::nullValue;
}
void SocketManager::ClientSendMsg(Json::Value msg)
{
	/*char msg1[]="hi";
	send(ClientSocket, msg1, 2, 0);
	static int cnt = 0;
	std::cout <<client<<"send "<< ++cnt << "th msg\n";
	return;*/
	std::string str_msg = msg.toStyledString();
	int size = send(ClientSocket, str_msg.c_str(), str_msg.length(), 0);
	//std::cout << "Client send " << str_msg.length() << "\n";
	if (size == SOCKET_ERROR)
	{
		std::cout << "发送信息失败" << WSAGetLastError() << "\n";
	}
	else
	{
		if (msg.isObject())
		{
			std::cout << "Client" << ClientSocket << " sent " << msg["type"] << "\n";
		}
		else
		{
			std::cout << "Client" << ClientSocket << " sent " << msg << "\n";
		}
	}
}
SOCKET SocketManager::GetClientSocket()
{
	return ClientSocket;
}
