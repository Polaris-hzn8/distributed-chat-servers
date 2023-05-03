/*************************************************************************
    > File Name: chatservice.cpp
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Sun 30 Apr 2023 17:26:49 CST
************************************************************************/

/**
 * chatservice 业务层
*/

#include "chatservice.h"
#include "common.h"

//获取单例对象的接口函数
ChatService* ChatService::instance() {
    static ChatService service;//单例对象 线程安全
    return &service;
}

//注册消息id 以及对应的Handler回调操作
ChatService::ChatService() {
    //unordered_map<int, MsgHandler> _msgHandlerMap;//存储消息id 及其对应的事件处理方法表
    //相应的消息id 及其 与对应的事件回调处理函数 做一个绑定
    _msgHandlerMap.insert({LOGIN_MSG, bind(&ChatService::login, this, _1, _2, _3) });
    _msgHandlerMap.insert({REG_MSG, bind(&ChatService::regis, this, _1, _2, _3) });

}

//获取消息id对应的事件处理器
MsgHandler ChatService::getHandler(int msg_id) { 
    auto it = _msgHandlerMap.find(msg_id);
    //没有查询到结果 进行日志打印 或者直接返回一个空操作的事件处理器
    if (it == _msgHandlerMap.end()) {
        //LOG_ERROR << "msg_id" << msg_id << " can not find handler!";
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time) {
            LOG_ERROR << "msg_id" << msg_id << " can not find handler!";
        };
    } else {
        return _msgHandlerMap[msg_id];
    }
}

//处理登录业务
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    LOG_INFO << "do login service!";
}

//处理注册业务
void ChatService::regis(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    LOG_INFO << "do regis service!";
}












