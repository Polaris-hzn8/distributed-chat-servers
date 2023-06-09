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

//信号量用于读写线程之间的通信
extern sem_t rwsem;

//记录菜单窗口是否运行
extern bool isMainMenuRunning_g;

//记录登录状态是否成功（atomic 该变量在多线程环境中读写 防止由于线程安全而出现的竞态条件问题）
//对于单个变量的值的修改 没有必要使用互斥锁来控制 使用基于cs的原子类型即可
extern atomic_bool isLoginSuccess_g;





#endif
