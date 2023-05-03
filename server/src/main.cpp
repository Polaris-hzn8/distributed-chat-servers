/*************************************************************************
    > File Name: main.cpp
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Sun 30 Apr 2023 15:08:58 CST
************************************************************************/

#include "head.h"
#include "chatserver.h"

int main() {
    EventLoop loop;//epoll
    InetAddress add("192.168.172.133", 20000);
    ChatServer server(&loop, add, "myChatServer");//创建Server对象

    server.start();//epoll_ctl 将listenfd 添加到 epoll树上
    loop.loop();//epoll_wait 以阻塞的方式 等待新用户连接 or 已连接用户的读写事件
    return 0;
}
