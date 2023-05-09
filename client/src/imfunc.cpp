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

//help command handler
unordered_map<string, string> commandMap = {
	{"help", "显示所有支持的命令 <usage> help"},
	{"chat", "1v1聊天 <usage> chat:fid:message"},
	{"addFriend", "添加好友 <usage> addFriend:fid"},
	{"createGroup", "创建群组 <usage> createGroup:groupname:groupdesc"},
	{"joinGroup", "加入群组 <usage> joinGroup:gid"},
	{"groupChat", "群聊消息 <usage> groupChat:gid:message"},
	{"logout", "退出登录 <usage> logout"}
};

//注册系统支持的客户端命令处理
unordered_map<string, function<void(int, string)>> commandHandlerMap = {
	{"help", help},
	{"singlechat", singlechat},
	{"addFriend", addFriend},
	{"createGroup", createGroup},
	{"joinGroup", joinGroup},
	{"groupChat", groupChat},
	{"logout", logout}
};

//获取系统时间
string getCurrentTime() {
    
}

//消息接收线程
void readTaskHandler(int clientfd) {
    for (;;) {
        //1.接收数据
        char buff[1024] = {0};
        if (recv(clientfd, buff, 1024, 0) <= 0) {
            close(clientfd);
            exit(-1);
        }

        //2.数据反序列化与分析
        json js = json::parse(buff);
        if (js["msgId"].get<int>() == ONE_CHAT_MSG) {
            /* 单聊消息 */
            string time = js["time"];
            int uid = js["uid"];
            string username = js["username"];
            string message = js["msg"];
            printf("<%s-%s-%d> : %s", time.c_str(), username.c_str(), uid, message.c_str());
            continue;
        }
    }
}

//消息接收线程
void writeTaskHandler(int clientfd) {

}


void help(int fd, string str) {
    cout << "system command list:" << endl;
    for (auto command : commandMap) cout << command.first << " : " << command.second << endl;
    cout << endl;
}

void singlechat(int clientfd, string str) {
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
    if (len == -1) cerr << "send singlechat msg failed!~" << request << endl;
}

void addFriend(int clientfd, string str) {
    int fid = atoi(str.c_str());
    json js;
    js["msgId"] = ADD_FRIEND_MSG;
    js["uid"] = userInfo_g.getId();
    js["fid"] = fid;
    string request = js.dump();

    int len = send(clientfd, request.c_str(), strlen(request.c_str()), 0);
    if (len == -1) cerr << "send singchat msg falied!~" << request << endl;
}

void createGroup(int, string) {

}

void joinGroup(int, string) {

}

void groupChat(int, string) {

}

void logout(int, string) {

}


