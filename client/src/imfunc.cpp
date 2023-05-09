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

//显示当前登录用户的基本信息
void showAccountInfo() {
	
}


void help(int fd = 0, string str = "") {
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


