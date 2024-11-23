#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include<assert.h>
#include<iostream>
#include<vector>
#include<string>
#include "json/json.h" 
#pragma comment (lib, "ws2_32.lib")

//目标:服务端实时接受多个客户端的JSON数据 

class SocketManager
{
public:
	SocketManager();
	~SocketManager();
	bool StartServer();
	void ServerAcceptClient();
	bool ServerSendMsg(std::vector<SOCKET>::iterator);
	bool ServerRecMsg(std::vector<SOCKET>::iterator);
	void ServerRecClientsMsg();
	//void ServerWork();

	void StartClient();//如果StartServer失败就StartCient (？可以自己给自己发消息吗?
	void ClientSendMsg();//发送客户端的行为并返回服务端的反馈
	void ClientRecMsg();//接受消息
	void Test();

private:
	WSADATA wsaData;
	SOCKET ServerSocket, ClientSocket;
	static const int  SERVER_PORT = 1235;
	static const int  RecBufferSize = 100;//需要修改
	static const int  SendBufferSize =100;//
	char RecMsgBuffer[RecBufferSize];
	char SendMsgBuffer[SendBufferSize];
	std::vector<SOCKET>ClientSockets;//存储连接的套接字 
};
