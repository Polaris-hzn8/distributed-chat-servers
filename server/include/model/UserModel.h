/*************************************************************************
    > File Name: UserModel.h
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Wed 03 May 2023 12:08:17 CST
************************************************************************/

#ifndef _USERMODEL_H
#define _USERMODEL_H

#include "User.h"

class UserModel {
public:
    //向User表中插入的username是否唯一
    bool isUnique(string username);
    //User表插入操作
    bool insert(User &user);
    //根据id查询对应的User数据
    User query(int uid);
    //更新用户状态
    bool updateState(User user);
    //重置用户状态信息
    bool resetState();
};


#endif // !_USERMODEL_H