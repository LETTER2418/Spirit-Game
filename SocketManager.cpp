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
	for (auto Client : ClientSockets)
	{
		if (Client != INVALID_SOCKET)
		{
			closesocket(Client);
		}
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

	//添加一个新的文件描述符
	FD_SET(ServerSocket, &readfds);

	//select函数的阻塞性依赖于是非阻塞还是阻塞套接字, 通常设置非阻塞
	//使用select函数检查客户端SOCKET的可读性,否则WSAGetLastError() == WSAEWOULDBLOCK GG
	//select函数可以实现I/O多路复用
	select(0, &readfds, nullptr, nullptr, &timeout);

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
		ClientsState[ClientSocket] = 1;
		ClientSockets.push_back(ClientSocket);
		return 1;
	}
}
void SocketManager::ServerAddSendMsgList(SOCKET Client, const Json::Value& msg)
{
	ServerSendMsgList[Client].push(msg);
}
bool  SocketManager::ServerSendMsg(SOCKET Client, const Json::Value &msg)
{
	//牢记:select用于ServerSocket时只能用于readfds判断是否有连接请求,不能用于判断服务端是否能数据读写
   //发送的消息出现问题 ****烫烫烫(Msg缓冲区超出Client缓冲区大小)

	if (!ClientsState[Client])
	{
		return 0;
	}
	str_tmp = msg.toStyledString();
	str_tmp += "#";//结束符号
	if (str_tmp.length() > RecvBufferSize)
	{
		std::cout << str_tmp.length() << "\n";
		std::cout << "str_tmp.length() > RecvBufferSize\n";
		exit(0);
	}

	/*std::thread t ([=]() {send(Client, str_tmp.c_str(), str_tmp.length(), 0); });
	t.detach();
	return 1;*/
	int size = send(Client, str_tmp.c_str(), str_tmp.length(), 0);
	//static int cnt = 0;
	//std::cout << "Server send " << ++cnt << "th msg\n"<<str_tmp<<"\n";
	if (size <= 0)
	{
		if (WSAGetLastError() == 10054)
		{
			ClientsState[Client] = 0;
		}
		else
		{
			std::cout << "ServerSendMsg error " << WSAGetLastError() << "\n";
			exit(0);
			return 0;
		}
	}
	return 1;
}
void SocketManager::ServerProcessClientSendMsgList(SOCKET Client)
{
	//std::lock_guard<std::mutex> lock(coutMutex);
	auto &MsgList = ServerSendMsgList[Client];
	if (!ClientsState[Client])
	{
		return;
	}
	while (1)
	{
		FD_ZERO(&writefds);
		FD_SET(Client, &writefds);
		select(0, nullptr, &writefds, nullptr, &timeout);
		//std::cout << "SendMsgList size==" << MsgList.size() << "\n";
		if (FD_ISSET(Client, &writefds))
		{
			//std::cout << "可写\n";

		}
		else
		{
			//std::cout << "不可写\n";
		}
		if (!FD_ISSET(Client, &writefds) || MsgList.empty())
		{
			break;
		}
		msg = MsgList.front();
		if (msg.isObject())
		{
			static int x = 0;
			if (!msg["cnt"].isNull() && x != msg["cnt"].asInt())
			{
				x = msg["cnt"].asInt();
				//std::cout << "Server send  " << msg["cnt"] << "th msg's response\n";
				//多客户端response过慢，需要优化，但是多线程没用(哭),埋个坑以后再填
			}
		}
		ServerSendMsg(Client, msg);
		MsgList.pop();
	}
}
void SocketManager::ServerProcessClientsSendMsgList()
{
	for (auto& [Client, MsgList] : ServerSendMsgList)
	{
		//std::thread([this, Client]() { ServerProcessClientSendMsgList(Client); }).detach();
		 ServerProcessClientSendMsgList(Client);
	}
}
Json::Value& SocketManager::ServerRecvMsg(SOCKET Client)
{
	json_value = Json::nullValue;
	FD_ZERO(&readfds);
	FD_SET(Client, &readfds);
	select(0, &readfds, nullptr, nullptr, &timeout);
	if (!FD_ISSET(Client, &readfds))//!!! 对Client检查可读性，返回1表示该套接字上有数据可读
	{
		return json_value;
	}
	memset(RecvMsgBuffer, '\0', sizeof(RecvMsgBuffer));
	int size = recv(Client, RecvMsgBuffer, sizeof(RecvMsgBuffer), 0);
	//size==-1可能是非阻塞模式下没有收到数据 
	if (size == 0)
	{
		std::cout << "Client closed\n";
		return json_value;
	}
	//std::cout  << "Server recv \n" << RecvMsgBuffer << "\n"; 
	str_tmp = std::string(RecvMsgBuffer);
	if (str_tmp.find('#') == std::string::npos)
	{
		server_str_msg += RecvMsgBuffer;
		return json_value;
	}
	int len = str_tmp.find('#');
	server_str_msg += str_tmp.substr(0, len);
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(server_str_msg, json_value);
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
		//static int cnt = 0;
		//std::cout <<"server recv "<< ++cnt << "th msg\n";
		//std::cout << val<<"\n";
	}
	else
	{
		std::cout << "server parse fail\n";
		exit(0);
	}
	return json_value;
}
void SocketManager::ServerRecvClientsMsg()
{
	for (auto it = ClientSockets.begin(); it != ClientSockets.end();)
	{
		//先收到客户端的消息再发送指令
		Json::Value msg = ServerRecvMsg(*it);
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
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);//绑定本机的环回地址

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

Json::Value& SocketManager::ClientRecvMsg()
{
	json_value = Json::nullValue;
	FD_ZERO(&readfds);
	FD_SET(ClientSocket, &readfds);
	select(0, &readfds, nullptr, nullptr, &timeout);
	if (!FD_ISSET(ClientSocket, &readfds))
	{
		return json_value;
	}

	memset(RecvMsgBuffer, '\0', sizeof(RecvMsgBuffer));

	//find #就把str_msg的信息转成Json返回并清空str_msg
	int state = recv(ClientSocket, RecvMsgBuffer, sizeof(RecvMsgBuffer), 0);
	
	str_tmp = std::string(RecvMsgBuffer);

	if (str_tmp.find('#') == std::string::npos)
	{
		client_str_msg += RecvMsgBuffer;
		return json_value;
	}
	int len = str_tmp.find('#');
	client_str_msg += str_tmp.substr(0, len);

	if (state == 0)
	{
		std::cout << "服务端已关闭连接\n";
		return json_value;
	}
	else if (state != SOCKET_ERROR)
	{
		Json::Reader reader;
		bool parsingSuccessful = reader.parse(client_str_msg, json_value);
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
			// static int cnt = 0;
			//std::cout <<"client recv"<< cnt++ << "th msg\n";
		}
		else
		{
			std::cout << "clientrecvmsg parse error\n " << client_str_msg << "\n";
			exit(0);
		}

	}
	else
	{
		std::cout << "ClientRecvMsg error "<<WSAGetLastError()<<"\n";
		exit(0);
	}
	return json_value;
}
void SocketManager::ClientSendMsg(const Json::Value& msg)
{
	str_tmp = msg.toStyledString();
	str_tmp += '#';
	if (str_tmp.length() > RecvBufferSize)
	{
		std::cout << "str_tmp.length() > RecvBufferSize\n";
		exit(0);
	}
	const char* buffer = str_tmp.c_str();
	if (buffer == nullptr) {
		std::cout << "发送信息失败：缓冲区指针无效\n";
		exit(0);
	}
	strcpy_s(tmp, str_tmp.c_str());
	/*std::thread([&]() {send(ClientSocket, tmp, RecvBufferSize, 0); }).detach();
	return;*/
	int size = send(ClientSocket, tmp, RecvBufferSize, 0);
	/*static int cnt = 0;
	std::cout << "client send " << ++cnt << "th msg \n"<<str_tmp<<"\n";*/
	if (size == SOCKET_ERROR)
	{
		std::cout << "ClientSendMsg error" << WSAGetLastError() << "\n";
		std::cout << msg << "\n";
		exit(0);
	}
	else
	{

	}
}
void SocketManager::ClientAddSendMsgList(const Json::Value& msg)
{
	ClientSendMsgList.push(msg);
}
void SocketManager::ClientProcessSendMsgList()
{
	while (1)
	{
		FD_ZERO(&writefds);
		FD_SET(ClientSocket, &writefds);
		select(ClientSocket, nullptr, &writefds, nullptr, &timeout);
		if (!FD_ISSET(ClientSocket, &writefds) || ClientSendMsgList.empty())
		{
			break;
		}
		auto msg = ClientSendMsgList.front();
		ClientSendMsg(msg);
		ClientSendMsgList.pop();
	}

}
SOCKET SocketManager::GetClientSocket()
{
	return ClientSocket;
}
std::map<SOCKET, int> SocketManager::GetClientsState()
{
	return ClientsState;
}
