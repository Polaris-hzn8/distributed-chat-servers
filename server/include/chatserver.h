/*************************************************************************
	> File Name: chatserver.h
	> Author: luochenhao
	> Mail: 3453851623@qq.com
	> Created Time: Fri 28 Apr 2023 12:11:38 AM CST
 ************************************************************************/

/**
 * chatserver 网络层
*/

#ifndef _CHATSERVER_H
#define _CHATSERVER_H

#include "head.h"

class ChatServer {
public:
    ChatServer(EventLoop *loop, const InetAddress &listenAddr, const string &nameArg);//构造函数 为了构造成员变量TcpServer
    void start();//开启事件循环

private:
    void onConnection(const TcpConnectionPtr &conn);//上报连接相关信息的回调函数
    void onMessage(const TcpConnectionPtr &conn, Buffer *buff, Timestamp time);//上报读写事件相关信息的回调函数

    TcpServer _server; //TcpServer产生的对象作为组合对象 组合muduo库 实现服务器功能的类对象
    EventLoop *_loop; //指向一个事件循环的指针 保存事件循环 可以在合适的时候调用quit方法 退出事件循环 epoll
};

#endif
