/*************************************************************************
    > File Name: chatserver.cpp
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Sun 30 Apr 2023 14:45:20 CST
************************************************************************/

#include "head.h"
#include "chatserver.h"

/**
 * @brief Construct a new Chat Server:: Chat Server object
 * 
 * @param loop 事件循环 epoll
 * @param listenAddr IP + Port
 * @param nameArg 服务器的名字
 */
ChatServer::ChatServer(EventLoop *loop, const InetAddress &listenAddr, const string &nameArg)
    :_server(loop, listenAddr, nameArg), _loop(loop) {
    //1.给服务器注册 用户连接的创建与断开 进行回调（注册连接回调）
    /* 将成员函数myConnection地址 与 this指针指向的对象绑定 转化为一个仿函数并存储到对象 f 中  */
    /* function<void(const TcpConnectionPtr&)> f =  = bind(&ChatServer::myConnection, this, placeholders::_1); */
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));
    
    //2.给服务器注册 用户读写事件 进行回调（注册消息回调）
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

    //3.设置服务器端的线程数量 1个IO线程 3个worker线程
    _server.setThreadNum(4);
}

/**
 * @brief Construct a new Chat Server::start object
 * 开启事件循环
 */
void ChatServer::start() {
    _server.start();
}

/**
 * @brief 处理用户连接创建与断开
 * 关于 epoll + listenfd + accept 底层的muduo库全部都封装了, 只暴露了一个回调函数的接口 myConnection()
 * 当有新用户创建连接 or 已经建立连接的用户选择断开连接时 这个方法就会响应 
 * @param conn 网络连接
 */
void ChatServer::onConnection(const TcpConnectionPtr &conn) {
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
        _loop->quit();//退出epoll服务器结束
    }
}


/**
 * @brief 处理用户读写事件
 * 
 * @param conn 网络连接
 * @param buff 缓冲区
 * @param time 接受到数据的时间
 */
void ChatServer::onMessage(const TcpConnectionPtr &conn, Buffer *buff, Timestamp time) {
    string content = buff->retrieveAllAsString();
    //1.数据的反序列化string类型转为json类型
    json js = json::parse(content);

    //2.解耦网络模块与业务模块的代码 OOP解耦模块之间的关系：使用基于面向接口的编程（抽象基类）、基于回调操作
    //通过js["msg_id"]来 -> 获取业务handler -> conn json time




    cout << "recv data: " << content << "time: " << time.toString() << endl;
    conn->send(content);
}

