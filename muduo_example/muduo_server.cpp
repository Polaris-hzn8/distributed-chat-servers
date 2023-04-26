/*************************************************************************
    > File Name: muduo_server.cpp
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Sat 22 Apr 2023 21:55:05 CST
************************************************************************/

#include <iostream>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <functional>
#include <string>
using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

/*
基于muduo网络库开发服务器程序 基本步骤
1、组合 TcpServer 对象
2、创建 EventLoop 事件循环对象的指针（Epoll事件循环）
3、明确TcpServer构造函数 需要什么参数、从而编写 ChatServer 的构造函数
4、在当前服务器类的构造函数中 注册处理连接的回调函数 和 处理读写事件的回调函数
5、设置合适的服务器端线程的数量，muduo库会自己划分IO线程 与 worker线程
*/

class ChatServer {
public:
    /**
     * @brief Construct a new Chat Server object
     * 
     * @param loop 事件循环 epoll
     * @param listenAddr IP + Port
     * @param nameArg 服务器的名字
     */
    ChatServer(EventLoop *loop,
               const InetAddress &listenAddr, 
               const string &nameArg)
        :server_(loop, listenAddr, nameArg), loop_(loop) {
        
        //1.给服务器 注册用户 连接的创建与断开 进行回调
        /* 将成员函数myConnection地址 与 this指针指向的对象绑定 转化为一个仿函数并存储到对象 f 中  */
        /* function<void(const TcpConnectionPtr&)> f =  = bind(&ChatServer::myConnection, this, placeholders::_1); */
        server_.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));
        
        //2.给服务器 注册用户 读写事件 进行回调
        server_.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

        //3.设置服务器端的线程数量 1个IO线程 3个worker线程
        server_.setThreadNum(4);
    }

    /**
     * @brief 开启事件循环
     * 
     */
    void start() {
        server_.start();
    }

private:
    /**
     * @brief 处理用户连接的 创建与断开
     * 关于 epoll + listenfd + accept 底层的muduo库全部都封装了, 只暴露了一个回调函数的接口 myConnection()
     * 当有新用户创建连接 or 已经建立连接的用户选择断开连接时 这个方法就会响应 
     * @param conn 网络连接
     */
    void onConnection(const TcpConnectionPtr &conn) {
        if (conn->connected()) {
            cout << conn->peerAddress().toIpPort() 
                 << "->" 
                 << conn->localAddress().toIpPort() 
                 << " state:online" << endl;
        } else {
            cout << conn->peerAddress().toIpPort() 
                 << "->" 
                 << conn->localAddress().toIpPort() 
                 << " state:offline" << endl;
            conn->shutdown();//close(fd);
            loop_->quit();//退出epoll服务器结束
        }
    }

    /**
     * @brief 处理用户的 读写事件
     * 
     * @param conn 网络连接
     * @param buff 缓冲区
     * @param time 接受到数据的时间
     */
    void onMessage(const TcpConnectionPtr &conn, Buffer *buff, Timestamp time) {
        string content = buff->retrieveAllAsString();
        cout << "recv data: " << content << "time: " << time.toString() << endl;
        conn->send(content);
    }

    TcpServer server_; 
    EventLoop *loop_; // epoll
};


int main() {
    EventLoop loop;//epoll
    InetAddress add("192.168.172.133", 20000);
    ChatServer server(&loop, add, "myChatServer");//创建Server对象

    server.start();//epoll_ctl 将listenfd 添加到 epoll树上
    loop.loop();//epoll_wait 以阻塞的方式 等待新用户连接 or 已连接用户的读写事件
    
    return 0;
}

