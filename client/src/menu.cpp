/*************************************************************************
	> File Name: menu.cpp
	> Author: luochenhao
	> Mail: 3453851623@qq.com
	> Created Time: Tue 09 May 2023 08:28:29 AM CST
 ************************************************************************/

#include "head.h"
#include "imfunc.h"
#include "menu.h"
#include "data.h"

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
		else {
			command = commandbuff.substr(0, idx);
		}

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

//显示当前登录用户的基本信息
void showAccountInfo() {
	//1.显示当前登录用户信息
	printf("=======================login user========================");
	printf("current login uid = %d, username = %s.", userInfo_g.getId(), userInfo_g.getName());

	//2.显示当前登录用户的好友列表
	printf("=======================friend list========================");
	if (!friendList_g.empty()) {
		for (User user : friendList_g) {
			printf("uid:%d\tusername:%s\tstate:%s\n", user.getId(), user.getName(), user.getState());
		}
	}

	//3.显示当前登录用户的群组列表
	printf("=======================group list========================");
	if (!groupList_g.empty()) {
		for (Group group : groupList_g) {
			printf("gid:%d\tgroupname:%s\tgroupdesc:%s\n", group.getId(), group.getName(), group.getDesc());
			for (GroupUser groupuser : group.getGroupUsers()) {
				printf("uid:%d\tusername:%s\tgrouprole\tstate:%s\n", groupuser.getId(), groupuser.getName(), groupuser.getRole(), groupuser.getState());
			}
		}
	}
	
}


