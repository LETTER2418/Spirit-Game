#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include<assert.h>
#include<iostream>
#include<vector>
#include<string>
#include "json/json.h" 
#pragma comment (lib, "ws2_32.lib")

//作用:服务端和多个客户端实时通信JSON数据 

class SocketManager
{
public:
	SocketManager();
	~SocketManager();
	bool StartServer();
	bool ServerAcceptClient();
	bool ServerSendMsg(SOCKET, Json::Value);
	Json::Value ServerRecMsg(SOCKET);
	void ServerRecClientsMsg();

	void StartClient();//如果StartServer失败就StartCient 
	void ClientSendMsg(Json::Value);//发送客户端的行为并返回服务端的反馈
	std::vector<SOCKET> GetClientSockets();
	SOCKET GetClientSocket();
	Json::Value ClientRecMsg();//接受消息

	void ServerTest();
	void ClientTest();
	void Test();
	Json::Value CreateMsg();

private:
	WSADATA wsaData;
	SOCKET ServerSocket, ClientSocket;
	static const int  SERVER_PORT = 6666;
	static const int  RecBufferSize = 5E5;//需要合适,否则溢出会"烫烫烫”
	static const int  SendBufferSize = 5E5;
	bool error = 0;
	char RecMsgBuffer[RecBufferSize];
	char SendMsgBuffer[SendBufferSize];
	std::vector<SOCKET>ClientSockets;//存储连接的套接字 
	fd_set readfds, writefds, exceptfds;
	timeval timeout;
};
