/*************************************************************************
    > File Name: chatservice.cpp
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Sun 30 Apr 2023 17:26:49 CST
************************************************************************/

/**
 * chatservice 业务层
*/

#include "common.h"
#include "json.hpp"
#include "chatservice.h"


//获取单例对象的接口函数
ChatService* ChatService::instance() {
    static ChatService service;//单例对象 线程安全
    return &service;
}

//将消息id 以及对应的Handler回调操作 初始化写入_msgHandlerMap中
ChatService::ChatService() {
    //unordered_map<int, MsgHandler> _msgHandlerMap;//存储消息id 及其对应的事件处理方法表
    //相应的消息id 及其 与对应的事件回调处理函数 做一个绑定
    _msgHandlerMap.insert({LOGIN_MSG, bind(&ChatService::login, this, _1, _2, _3) });//登录
    _msgHandlerMap.insert({REG_MSG, bind(&ChatService::regis, this, _1, _2, _3) });//注册
    _msgHandlerMap.insert({ONE_CHAT_MSG, bind(&ChatService::singleChat, this, _1, _2, _3) });//单聊
    _msgHandlerMap.insert({GROUP_CHAT_MSG, bind(&ChatService::groupChat, this, _1, _2, _3) });//群聊
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

/*
    ORM框架：Object Relation Map 对象关系映射
    业务层操作的都是对象, 在数据层 DAO 封装了所有数据的相应操作
*/

//处理注册业务 name passwd
void ChatService::regis(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    // {"msg_id":2,"name":"luochenhao","password":"123456"}
    // {"msg_id":2,"name":"lch","password":"123456"}

    //1.从Json中获取客户端发来的信息
    LOG_INFO << "do regis service!";
    string name = js["name"];
    string password = js["password"];

    User user;
    user.setName(name);
    user.setPassword(password);
    
    //2.将数据插入至服务器的数据库
    bool flag = _userModel.insert(user);
    if (flag) {
        //注册成功 向客户端返回json response应答
        json response;
        response["errno"] = 0;
        response["msg_id"] = REG_MSG_ACK;
        response["user_id"] = user.getId();
        conn->send(response.dump());
    } else {
        //注册失败
        json response;
        response["errno"] = 1;
        response["msg_id"] = REG_MSG_ACK;
        conn->send(response.dump());
    }
}

//处理登录业务 id password
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    // {"msg_id":1,"id":28,"password":"123456"}
    
    //1.从Json中获取客户端发来的信息
    LOG_INFO << "do login service!";
    int id = js["id"].get<int>();
    string password = js["password"];

    //2.用户登录验证
    User user = _userModel.query(id);
    if (user.getId() == -1) {
        //2-1 用户名不存在
        json response;
        response["msg_id"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "account not exist";
        conn->send(response.dump());
    } else if (user.getPassword() != password) {
        //2-2 用户名对应的密码错误
        json response;
        response["msg_id"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "account password error";
        conn->send(response.dump());
    } else if (user.getState() == "online") {
        //2-3 用户已经登录 不允许重复登录
        json response;
        response["msg_id"] = LOGIN_MSG_ACK;
        response["errno"] = 2;
        response["errmsg"] = "account already login";
        conn->send(response.dump());
    } else {
        //2-4 登录成功
        //（1）登录成功 记录用户的连接信息
        // map会被多个线程调用 需要考虑线程安全问题
        // 锁的粒度一定要小 否则多线程变成一个串行的线程 没有体现并发程序的优势
        {
            lock_guard<mutex> lock(_connMutex);
            _userConnMap.insert({id, conn});
        }

        //（2）需要更新用户状态信息
        user.setState("online");
        _userModel.updateState(user);

        //（3）返回响应消息
        json response;
        response["msg_id"] = LOGIN_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        response["name"] = user.getName();

        //（4）查询登录的用户是否有离线消息 如果有则从数据库中读取离线消息 并发送给登录用户
        vector<string> messages = _offMessageModel.query(id);
        if (!messages.empty()) {
            response["offlinemssage"] = messages;//json直接支持容器序列化与反序列化
            _offMessageModel.remove(id);//读取该用户的离线消息后 将该用户的所有离线消息删除
        }
        conn->send(response.dump());
    }
}


//处理客户端异常退出
void ChatService::clientCloseUnexpectedly(const TcpConnectionPtr &conn) {
    User user;
    //1.循环遍历_userConnMap 找到出现异常的conn将用户的conn信息从HashMap中删除
    //利用大括号 降低锁的粒度
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it) {
            if (it->second == conn) {
                user.setId(it->first);//记录出现异常的客户端连接id
                _userConnMap.erase(it);//将连接信息conn从_userConnMap中删除
                break;
            }
        }
    }

    //2.需要更新用户状态信息
    if (user.getId() != -1) {
        user.setState("offline");
        _userModel.updateState(user);
    }
}


void ChatService::singleChat(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    //lch登录 {"msg_id":1,"id":28,"password":"123456"}
    //zhangsan登录 {"msg_id":1,"id":13,"password":"123456"}
    //zhangsan给lch发送消息 {"msg_id":5, "from":13, "name":"zhangsan", "to":28, "message":"today is a good day"}
    //zhangsan给lch发送消息 {"msg_id":5, "from":13, "name":"zhangsan", "to":28, "message":"i want to go out for a walk."}
    
    int to = js["to"].get<int>();
    {   
        //1.临界资源上锁
        lock_guard<mutex> lock(_connMutex);

        //2.目标用户是否在线查询
        auto it = _userConnMap.find(to);
        /* 
            用户在线是进行消息转发需要放入 临界区加锁的原因：
            如果用户在线需要 使用在线用户的conn 在使用conn的时候有可能会在其他地方被删除（加入临界区保证conn不能被删除）
         */

        //3.目标用户在线进行消息发送 
        if (it != _userConnMap.end()) {
            //单聊目标在线 服务器直接转发消息给to用户
            TcpConnectionPtr tcpconnptr = it->second;
            tcpconnptr->send(js.dump());
            return;
        }
    }
    
    //4.单聊目标不在线 直接存储离线消息
    _offMessageModel.insert(to, js.dump());
}


void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    
}

