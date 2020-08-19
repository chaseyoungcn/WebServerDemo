#include "MySocket.h"
//#include "StdAfx.h"

//#pragma	comment(lib, "ws2_32.lib")
SocketClient::SocketClient(const std::string& host, int port)
{	//子类构造函数 先调用父类 创建一个socket
	bool isok = false;
	do {
		hostent* he = gethostbyname(host.c_str());
		if (!he) break;//失败的时候 推出
		sockaddr_in addr = { 0 };
		addr.sin_family = AF_INET;//协议簇
		addr.sin_port = htons(port);//web 端口一般是80
		addr.sin_addr = *((in_addr*)he->h_addr);//地址

		if (SOCKET_ERROR == connect(socket_, (sockaddr*)&addr, sizeof(sockaddr))) {
			//std::cout << "connetion error" << endl;
			break;
		}

	} while (false);

	if (!isok) {
		char error_msg[MAXBYTE] = { 0 };
		strerror_s(error_msg, WSAGetLastError());
		throw error_msg;
	}

}

bool MySocket::init_winsocket = false;

//bool MySocket::init_winsocket;

MySocket::MySocket()
{
	if (!init_winsocket) {//只会初始化一次//初始化winsocket
		WSADATA info;
		if (WSAStartup(MAKEWORD(2, 2), &info))
			throw "could not start WSA";//winsocekt 初始化
		init_winsocket = true;
	}
	socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == socket_) throw "Invalid socket";
}

MySocket::MySocket(SOCKET new_socket)
{
	socket_ = new_socket;
}

MySocket::~MySocket()
{

}

std::string MySocket::ReceiveLine()
{
	std::string ret;
	while (true) {
		char buf;
		switch(recv(socket_, &buf, 1, 0)) {
			case 0:
				break;//接受完成
			case -1:
				return "";//接受失败
		}
		ret += buf;
		if (buf == '\n') break;
	}
	return ret;
}

bool MySocket::SendLine(std::string buffer)
{	
	buffer += '\n';
								//文件     缓冲区指针        缓冲区长度       flag?? 
	return  SOCKET_ERROR != send(socket_, buffer.c_str(), buffer.size(), 0);
}

void MySocket::Closed()
{
	closesocket(socket_);
}

SocketServer::SocketServer(int port, int connection, TypeSocekt type)
{
	sockaddr_in addr = { 0 };
	addr.sin_family = PF_INET;
	addr.sin_port = htons(port);
	if (type == UnblockingSocekt) { 
		unsigned long arg = 1;
		ioctlsocket(socket_, FIONBIO, &arg); 
	}
	if (SOCKET_ERROR == bind(socket_, (sockaddr*)&addr, sizeof(sockaddr))) {
		//构造函数 抛出异常后,析构函数不会被调用 所以要手动关闭父类生成的文件标识符
		Closed();
		throw "bind error";
	}
	listen(socket_, connection);

}

MySocket* SocketServer::Accept()
{	
	MySocket* accept_socket = new MySocket
		(accept(socket_, nullptr, nullptr));
	if (*accept_socket == INVALID_SOCKET) {
		throw "Invalid socket";
	}
	return accept_socket;
}
