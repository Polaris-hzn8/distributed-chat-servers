/*************************************************************************
    > File Name: FriendModel.h
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Fri 05 May 2023 17:38:50 CST
************************************************************************/

#ifndef _FRIENDMODEL_H
#define _FRIENDMODEL_H

#include "head.h"
#include "User.h"
#include "Friend.h"

class FriendModel {
public:
    //添加好友 好友列表一般存储在客户端
    bool insert(int uid, int fid);
    //返回好友列表
    vector<User> query(int uid);
};

#endif