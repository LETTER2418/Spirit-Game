#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include<assert.h>
#include<iostream>
#include<vector>
#include<string>
#include<map>
#include<queue>
#include<thread>
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
	Json::Value ServerRecvMsg(SOCKET);
	void ServerRecvClientsMsg();
	void ServerAddSendMsgList(SOCKET Client,Json::Value);
	void ServerProcessSendMsgList();

	void StartClient();//如果StartServer失败就StartCient 
	void ClientSendMsg(Json::Value);//发送客户端的行为并返回服务端的反馈
	void ClientAddSendMsgList(Json::Value);
	void ClientProcessSendMsgList();
	std::vector<SOCKET> GetClientSockets();
	SOCKET GetClientSocket();
	Json::Value ClientRecvMsg();//接受消息

	void ServerTest();
	void ClientTest();
	void Test();
	Json::Value CreateMsg();

private:
	WSADATA wsaData;
	SOCKET ServerSocket= INVALID_SOCKET, ClientSocket=INVALID_SOCKET;
	static const int  SERVER_PORT = 1234;
	static const int  RecvBufferSize = 2E5 ;//溢出会"烫烫烫,  
	static const int  SendBufferSize = 2E5 ;
	bool error = 0;
	char RecvMsgBuffer[RecvBufferSize];
	char SendMsgBuffer[SendBufferSize];
	char tmp[SendBufferSize];
	std::vector<SOCKET>ClientSockets;//存储连接的套接字 
	fd_set readfds, writefds, exceptfds;
	timeval timeout;
	std::map<SOCKET, std::queue<Json::Value>>ServerSendMsgList;//Server send to client
	std::map<SOCKET, std::queue<Json::Value>>ServerRecvMsgList;
	std::queue<Json::Value>ClientRecvMsgList;
	std::queue<Json::Value>ClientSendMsgList;
	std::string server_str_msg;
	std::string client_str_msg;
	std::string str_tmp;
};
