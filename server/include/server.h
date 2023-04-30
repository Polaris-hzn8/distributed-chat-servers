/*************************************************************************
	> File Name: server.h
	> Author: luochenhao
	> Mail: 3453851623@qq.com
	> Created Time: Fri 28 Apr 2023 12:11:38 AM CST
 ************************************************************************/

#ifndef _SERVER_H
#define _SERVER_H

#include "head.h"

class ChatServer {
public:
    ChatServer(EventLoop *loop, const InetAddress &listenAddr, const string &nameArg);//构造函数
    void start();//开启事件循环

private:
    void onConnection(const TcpConnectionPtr &conn);//上报连接相关信息的回调函数
    void onMessage(const TcpConnectionPtr &conn, Buffer *buff, Timestamp time);//上报读写事件相关信息的回调函数

    TcpServer _server; //组合muduo
    EventLoop *_loop; //epoll
};


#endif
