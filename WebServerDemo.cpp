// ConsoleApplication2.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
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
//#include "StdAfx.h"

#pragma	comment(lib, "ws2_32.lib")//
//访问百度 发过来的网页代码 无法显示 ,未知的外部符号

//int _tmain(int argc, _TCHAR* argv[])
unsigned int __stdcall Connection(void* p) {//服务端回调函数
	MySocket* accept_socket = (MySocket*)p;
	/* std::ifstream file("1.txt");//发送网页源码
	*  string buf;
	*  while(getline(file, buf)) accept_socket->SendLine(buf);
	* 
	* 
	* 
	*/
	accept_socket->SendLine("html 格式 通过127.0.0.1可以访问");
	while (true) {
		//发送数据 给浏览器
		// 浏览器也有数据返回给服务器
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

	//try
	//{
	//	socketclient web_read("www.baidu.com", 80);
	//	web_read.sendline("get / http/1.0");//报文头		
	//	web_read.sendline("host: www.baidu.com");//请求头
	//	web_read.sendline("");//结束
	//	while (true) {
	//		std::string buf = web_read.receiveline();
	//		if (buf.empty()) break;
	//		std::cout << buf << std::endl;

	//	}
	//}
	//catch (const std::exception&)
	//{

	//}

	try
	{
		SocketServer web_server(80, 10);
		while (true) {
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


















// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
