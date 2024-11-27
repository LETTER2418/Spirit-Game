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
	delete[]SendMsgBuffer;
	delete[]RecvMsgBuffer;
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

	//设置为非阻塞方式
	u_long mode = 1;
	if (ioctlsocket(ServerSocket, FIONBIO, &mode) == SOCKET_ERROR)
	{
		std::cout << "ioctlsocket failure, error:" << WSAGetLastError() << "\n";
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
	//使用select函数检查客户端SOCKET的可读性,否则WSAGetLastError() == WSAEWOULDBLOCK GG
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
		if (WSAGetLastError() == WSAEWOULDBLOCK)
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
void SocketManager::ServerAddSendMsgList(SOCKET Client, Json::Value msg)
{
	ServerSendMsgList[Client].push(msg);
}
bool  SocketManager::ServerSendMsg(SOCKET Client, Json::Value msg)
{
	//牢记:select用于ServerSocket时只能用于readfds判断是否有连接请求,不能用于判断服务端是否能数据读写
   //发送的消息出现问题 ****烫烫烫(Msg缓冲区超出Client缓冲区大小)

	if (msg.isObject() && msg["type"] == 2)//Json有多种数据类型,只有对象才能使用[""]访问成员!!!否则程序会直接退出
	{
		//std::cout << msg << "\n";
	}

	str_tmp = msg.toStyledString();
	str_tmp += "#";//结束符号
	if (str_tmp.length() > RecvBufferSize)
	{
		std::cout << "str_msg.length() > RecvBufferSize\n";
		exit(2341);
	}
	int size = send(Client, str_tmp.c_str(), str_tmp.length(), 0);
	//static int cnt = 0;
	//std::cout << "Server is sending " << ++cnt << "th msg\n"<<str_tmp<<"\n";
	if (size <= 0)
	{
		std::cout << "ServerSendMsg error " << WSAGetLastError() << "\n";
		error = 1;
		exit(9929);
		return 0;
	}
	else
	{

	}
	return 1;
}
void SocketManager::ServerProcessSendMsgList()
{
	//初始化为空集
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);

	for (auto [Client, Msglist] : ServerSendMsgList)
	{
		//添加一个新的文件描述符
		FD_SET(Client, &readfds);
		FD_SET(Client, &writefds);
		FD_SET(Client, &exceptfds);
	}
	select(0, &readfds, &writefds, nullptr, &timeout);
	for (auto& [Client, MsgList] : ServerSendMsgList)
	{
		if (FD_ISSET(Client, &writefds) && !MsgList.empty())//注意是确认客户端的可写性
		{
			auto msg = MsgList.front();
			ServerSendMsg(Client, msg);
			MsgList.pop();
		}
		else
		{
			//std::cout << Client << "不可写\n";
		}
	}
}
Json::Value SocketManager::ServerRecvMsg(SOCKET Client)
{

	//初始化为空集
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);

	//添加一个新的文件描述符
	FD_SET(Client, &readfds);
	FD_SET(Client, &writefds);
	FD_SET(Client, &exceptfds);

	select(0, &readfds, &writefds, nullptr, &timeout);

	if (!FD_ISSET(Client, &readfds))//!!! 对Client检查可读性，返回1表示该套接字上有数据可读
	{
		return Json::nullValue;
	}
	memset(RecvMsgBuffer, '\0', sizeof(RecvMsgBuffer));
	int size = recv(Client, RecvMsgBuffer, sizeof(RecvMsgBuffer), 0);
	//size==-1可能是非阻塞模式下没有收到数据 
	if (size == 0)
	{
		std::cout << "Client closed\n";
		return Json::nullValue;
	}
	else if (size > 0)
	{
		//std::cout  << "Server recv \n" << RecvMsgBuffer << "\n"; 
	}

	Json::Value val = Json::nullValue;
	str_tmp = std::string(RecvMsgBuffer);
	if (str_tmp.find('#') == std::string::npos)
	{
		server_str_msg += RecvMsgBuffer;
		return val;
	}
	int len = str_tmp.find('#');
	server_str_msg += str_tmp.substr(0, len);
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(server_str_msg, val);
	if (len + 1 >= str_tmp.size())
	{
		server_str_msg = "";
	}
	else
	{
		server_str_msg = str_tmp.substr(len + 1);
	}
	if (parsingSuccessful)
	{
		static int cnt = 0;
		std::cout <<"server recv "<< ++cnt << "th msg\n";
		std::cout << val<<"\n";
	}
	else
	{
		std::cout << "server parse fail\n";
		exit(934);
	}
	return val;
}
void SocketManager::ServerRecvClientsMsg()
{
	for (auto it = ClientSockets.begin(); it != ClientSockets.end();)
	{
		//先收到客户端的消息再发送指令
		Json::Value msg = ServerRecvMsg(*it);
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

	/* 客户端好像没必要设置非阻塞
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

Json::Value SocketManager::ClientRecvMsg()
{
	//std::cout << " ClientRecvMsg begin and recv function begin\n";
	//初始化为空集
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);

	//添加一个新的文件描述符
	FD_SET(ClientSocket, &readfds);
	FD_SET(ClientSocket, &writefds);
	FD_SET(ClientSocket, &exceptfds);

	select(0, &readfds, &writefds, nullptr, &timeout);

	if (!FD_ISSET(ClientSocket, &readfds))
	{
		return Json::nullValue;
	}
	//std::cout << "可读\n";
	memset(RecvMsgBuffer, '\0', sizeof(RecvMsgBuffer));
	//等到find #就把str_msg的信息转成Json返回并清空str_msg
	Json::Value val = Json::nullValue;
	int state = recv(ClientSocket, RecvMsgBuffer, sizeof(RecvMsgBuffer), 0);
	str_tmp = std::string(RecvMsgBuffer);

	if (str_tmp.find('#') == std::string::npos)
	{
		client_str_msg += RecvMsgBuffer;
		return val;
	}
	int len = str_tmp.find('#');
	client_str_msg += str_tmp.substr(0, len);

	if (state == 0)
	{
		std::cout << "服务端已关闭连接\n";
		return val;
	}
	else if (state != SOCKET_ERROR)
	{
		Json::Reader reader;
		bool parsingSuccessful = reader.parse(client_str_msg, val);
		if (len + 1 >= str_tmp.size())
		{
			client_str_msg = "";
		}
		else
		{
			client_str_msg = str_tmp.substr(len + 1);
		}
		if (parsingSuccessful)
		{
			 static int cnt = 0;
			 if (cnt++ == 0)
			 {
				 std::cout << val << "\n";
			 }
			 /*std::cout << cnt++ << " msg\n";
			std::cout <<  val <<"  \n";*/
		}
		else
		{
			std::cout << "parse fail\n";
			exit(113);
		}

	}
	else
	{
		exit(2344);
	}

	return val;
}
void SocketManager::ClientSendMsg(Json::Value msg)
{
	str_tmp = msg.toStyledString();
	str_tmp += '#';
	int size = send(ClientSocket, str_tmp.c_str(), RecvBufferSize, 0);
	static int cnt = 0;
	std::cout << "client send " << ++cnt << "th msg \n"<<str_tmp<<"\n";
	if (size == SOCKET_ERROR)
	{
		std::cout << "发送信息失败" << WSAGetLastError() << "\n";
	}
	else
	{
		
	}
}
SOCKET SocketManager::GetClientSocket()
{
	return ClientSocket;
}
