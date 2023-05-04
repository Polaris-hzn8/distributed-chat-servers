/*************************************************************************
    > File Name: UserModel.h
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Wed 03 May 2023 12:08:17 CST
************************************************************************/

#ifndef _USERMODEL_H
#define _USERMODEL_H

#include "user.h"

class UserModel {
public:
    //User表插入操作
    bool insert(User &user);
    //根据id查询对应的User数据
    User query(int id);
    //更新用户状态
    bool updateState(User user);
};


#endif // !_USERMODEL_H