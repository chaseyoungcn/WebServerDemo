#ifndef _MYSOCKET_H_
#define _MYSOCKET_H_

#include <string>
#include <WinSock2.h>

class MySocket{
public:
	enum TypeSocekt {
		BlockingSocket,
		UnblockingSocekt
	};
	MySocket();
	MySocket(SOCKET new_socket);
	virtual ~MySocket();
	std::string ReceiveLine();
	bool SendLine(std::string buffer);
	void Closed();

	bool operator==(unsigned long code){ 
		return socket_ == code;
	}
protected:
	SOCKET socket_;
	static bool init_winsocket;
};

class SocketClient :public MySocket {
public:
	//���ӷ�����
  	SocketClient(const std::string& host, int port);
};

class SocketServer :public MySocket {
public:
	SocketServer(int port, //�˿�
		int connection,  //���ܵ�������
		TypeSocekt type = BlockingSocket);//���շ���
	MySocket* Accept();

};




#endif // !_MYSOCKET_H_