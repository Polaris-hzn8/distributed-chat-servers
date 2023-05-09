/*************************************************************************
	> File Name: sys.cpp
	> Author: luochenhao
	> Mail: 3453851623@qq.com
	> Created Time: Tue 09 May 2023 05:18:57 PM CST
 ************************************************************************/

#include "sys.h"
#include "head.h"
#include "common.h"
#include "imfunc.h"
#include "data.h"
#include "User.h"
#include "Group.h"
#include "GroupUser.h"

//获取系统时间
string getCurrentTime() {
    auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm *ptm = localtime(&t);
    int year = ptm->tm_year + 1900;
    int month = ptm->tm_mon + 1;
    int day = ptm->tm_mday;
    int hour = ptm->tm_hour;
    int minite = ptm->tm_min;
    int second = ptm->tm_sec;
    
    char content[60] = {0};
    sprintf(content, "%d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minite, second);
    return string(content);
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
		}
    }
}

//更新本地文件信息
void accountRefresh(json &response) {
	//cout << response << endl;
	memset(&userInfo_g, 0, sizeof(userInfo_g));
	friendList_g.clear();
	groupList_g.clear();

	/* 记录登录用户的uid与username */
	int uid = response["uid"].get<int>();
	string username = response["username"];
	userInfo_g.setId(uid);
	userInfo_g.setName(username);

	/* 记录当前用户的好友列表信息 */
	if (response.contains("friends")) {
		vector<string> friends = response["friends"];
		for (string friend_s : friends) {
			json friend_j = json::parse(friend_s);
			User friend_;
			friend_.setId(friend_j["uid"]);
			friend_.setName(friend_j["username"]);
			friend_.setState(friend_j["state"]);
			friendList_g.push_back(friend_);
		}
	}

	/* 记录当前用户的群组列表信息 */
	if (response.contains("groups")) {
		vector<string> groups = response["groups"];
		for (string group_s : groups) {
			json group_j = json::parse(group_s);
			Group group_;
			group_.setId(group_j["gid"]);
			group_.setName(group_j["groupname"]);
			group_.setDesc(group_j["groupdesc"]);
			
			vector<GroupUser> groupusers_;
			vector<string> groupusers = group_j["groupusers"];
			for (string groupuser_s : groupusers) {
				json groupuser_j = json::parse(groupuser_s);
				GroupUser groupuser_;
				groupuser_.setId(groupuser_j["uid"]);
				groupuser_.setName(groupuser_j["username"]);
				groupuser_.setState(groupuser_j["state"]);
				groupuser_.setRole(groupuser_j["grouprole"]);

				group_.getGroupUsers().push_back(groupuser_);
			}
			groupList_g.push_back(group_);
		}
	}
}


//显示当前登录用户的基本信息
void showAccountInfo() {
	//1.显示当前登录用户信息
	printf("=======================login user========================\n");
	printf("current login uid = %d, username = %s.\n", userInfo_g.getId(), userInfo_g.getName().c_str());

	//2.显示当前登录用户的好友列表
	printf("=======================friend list========================\n");
	if (!friendList_g.empty()) {
		for (User user : friendList_g) {
			printf("uid:%d\tusername:%s\tstate:%s\n", user.getId(), user.getName().c_str(), user.getState().c_str());
		}
	}

	//3.显示当前登录用户的群组列表
	printf("=======================group list========================\n");
	if (!groupList_g.empty()) {
		for (Group group : groupList_g) {
			printf("gid:%d\tgroupname:%s\tgroupdesc:%s\n", group.getId(), group.getName().c_str(), group.getDesc().c_str());
			for (GroupUser groupuser : group.getGroupUsers()) {
				printf("  uid:%d\tusername:%s\tgrouprole:%s\tstate:%s\n", 
					groupuser.getId(), 
					groupuser.getName().c_str(), 
					groupuser.getRole().c_str(), 
					groupuser.getState().c_str());
			}
		}
	}
}


//主聊天页面程序
void mainMenu(int clientfd) {
	help();

	char buff[1024] = {0};
	for (;;) {
		//1.输入commandbuff
		cin.getline(buff, 1024);
		string commandbuff(buff);//用buff构造一个string名为commandbuff
		string command;

		//2.commandbuff解析
		int idx = commandbuff.find(":");
		if (idx == -1) command = commandbuff;
		else command = commandbuff.substr(0, idx);

		//3.判断命令有效性
		auto it = commandHandlerMap.find(command);
		if (it == commandHandlerMap.end()) {
			/* 无效命令 */
			cerr << "invalid command, please input command follow the instructions." << endl;
			continue;
		} else {
			/* 有效命令 调用相应命令的事件处理回调 mainMenu对修改封闭 添加新功能不需要修改该函数 */
			it->second(clientfd, commandbuff.substr(idx + 1, commandbuff.size() - idx));
		}
	}
}

