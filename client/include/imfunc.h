/*************************************************************************
	> File Name: im.h
	> Author: luochenhao
	> Mail: 3453851623@qq.com
	> Created Time: Tue 09 May 2023 08:27:44 AM CST
 ************************************************************************/

#ifndef _IMFUNC_H
#define _IMFUNC_H

#include "head.h"

//help command handler
extern unordered_map<string, string> commandMap;
//注册系统支持的客户端命令处理
extern unordered_map<string, function<void(int, string)>> commandHandlerMap;

void help(int fd = 0, string str = "");
void refresh(int fd = 0, string str = "");

void chat(int, string);
void addFriend(int, string);
void createGroup(int, string);
void joinGroup(int, string);
void groupChat(int, string);

void logout(int, string);


#endif