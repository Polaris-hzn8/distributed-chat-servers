/*************************************************************************
	> File Name: data.h
	> Author: luochenhao
	> Mail: 3453851623@qq.com
	> Created Time: Tue 09 May 2023 08:29:57 AM CST
 ************************************************************************/

#ifndef _DATA_H
#define _DATA_H

#include "head.h"
#include "User.h"
#include "Group.h"

//将部分服务端部分消息在客户端进行保存 当客户端需要使用时可以直接使用
//减轻访问服务器次数 减轻服务器压力
//登录的用户信息
extern User userInfo_g;
//登录用户的好友列表
extern vector<User> friendList_g;
//登录用户的群组信息列表
extern vector<Group> groupList_g;

extern bool isMainMenuRunning;

#endif
