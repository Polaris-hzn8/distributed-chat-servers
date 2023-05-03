/*************************************************************************
    > File Name: UserModel.cpp
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Wed 03 May 2023 12:14:49 CST
************************************************************************/

#include "head.h"
#include "UserModel.h"
#include "db.h"

bool UserModel::insert(User &user) {
    //1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into user(name, password, state) values('%s', '%s', '%s')",
        user.getName().c_str(), user.getPasswd().c_str(), user.getState().c_str());
    
    //2.数据插入
    Mysql mysql;
    if (mysql.connect()) {
        if (mysql.update(sql)) {
            //在数据插入成功后 将自动生成的id返回给用户
            int id = mysql_insert_id(mysql.getConnection());
            user.setId(id);
            return true;
        }
    }
    return false;
}






