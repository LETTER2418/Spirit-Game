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
		ServerSendMsg(Client,msg);
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
	std::cout << "服务端向" << ServerSocket << "发送数据" << str_msg.c_str() << "\n";
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
		Error = 1;
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
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);//绑定本机的环回地址
	if (SOCKET_ERROR == bind(ServerSocket, (SOCKADDR*)&addr, sizeof(sockaddr_in)))
	{
		std::cout << "地址绑定失败！错误代码：" << WSAGetLastError() << "\n";
		closesocket(ServerSocket);//modify
		return false;
	}

	//将套接字设为监听状态
	int size = listen(ServerSocket, 0);
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
	sockaddr_in  client_addr;
	int addr_len = sizeof(client_addr);
	auto sc = accept(ServerSocket, (struct sockaddr*)&client_addr, &addr_len);
	if (sc == INVALID_SOCKET)
	{
		if (GetLastError() == WSAEWOULDBLOCK)
		{
			//std::cout << "本次 accept函数没有客户端建立连接！" << "\n";
			Sleep(200);//设置令其间隔一段时间
		}
		else
		{
			std::cout << "accept函数调用失败！网络异常程序退出" << "\n";
			closesocket(ServerSocket);
		}
		return false;
	}
	else
	{
		std::cout << "服务器成功和第:" << (int)ClientSockets.size() + 1 << "个客户端建立连接！" << "\n";
		ClientSockets.push_back(sc);//将建立连接的套接字压入list
		return true;
	}
}
void  SocketManager::ServerSendMsg(SOCKET Client, Json::Value msg)
{
	//发送的消息出现问题 ****烫烫烫(Msg缓冲区超出Client缓冲区大小)
	std::string str_msg = msg.toStyledString();
	int size = send(Client, str_msg.c_str(), str_msg.length(), 0);
	std::cout << "服务端向" << Client << "发送数据" << str_msg.c_str() << "\n";
	if (size <= 0)
	{
		if (GetLastError() == WSAEWOULDBLOCK)
		{
			std::cout << "Send data failure！\n";
		}
		else
		{
			std::cout << Client << " is closed" << "\n";
		}
		Error = 1;
		return;
	}
	return;
}
Json::Value SocketManager::ServerRecMsg(SOCKET Client)
{
	int size = recv(Client, RecMsgBuffer, sizeof(RecMsgBuffer), 0);
	if (size <= 0)
	{
		if (GetLastError() == WSAEWOULDBLOCK)
		{
			std::cout << "recv data failure！\n";
		}
		else
		{
			std::cout << Client << " is closed" << "\n";
		}
		return Json::nullValue;
	}
	else
	{
		std::cout << Client << "说：" << RecMsgBuffer << "\n";
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
		if (Error == 0)
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
	return ClientSockets;
}
void SocketManager::StartClient()
{
	// 创建客户端套接字
	ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ClientSocket == INVALID_SOCKET) {
		std::cout << "Client创建套接字失败！错误代码：" << WSAGetLastError() << "\n";
		return;
	}

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
			return;
		}
	}

}

Json::Value SocketManager::ClientRecMsg()
{
	int state = recv(ClientSocket, RecMsgBuffer, sizeof(RecMsgBuffer), 0);
	if (state == 0)
	{
		std::cout << "服务端已关闭连接\n";
		return Json::nullValue;
	}
	else if (state != SOCKET_ERROR)
	{
		std::cout << "Clien" << ClientSocket << " received " << RecMsgBuffer << "\n";
		Json::Value val;
		Json::Reader reader;
		bool parsingSuccessful = reader.parse(RecMsgBuffer, val);
		if (parsingSuccessful)
		{
			return val;
		}
	}
	return Json::nullValue;
}
void SocketManager::ClientSendMsg(Json::Value msg)
{
	std::cout << "sending\n";
	std::string str_msg = msg.toStyledString();
	 
	int size = send(ClientSocket, str_msg.c_str(), str_msg.length(), 0);
	std::cout << "finist sending\n";
	if (size == SOCKET_ERROR)
	{
		std::cout << "发送信息失败" << WSAGetLastError() << "\n";
	}
	else
	{
		std::cout << "Client" << ClientSocket << " sent " << str_msg << "\n";
	}
}
