/*************************************************************************
    > File Name: subthread.cpp
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Wed 17 May 2023 08:56:15 CST
************************************************************************/

#include "head.h"
#include "data.h"
#include "common.h"
#include "sys.h"

void processLoginResponse(json &response);
void processRegResponse(json &response);
void processRefreshResponse(json &response);

//消息接收线程 用于处理服务端发送来的各种响应消息
void readTaskHandler(int clientfd) {
    for (;;) {
        //1.接收数据
        char buff[1024] = {0};
        if (recv(clientfd, buff, 1024, 0) <= 0) {
            close(clientfd);
            exit(-1);
        }

        //2.数据反序列化与解析
        json js = json::parse(buff);
        int msgId = js["msgId"].get<int>();
        if (msgId == ONE_CHAT_MSG) {
            /* 客户端收到好友的单聊消息 */
            int fid = js["from"].get<int>();
            string time = js["time"];
            string username = js["username"];
            string message = js["msg"];

			printf("<%s %d %s> : %s\n", time.c_str(), fid, username.c_str(), message.c_str());
        } else if (msgId == GROUP_CHAT_MSG) {
			/* 客户单收到群聊消息 */
			int gid = js["gid"].get<int>();
			int uid = js["uid"].get<int>();
			string username = js["username"];
			string message = js["msg"];
            string time = js["time"];
			printf("<Group:%d><%s %d %s> : %s\n", gid, time.c_str(), uid, username.c_str(), message.c_str());
		} else if (msgId == LOGIN_MSG_ACK) {
			/* 处理登录的响应消息 */
			processLoginResponse(js);
			sem_post(&rwsem);//通知主线程 登录结果处理完成
		} else if (msgId == REG_MSG_ACK) {
            /* 处理注册响应消息 */
			processRegResponse(js);
			sem_post(&rwsem);//通知主线程 注册结果处理完成
		} else if (msgId == ACOUNT_FRESH_ACK) {
            /* 处理用户信息刷新响应消息 */
            processRefreshResponse(js);
        }
    }
}

//处理登录的响应消息 json数据处理
void processLoginResponse(json &response) {
	cout << response << endl;
	if (response["errno"].get<int>() != 0) {
		//1.登录信息有误 打印服务器返回的异常信息
		string errmsg = response["errmsg"]; 
		cerr << errmsg << endl;
	} else {
		//2.登录成功 初始化本地数据
		accountRefresh(response);
		//3.显示当前登录用户的基本信息
		showAccountInfo();
		//4.显示当前用户的离线消息（私聊离线信息 & 群组离线消息）
		if (response.contains("offlinemsgs")) {
			printf("You have offlinemsgs from your friends or Groups!~\n");
			vector<string> offlinemsgs = response["offlinemsgs"];
			for (string offlinemsg_s : offlinemsgs) {
				json offlinemsg_j = json::parse(offlinemsg_s);
				int msgId = offlinemsg_j["msgId"].get<int>();
				if (msgId == ONE_CHAT_MSG) {
					/* 客户端收到好友的单聊离线消息 */
					int fid = offlinemsg_j["from"].get<int>();
					string time = offlinemsg_j["time"];
					string username = offlinemsg_j["username"];
					string message = offlinemsg_j["msg"];
					printf("<%s %d %s> : %s\n", time.c_str(), fid, username.c_str(), message.c_str());
				} else if (msgId == GROUP_CHAT_MSG) {
					/* 客户单收到群聊离线消息 */
					int gid = offlinemsg_j["gid"].get<int>();
					int uid = offlinemsg_j["uid"].get<int>();
					string username = offlinemsg_j["username"];
					string message = offlinemsg_j["msg"];
					string time = offlinemsg_j["time"];
					printf("<Group:%d><%s %d %s> : %s\n", gid, time.c_str(), uid, username.c_str(), message.c_str());
				}
			}
		}
		//5.登录消息处理结束 结果为登录成功
		isLoginSuccess_g = true;
	}
}

//处理注册的响应消息 json数据处理（注册成功与否都是回到首界面 所以需要添加变量来判定注册结果TF）
void processRegResponse(json &response) {
	if (response["errno"].get<int>() != 0) {
		string errmsg = response["errmsg"];
		cerr << errmsg << endl;
	} else {
		string errmsg = response["sysmsg"];
		cerr << errmsg << endl;
		cout << response["username"] << " register success, userid is " << response["uid"] << ", don't forget it!" << endl;
	}
}

//处理用户信息刷新的响应消息 json数据处理
void processRefreshResponse(json &response) {
    if (response["errno"].get<int>() == 0) {
        accountRefresh(response);
        showAccountInfo();
        return;
    }
    cout << "account refresh error please check your network." << endl;
}



