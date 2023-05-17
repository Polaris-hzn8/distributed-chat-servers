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
    _msgHandlerMap.insert({ACOUNT_FRESH_MSG, bind(&ChatService::sendNewestInfo, this, _1, _2, _3) });//更新客户端数据
    _msgHandlerMap.insert({LOGOUT_MSG, bind(&ChatService::clientClose, this, _1, _2, _3) });//更新客户端数据

    _msgHandlerMap.insert({ONE_CHAT_MSG, bind(&ChatService::chat, this, _1, _2, _3) });//单聊
    _msgHandlerMap.insert({ADD_FRIEND_MSG, bind(&ChatService::addFriend, this, _1, _2, _3) });//添加好友

    _msgHandlerMap.insert({CREATE_GROUP_MSG, bind(&ChatService::createGroup, this, _1, _2, _3) });//创建群聊
    _msgHandlerMap.insert({ADD_GROUP_MSG, bind(&ChatService::joinGroup, this, _1, _2, _3) });//加入群聊
    _msgHandlerMap.insert({GROUP_CHAT_MSG, bind(&ChatService::groupChat, this, _1, _2, _3) });//发送群消息

    //连接redis服务器
    if (_redis.connect()) {
        /* 连接上redis服务器后需要预置一个回调 */
        /* 因为redis底层会监听通道上的消息 如果需要上报消息 则需要通过这个回调函数（预先注册回调函数） */
        /* 通过绑定器绑定服务中的方法 this对象及其预留参数 channel_id channel_id->str */
        
        //1.bind包装上报消息的方法
        function<void(int, string)> func = std::bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2);
        //2.redis设置上报通道消息的回调方法
        _redis.set_notify_message_handler(func);//注册回调函数
    }
}

//获取消息id对应的事件处理器
MsgHandler ChatService::getHandler(int msgId) {
    auto it = _msgHandlerMap.find(msgId);
    //没有查询到结果 进行日志打印 或者直接返回一个空操作的事件处理器
    if (it == _msgHandlerMap.end()) {
        //LOG_ERROR << "msg_id" << msg_id << " can not find handler!";
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time) {
            LOG_ERROR << "msgId" << msgId << " can not find handler!";
        };
    } else {
        return _msgHandlerMap[msgId];
    }
}

/*
    ORM框架：Object Relation Map 对象关系映射
    业务层操作的都是对象, 在数据层 DAO 封装了所有数据的相应操作
*/

//处理注册业务 name passwd
void ChatService::regis(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    LOG_INFO << "do regis service";
    // {"msgId":2,"username":"luochenhao","password":"123456"}
    // {"msgId":2,"username":"lch","password":"123456"}

    //1.从Json中获取客户端发来的信息
    LOG_INFO << "do regis service!";
    cout << js << endl;
    string username = js["username"];
    string password = js["password"];

    User user;
    user.setName(username);
    user.setPassword(password);
    
    json response;
    response["msgId"] = REG_MSG_ACK;

    //2.将数据插入至服务器的数据库
    bool isUnique = _userModel.isUnique(username);
    if (!isUnique) {
        //注册用户名重复
        response["errno"] = 1;
        response["errmsg"] = "username already exist!~";
    } else {
        //username唯一可以进行注册操作
        if (!_userModel.insert(user)) {
            //数据库插入数据失败
            response["errno"] = 1;
            response["errmsg"] = "data insert failed, netword error!~";
        } else {
            //数据库插入数据成功
            response["errno"] = 0;
            response["sysmsg"] = username + " register success!";
            response["uid"] = user.getId();
        }
    }
    conn->send(response.dump());
}

//处理登录业务 id password
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    LOG_INFO << "do login service";
    // {"msgId":1,"uid":28,"password":"123456"}
    
    //1.从Json中获取客户端发来的信息
    cout << js << endl;
    int uid = js["uid"].get<int>();
    string password = js["password"];

    json response;
    response["msgId"] = LOGIN_MSG_ACK;

    //2.用户登录验证
    User user = _userModel.query(uid);

    if (user.getId() == -1) {
        //2-1 用户名不存在
        response["errno"] = 1;
        response["errmsg"] = "account not exist";
    } else if (user.getPassword() != password) {
        //2-2 用户名对应的密码错误
        LOG_INFO << "account password error!~";
        response["errno"] = 1;
        response["errmsg"] = "account password error";    
    } else if (user.getState() == "online") {
        //2-3 用户已经登录 不允许重复登录
        response["errno"] = 2;
        response["errmsg"] = "account already login";
    } else {
        //2-4 登录成功
        //（1）登录成功 记录用户的连接信息
        // map会被多个线程调用 需要考虑线程安全问题
        // 锁的粒度一定要小 否则多线程变成一个串行的线程 没有体现并发程序的优势
        LOG_INFO << "password and uid correct!";
        response["errno"] = 0;
        response["sysmsg"] = "login success.";
        {
            lock_guard<mutex> lock(_connMutex);
            _userConnMap.insert({uid, conn});
        }

        //（2）需要更新用户状态信息
        user.setState("online");
        _userModel.updateState(user);
        LOG_INFO << "update the user state!";

        //（3）用户以uid登录成功后 向redis订阅channel(uid)
        _redis.subscribe(uid);//消息队列中注册uid
        LOG_INFO << "subscribe redis channel finished!";

        //（4）查询登录的用户是否有离线消息 如果有则从数据库中读取离线消息 并发送给登录用户
        vector<string> messages = _offMessageModel.query(uid);
        if (!messages.empty()) {
            response["offlinemsgs"] = messages;//json直接支持容器序列化与反序列化
            _offMessageModel.remove(uid);//读取该用户的离线消息后 将该用户的所有离线消息删除
        }
        LOG_INFO << "query user offlinemsg finished!";

        //（5）设置返回的响应消息
        response["uid"] = user.getId();
        response["username"] = user.getName();

        //（6）查询该用户的好友列表 并封装成json格式 进行返回
        vector<User> friends = _friendModel.query(uid);
        if (!friends.empty()) {
            vector<string> friends_t;
            for (User user : friends) {
                json js;
                js["uid"] = user.getId();
                js["username"] = user.getName();
                js["state"] = user.getState();
                friends_t.push_back(js.dump());
            }
            response["friends"] = friends_t;
        }
        LOG_INFO << "query user friendList info finished!";

        //（7）查询登录用户的群组列表 并封装成json格式 进行返回
        vector<Group> groups = _groupModel.queryGroups(uid);
        if (!groups.empty()) {
            //封装群组信息
            vector<string> groups_t;
            for (Group group : groups) {
                json groupjs;
                groupjs["gid"] = group.getId();
                groupjs["groupname"] = group.getName();
                groupjs["groupdesc"] = group.getDesc();
                //封装群组信息中的 群组成员信息
                vector<GroupUser> groupusers = group.getGroupUsers();
                vector<string> groupusers_t;
                for (GroupUser groupuser : groupusers) {
                    json groupuserjs;
                    groupuserjs["uid"] = groupuser.getId();
                    groupuserjs["username"] = groupuser.getName();
                    groupuserjs["state"] = groupuser.getState();
                    groupuserjs["grouprole"] = groupuser.getRole();
                    groupusers_t.push_back(groupuserjs.dump());
                }
                groupjs["groupusers"] = groupusers_t;
                groups_t.push_back(groupjs.dump());
            }
            response["groups"] = groups_t;
        }
        LOG_INFO << "query user groupList info finished!";
    }
    conn->send(response.dump());
    LOG_INFO << "login service finished!";
}


//发送账户基本信息（用户客户端请求信息刷新）
void ChatService::sendNewestInfo(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    LOG_INFO << "do sendNewestInfo service!";
    int uid = js["uid"];
    User user = _userModel.query(uid);

    json response;
    response["msgId"] = ACOUNT_FRESH_ACK;
    response["errno"] = 0;
    //1.用户基本信息
    response["uid"] = user.getId();
    response["username"] = user.getName();

    //2.查询该用户的好友列表 并封装成json格式 进行返回
    vector<User> friends = _friendModel.query(uid);
    if (!friends.empty()) {
        vector<string> friends_t;
        for (User user : friends) {
            json js;
            js["uid"] = user.getId();
            js["username"] = user.getName();
            js["state"] = user.getState();
            friends_t.push_back(js.dump());
        }
        response["friends"] = friends_t;
    }

    //3.查询登录用户的群组列表 并封装成json格式 进行返回
    vector<Group> groups = _groupModel.queryGroups(uid);
    if (!groups.empty()) {
        //封装群组信息
        vector<string> groups_t;
        for (Group group : groups) {
            json groupjs;
            groupjs["gid"] = group.getId();
            groupjs["groupname"] = group.getName();
            groupjs["groupdesc"] = group.getDesc();
            //封装群组信息中的 群组成员信息
            vector<GroupUser> groupusers = group.getGroupUsers();
            vector<string> groupusers_t;
            for (GroupUser groupuser : groupusers) {
                json groupuserjs;
                groupuserjs["uid"] = groupuser.getId();
                groupuserjs["username"] = groupuser.getName();
                groupuserjs["state"] = groupuser.getState();
                groupuserjs["grouprole"] = groupuser.getRole();
                groupusers_t.push_back(groupuserjs.dump());
            }
            groupjs["groupusers"] = groupusers_t;
            groups_t.push_back(groupjs.dump());
        }
        response["groups"] = groups_t;
    }
    conn->send(response.dump());
}

//处理客户端正常退出
void ChatService::clientClose(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    LOG_INFO << "do logout service user choose to logout!";
    int uid = js["uid"];
    //1.循环遍历_userConnMap 找到出现异常的conn将用户的conn信息从HashMap中删除
    //利用大括号 降低锁的粒度
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(uid);
        if (it != _userConnMap.end()) _userConnMap.erase(it);
    }

    //2.需要更新用户状态信息
    User user;
    user.setId(uid);
    user.setState("offline");
    _userModel.updateState(user);

    //3.redis消息队列中取消订阅
    _redis.unsubscribe(uid);
}

//处理客户端异常退出
void ChatService::clientCloseUnexpectedly(const TcpConnectionPtr &conn) {
    LOG_INFO << "do logout service for client close";
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

    int uid = user.getId();
    //2.需要更新用户状态信息
    user.setState("offline");
    _userModel.updateState(user);

    //3.redis消息队列中取消订阅
    _redis.unsubscribe(uid);
}

//处理服务器异常退出
void ChatService::reset() {
    LOG_INFO << "do reset service for server close!";
    //在服务器退出后 将所有online状态的用户设置为offline
    _userModel.resetState();
}

//单聊
void ChatService::chat(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    LOG_INFO << "do chat service!";
    //lch登录 {"msgId":1,"uid":28,"password":"123456"}
    //zhangsan登录 {"msgId":1,"uid":13,"password":"123456"}
    //zhangsan给lch发送消息 {"msgId":5, "from":13, "username":"zhangsan", "to":28, "msg":"today is a good day"}
    //zhangsan给lch发送消息 {"msgId":5, "from":13, "username":"zhangsan", "to":28, "msg":"i want to go out for a walk."}
    
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

        //3.目标用户如果在本台服务器上在线 则直接进行消息发送 
        if (it != _userConnMap.end()) {
            //单聊目标在线 服务器直接转发消息给to用户
            LOG_INFO << "target user is online within same server!";
            TcpConnectionPtr tcpconnptr = it->second;
            tcpconnptr->send(js.dump());
            return;
        }
    }

    //4.目标用户如果在其他服务器上在线 则直接进行消息发送（将消息发送到redis消息队列上 由redis消息队列转发给对应用户）
    User user = _userModel.query(to);
    if (user.getState() == "online") {
        LOG_INFO << "target user is online in other servers!";
        _redis.publish(to, js.dump());//将消息发布到redis的channel_id(to)通道上 == 事件上报
        return;
    }
    
    //5.单聊目标 在所有的服务器上都不在线 直接存储离线消息
    _offMessageModel.insert(to, js.dump());
    LOG_INFO << "target user is offline, save the msg in datebase!";
}


//添加好友
void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    LOG_INFO << "do add friend service!";
    //zhangsan给lch发送添加好友的消息 {"msgId":6, "uid":13, "fid":28}
    int uid = js["uid"].get<int>();
    int fid = js["fid"].get<int>();
    _friendModel.insert(uid, fid);
    _friendModel.insert(fid, uid);
}


//创建群组
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    LOG_INFO << "do create group service!";
    int uid = js["uid"].get<int>();
    string groupname = js["groupname"];
    string groupdesc = js["groupdesc"];

    //存储新创建的群组信息
    Group group(-1, groupname, groupdesc);
    if (_groupModel.createGroup(group)) {
        //将创建者加入群组中
        _groupModel.joinGroup(uid, group.getId(), "creator");
    }
}

//加入群组
void ChatService::joinGroup(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    LOG_INFO << "do join group service!";
    int uid = js["uid"].get<int>();
    int gid = js["gid"].get<int>();
    _groupModel.joinGroup(uid, gid, "normal");
}

//进行群消息聊天
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    LOG_INFO << "do group chat service!";
    // {"uid":xx, "gid":xx, "msg":"xxxx"}
    //1.获取js数据
    int uid = js["uid"].get<int>();
    int gid = js["gid"].get<int>();
    
    //2.根据群用户不同在线状态 进行发送群消息 or 存储离线消息
    vector<int> guids = _groupModel.queryGroupUsers(uid, gid);
    /* c++中的map不是线程安全的map */
    lock_guard<mutex> lock(_connMutex);
    for (int guid : guids) {
        auto it  = _userConnMap.find(guid);
        if (it != _userConnMap.end()) {
            // case1 : 用户与好友处于同一台服务器上 并且好友处于在线状态 直接转发消息
            it->second->send(js.dump());
        } else {
            /* 目标用户在本台服务器上的_connMap中没有发现 */
            User user = _userModel.query(guid);
            if (user.getState() == "online") {
                // case2 : 用于处于不同服务器上 并且好友处于在线状态 通过redis消息队列向其他服务器发送消息
                _redis.publish(guid, js.dump());
            } else {
                // case3 : 用户的好友处于离线状态
                _offMessageModel.insert(uid, js.dump());
            }
        }
    }
}


//redis调用的回调函数
//从redis消息队列中获取订阅的消息
void ChatService::handleRedisSubscribeMessage(int uid, string msg) {
    LOG_INFO << "do handle redis subscribe message service! channel msg happened.";
    /* 消息会被redis转发到uid所在的服务器上 */
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(uid);
    if (it != _userConnMap.end()) {
        it->second->send(msg);
        return;
    }
    _offMessageModel.insert(uid, msg);//如果在订阅通道有消息一瞬间，客户端因为某种原因刚好下线了，只好存储离线消息（非常巧合）
}

