# WebServerDemo
一个win下的web服务器 

为了熟悉流程, https://www.bilibili.com/video/BV16t411N7aQ?p=7&t=1720
按照本地址中,web服务器视频实现
### 要实现的内容:
1. 客户端: 发送/接受数据
2. 服务端: 发送/接收数据
3. 抽象一个socket类 方便使用 : 初始化, 发送/接受数据
4. SocketClient: 特定初始化
5. SocketServer: 特定初始化, 接收连接



### 存在的BUG:
1. 接受baidu服务器的信息, 无法识别. 
  :提示无法解析的外部符号
   课程中使用 ```#pragma	comment(lib, "ws2_32.lib")```, 在vs2019中已失效
   
2. 在本机充当服务器的时候, 发送静态网页源码 貌似没有渲染成功,只输出字符串 ,视频中既如此

### 用到的socketAPI:

老套路:
服务器: socket-> bind ->listen ->accpet-> sned/recv -> close
客户端: socket-> connect- > send/recv ->close

### 代码详解:
#### 封装socket操作
```cpp
//头文件 定义父类 一些通用socket操作
class MySocket{
public:
	enum TypeSocekt {//IO连接类型
		BlockingSocket,
		UnblockingSocekt
	};
	MySocket();
	MySocket(SOCKET new_socket);
	virtual ~MySocket();
	std::string ReceiveLine();//收发数据
	bool SendLine(std::string buffer);
	void Closed();

	bool operator==(unsigned long code){ 
		return socket_ == code;//重载运算符
	}
protected:
	SOCKET socket_;
	static bool init_winsocket;
};
```

客户端/服务器类 继承父类, 并创建自己的独特行为

```cpp
class SocketClient :public MySocket {
public:
	//连接服务器
    //host: 域名 port:端口
  	SocketClient(const std::string& host, int port);
};

class SocketServer :public MySocket {
public:
	SocketServer(int port, //端口
		int connection,  //接受的连接数
		TypeSocekt type = BlockingSocket);//接收方法 默认阻塞
	MySocket* Accept();

};
```

#### 具体实现

##### Mysocket类

```cpp
bool MySocket::init_winsocket = false;//保证只初始化一次
MySocket::MySocket()
{
	if (!init_winsocket) {//只会初始化一次//初始化winsocket
		WSADATA info;
		if (WSAStartup(MAKEWORD(2, 2), &info))
			throw "could not start WSA";
        	//winsocekt 初始化
		init_winsocket = true;
	}
     //创建socket描述符
	socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == socket_) throw "Invalid socket";
}

MySocket::MySocket(SOCKET new_socket){
	socket_ = new_socket;//一个简单的复制构造函数
}

MySocket::~MySocket(){
}

std::string MySocket::ReceiveLine(){
	std::string ret;
	while (true) {
		char buf;
        //面试不问具体API 理解即可
        //描述字 + 缓冲区
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

bool MySocket::SendLine(std::string buffer){	
	buffer += '\n';//发送buffer内容
								//文件     缓冲区指针        缓冲区长度       flag?? 
	return  SOCKET_ERROR != send(socket_, buffer.c_str(), buffer.size(), 0);
}

void MySocket::Closed(){
	closesocket(socket_); //手动关闭socekt
}
```

##### 客户端类构造函数 : 

```cpp
SocketClient::SocketClient(const std::string& host, int port)
{	//子类构造函数 先调用父类 创建一个socket
	bool isok = false;
	do {
		hostent* he = gethostbyname(host.c_str());//
		if (!he) break;//失败的时候 退出
		sockaddr_in addr = { 0 };
		addr.sin_family = AF_INET;//协议簇IPV4
		addr.sin_port = htons(port);//web 端口一般是80
		addr.sin_addr = *((in_addr*)he->h_addr);//地址
        //这里存的是无符号32位整数 ip地址->int (之前的面试题..)

		if (SOCKET_ERROR == connect(socket_, (sockaddr*)&addr, sizeof(sockaddr))) {
			//connect socketAPI socket描述符 socket地址 地址长度
			break;
		}

	} while (false);

	if (!isok) {//如果连接失败就抛出错误
		char error_msg[MAXBYTE] = { 0 };
		strerror_s(error_msg, WSAGetLastError());
		throw error_msg;
	}

}
```

##### 服务器:

```cpp

SocketServer::SocketServer(int port, int connection, TypeSocekt type)
{
	sockaddr_in addr = { 0 };
	addr.sin_family = PF_INET;
	addr.sin_port = htons(port);
	if (type == UnblockingSocekt) { 
		unsigned long arg = 1;
		ioctlsocket(socket_, FIONBIO, &arg); 
	}//绑定 socket描述符 + 地址
	if (SOCKET_ERROR == bind(socket_, (sockaddr*)&addr, sizeof(sockaddr))) {
		//构造函数 抛出异常后,析构函数不会被调用 所以要手动关闭父类生成的文件标识符
		Closed();
		throw "bind error";
	}
	listen(socket_, connection);//文件描述符 最大连接数
}

MySocket* SocketServer::Accept(){	
	MySocket* accept_socket = new MySocket
		(accept(socket_, nullptr, nullptr));//listen之后一直接收连接
	if (*accept_socket == INVALID_SOCKET) {
		throw "Invalid socket";
	}
	return accept_socket;
}
```

#### main函数:

```cpp
// ConsoleApplication2.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
/*
1. 连接服务器
2. 发送数据win32:
3. 接受数据:
4. 显示数据:
*/
#include <iostream>
#include <fstream>//文件操作
#include "MySocket.h"
#include <process.h>//线程

#pragma	comment(lib, "ws2_32.lib")//
//访问百度 发过来的网页代码 无法显示 ,未知的外部符号

unsigned int __stdcall Connection(void* p) {//服务端回调函数
	MySocket* accept_socket = (MySocket*)p;
	/* std::ifstream file("1.txt");//发送网页源码
	*  string buf;
	*  while(getline(file, buf)) accept_socket->SendLine(buf);
	*/
	accept_socket->SendLine("html 格式 通过127.0.0.1可以访问");
	while (true) {//接收客户端返回的数据
		//发送数据 给浏览器
		//浏览器也有数据返回给服务器
		std::string buf = accept_socket->ReceiveLine();
		if (buf.empty()) {
			break;
		}
		std::cout << buf ;
	}
	delete accept_socket;
	return 0;
}

int main(){
	/*向服务器发GET请求*/
	//try
	//{
    // 实例化一个客户端类对象 首先会调用父类构造函数 创建一个socket文件描述符
    // 调用客户端类构造函数 根据网址和端口,进行connect连接
	//	socketclient web_read("www.baidu.com", 80); 
    //调用父类包装过的socketAPI send发送数据
	//	web_read.sendline("get / http/1.0");//请求行		
	//	web_read.sendline("host: www.baidu.com");//首部行
	//	web_read.sendline("");//结束
	//	while (true) {
	//		std::string buf = web_read.receiveline();//循环接收并打印 按行接受
	//		if (buf.empty()) break;
	//		std::cout << buf << std::endl;

	//	}
	//}
	//catch (const std::exception&)
	//{

	//}
  
  //充当服务器
	try
	{
		SocketServer web_server(80, 10);
		while (true) {
            //可以不停的接收
            //还是多线程
			MySocket* accept_socket = web_server.Accept();
			CloseHandle((HANDLE) _beginthreadex(nullptr, 0, Connection,
				accept_socket, 0, nullptr));//关闭线程句柄 不关闭 会导致句柄泄露	
		}
	}
	catch (const char *e)
	{
		std::cout << e << std::endl;
	}
	catch (...) {
		std::cout << "unknown error" << std::endl;
	}

	return 0;

}
```
