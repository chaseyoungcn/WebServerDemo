#include "MySocket.h"
//#include "StdAfx.h"

//#pragma	comment(lib, "ws2_32.lib")
SocketClient::SocketClient(const std::string& host, int port)
{	//���๹�캯�� �ȵ��ø��� ����һ��socket
	bool isok = false;
	do {
		hostent* he = gethostbyname(host.c_str());
		if (!he) break;//ʧ�ܵ�ʱ�� �Ƴ�
		sockaddr_in addr = { 0 };
		addr.sin_family = AF_INET;//Э���
		addr.sin_port = htons(port);//web �˿�һ����80
		addr.sin_addr = *((in_addr*)he->h_addr);//��ַ

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
	if (!init_winsocket) {//ֻ���ʼ��һ��//��ʼ��winsocket
		WSADATA info;
		if (WSAStartup(MAKEWORD(2, 2), &info))
			throw "could not start WSA";//winsocekt ��ʼ��
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
				break;//�������
			case -1:
				return "";//����ʧ��
		}
		ret += buf;
		if (buf == '\n') break;
	}
	return ret;
}

bool MySocket::SendLine(std::string buffer)
{	
	buffer += '\n';
								//�ļ�     ������ָ��        ����������       flag?? 
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
		//���캯�� �׳��쳣��,�����������ᱻ���� ����Ҫ�ֶ��رո������ɵ��ļ���ʶ��
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
