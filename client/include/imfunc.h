/*************************************************************************
	> File Name: im.h
	> Author: luochenhao
	> Mail: 3453851623@qq.com
	> Created Time: Tue 09 May 2023 08:27:44 AM CST
 ************************************************************************/

#ifndef _IMFUNC_H
#define _IMFUNC_H

#include "head.h"

//获取系统时间
string getCurrentTime();
//消息接收线程
void readTaskHandler(int clientfd);
//消息接收线程
void writeTaskHandler(int clientfd);

//显示当前登录用户的基本信息
void showAccountInfo();

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
	{"logout", logout},
};

void help(int fd = 0, string str = "");

void singlechat(int, string);
void addFriend(int, string);
void createGroup(int, string);
void joinGroup(int, string);
void groupChat(int, string);

void logout(int, string);

#endif
