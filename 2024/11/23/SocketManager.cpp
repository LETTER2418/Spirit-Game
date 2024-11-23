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
void SocketManager::Test()
{
 
}
bool SocketManager::StartServer()
{

	strncpy_s(SendMsgBuffer, "Hi from Server", sizeof(SendMsgBuffer));

	//创建套接字
	if ((ServerSocket = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
	{
		std::cout << "Server创建套接字失败！错误代码：" << WSAGetLastError() << "\n";
		return false;
	}

	//设置为非阻塞方式
	u_long ul = 1;
	if (ioctlsocket(ServerSocket, FIONBIO, &ul) == SOCKET_ERROR)
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
void SocketManager::ServerAcceptClient()
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
			return;
		}
	}
	else
	{
		std::cout << "服务器成功和第:" << (int)ClientSockets.size() + 1 << "个客户端建立连接！" << "\n";
		ClientSockets.push_back(sc);//将建立连接的套接字压入list
		//system("pause");
	}
}
bool  SocketManager::ServerSendMsg(std::vector<SOCKET>::iterator it)
{
	//发送的消息出现问题 ****烫烫烫(Msg缓冲区超出Client缓冲区大小)
	SOCKET Client = *it;
	strncpy_s(SendMsgBuffer, "Hi from Server", sizeof(SendMsgBuffer) - 1);
	int size = send(Client, SendMsgBuffer, sizeof(SendMsgBuffer), 0);
	std::cout << "服务端向" << Client << "发送数据" << SendMsgBuffer<<"\n";
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
		return false;
	}
	return true;
}
bool SocketManager::ServerRecMsg(std::vector<SOCKET>::iterator it)
{
	SOCKET Client = *it;
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
		return false;
	}
	else
	{
		std::cout << Client << "说：" << RecMsgBuffer << "\n";
	}
	return true;
}
void SocketManager::ServerRecClientsMsg()
{
	for (auto it = ClientSockets.begin(); it != ClientSockets.end();)
	{
		//先收到客户端的消息再发送指令
		bool f1 = ServerRecMsg(it);
		bool f2 = ServerSendMsg(it);
		if (f1&&f2)
		{
			it++;
		}
		else
		{
			it = ClientSockets.erase(it);
		}
	}
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
	 unsigned long ul = 1;
	 if (ioctlsocket(ClientSocket, FIONBIO, &ul) != 0) {
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

void SocketManager::ClientRecMsg()
{
	int state = recv(ClientSocket, RecMsgBuffer, sizeof(RecMsgBuffer), 0);
	if (state == 0)
	{
		std::cout << "服务端已关闭连接\n";
		 
	}
	else if (state != SOCKET_ERROR)
	{
		std::cout <<"Clien" << ClientSocket << " received "<<RecMsgBuffer << "\n";
	}
}
void SocketManager::ClientSendMsg()
{
	strncpy_s(SendMsgBuffer, "Hi from Cient", sizeof(SendMsgBuffer) - 1);
	int state = send(ClientSocket, SendMsgBuffer, sizeof(SendMsgBuffer), 0);
	if (state == SOCKET_ERROR)
	{
		std::cout << "发送信息失败" << WSAGetLastError() << "\n";
	}
	else
	{
		std::cout << "Client"<<ClientSocket<<" sent " << SendMsgBuffer << "\n";
	}
}
