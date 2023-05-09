/*************************************************************************
	> File Name: im.cpp
	> Author: luochenhao
	> Mail: 3453851623@qq.com
	> Created Time: Tue 09 May 2023 08:28:00 AM CST
 ************************************************************************/

#include "head.h"
#include "common.h"
#include "imfunc.h"
#include "data.h"
#include "sys.h"

//help command handler
unordered_map<string, string> commandMap = {
	{"help", "显示所有支持的命令 <usage> help"},
	{"chat", "1v1聊天 <usage> chat:fid:message"},
	{"addFriend", "添加好友 <usage> addFriend:fid"},
	{"createGroup", "创建群组 <usage> createGroup:groupname:groupdesc"},
	{"joinGroup", "加入群组 <usage> joinGroup:gid"},
	{"groupChat", "群聊消息 <usage> groupChat:gid:message"},
	{"logout", "退出登录 <usage> logout"},
    {"refresh", "刷新好友列表与群组列表 <usage> refresh"},
};

//注册系统支持的客户端命令处理
unordered_map<string, function<void(int, string)>> commandHandlerMap = {
	{"help", help},
	{"chat", chat},
	{"addFriend", addFriend},
	{"createGroup", createGroup},
	{"joinGroup", joinGroup},
	{"groupChat", groupChat},
	{"logout", logout},
    {"refresh", refresh}
};

//<usage> help
void help(int clientfd, string str) {
    cout << "system command list:" << endl;
    for (auto command : commandMap) cout << command.first << " : " << command.second << endl;
    cout << endl;
}

//<usage> chat:fid:message
void chat(int clientfd, string str) {
    //1.str命令验证
    int idx = str.find(":");
    if (idx == -1) {
        cerr << "chat command invalid!~" << endl;
        return;
    }

    //2.str命令参数获取 fid:message
    int fid = atoi(str.substr(0, idx).c_str());
    string message = str.substr(idx + 1, str.size() - idx);

    //3.组装json并发送数据
    json js;
    js["msgId"] = ONE_CHAT_MSG;
    js["from"] = userInfo_g.getId();
    js["username"] = userInfo_g.getName();
    js["to"] = fid;
    js["msg"] = message;
    js["time"] = getCurrentTime();
    string request = js.dump();

    int len = send(clientfd, request.c_str(), strlen(request.c_str()), 0);
    if (len == -1) cerr << "send chat msg failed!~" << request << endl;
}

//<usage> addFriend:fid
void addFriend(int clientfd, string str) {
    int fid = atoi(str.c_str());
    json js;
    js["msgId"] = ADD_FRIEND_MSG;
    js["uid"] = userInfo_g.getId();
    js["fid"] = fid;
    string request = js.dump();

    int len = send(clientfd, request.c_str(), strlen(request.c_str()), 0);
    if (len == -1) cerr << "send chat msg falied!~" << request << endl;
}

//<usage> refresh
void refresh(int clientfd, string str) {
    //1.发送用户数据刷新请求
    json js;
    js["msgId"] = ACOUNT_FRESH_MSG;
    js["uid"] = userInfo_g.getId();
    string request = js.dump();
    int len = send(clientfd, request.c_str(), strlen(request.c_str()), 0);
    if (len == -1) cerr << "send refresh request falied!~" << request << endl;

    //2.接受客户端发送的response
    /* 接受json response 数据 */
    char buff[1024] = {0};
    if ((recv(clientfd, buff, 1024, 0)) == -1) cerr << "recv refresh response error!~" << request << endl;
    else {
        json response = json::parse(buff);
        accountRefresh(response);
        showAccountInfo();
        return;
    }
    cout << "account refresh error please check your network." << endl;
}

//<usage> createGroup:groupname:groupdesc
void createGroup(int clientfd, string str) {
    int idx = str.find(":");
    if (idx == -1) {
        cerr << "createGroup command invalid!~" << endl;
        return;
    }
    //1.获取参数
    string groupname = str.substr(0, idx);
    string groupdesc = str.substr(idx + 1, str.size() - idx);

    //2.组装json字符串
    json js;
    js["msgId"] = CREATE_GROUP_MSG;
    js["uid"] = userInfo_g.getId();
    js["groupname"] = groupname;
    js["groupdesc"] = groupdesc;
    string request = js.dump();

    int len = send(clientfd, request.c_str(), strlen(request.c_str()), 0);
    if (len == -1) cerr << "send group create msg falied!~" << request << endl;
}

//<usage> joinGroup:gid
void joinGroup(int clientfd, string str) {
    int gid = atoi(str.c_str());
    json js;
    js["msgId"] = ADD_GROUP_MSG;
    js["uid"] = userInfo_g.getId();
    js["gid"] = gid;
    string request = js.dump();

    int len = send(clientfd, request.c_str(), strlen(request.c_str()), 0);
    if (len == -1) cerr << "send group create msg falied!~" << request << endl;
}

//<usage> groupChat:gid:message
void groupChat(int, string) {

}

//<usage> logout
void logout(int, string) {

}


