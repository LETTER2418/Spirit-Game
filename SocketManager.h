#pragma once
#include<winsock.h>
#include<windows.h>
//#include <ws2tcpip.h>
#include<assert.h>
#include<iostream>
#include<vector>
#include<string>
#include<queue>
//#include<mutex>
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
	bool ServerSendMsg(SOCKET, const Json::Value&);// 引用传参避免栈溢出
	Json::Value& ServerRecvMsg(SOCKET);
	void ServerRecvClientsMsg();
	void ServerAddSendMsgList(SOCKET Client,const Json::Value&);
	void ServerProcessClientsSendMsgList();
	void ServerProcessClientSendMsgList(SOCKET );

	void StartClient();//如果StartServer失败就StartCient 
	void ClientSendMsg(const Json::Value&);//发送客户端的行为并返回服务端的反馈
	void ClientAddSendMsgList(const Json::Value&);
	void ClientProcessSendMsgList();
	std::vector<SOCKET> GetClientSockets();
	SOCKET GetClientSocket();
	Json::Value& ClientRecvMsg();//接受消息
	std::map<SOCKET, int>GetClientsState();

	void ServerTest();
	void ClientTest();
	void Test();
	Json::Value CreateMsg();

private:
	WSADATA wsaData;
	SOCKET ServerSocket= INVALID_SOCKET, ClientSocket=INVALID_SOCKET;
	static const int  SERVER_PORT = 1234;
	static const int  RecvBufferSize = 5E5 ;//溢出会"烫烫烫,  
	static const int  SendBufferSize = 5E5 ;
	//bool error = 0;
	char RecvMsgBuffer[RecvBufferSize];
	char SendMsgBuffer[SendBufferSize];
	char tmp[SendBufferSize];
	std::vector<SOCKET>ClientSockets;//存储连接的套接字 
	fd_set readfds, writefds, exceptfds;
	timeval timeout;
	std::map<SOCKET, std::queue<Json::Value>>ServerSendMsgList;
	std::map<SOCKET, std::queue<Json::Value>>ServerRecvMsgList;
	std::map<SOCKET, int>ClientsState;
	std::queue<Json::Value>ClientRecvMsgList;
	std::queue<Json::Value>ClientSendMsgList;
	std::string server_str_msg;
	std::string client_str_msg;
	std::string str_tmp;
	Json::Value msg;//防止栈溢出
	Json::Value json_value;
	//std::mutex coutMutex;
};
