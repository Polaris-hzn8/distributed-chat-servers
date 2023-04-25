/*************************************************************************
    > File Name: muduo_server.cpp
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Sat 22 Apr 2023 21:55:05 CST
************************************************************************/

#include <iostream>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
using namespace std;
using namespace muduo;
using namespace muduo::net;

/*
基于muduo网络库开发服务器程序 基本步骤
1、组合TcpServer对象
2、创建EventLoop事件循环对象的指针（Epoll事件循环）
3、明确TcpServer构造函数需要什么参数、输出ChatServer的构造函数

*/
class CharServer {
public:
        
private:
    TcpServer _server;
    EventLoop *_loop;
};


int main() {
    
    return 0;
}

