/*************************************************************************
    > File Name: UserModel.cpp
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Wed 03 May 2023 12:14:49 CST
************************************************************************/

#include "db.h"
#include "head.h"
#include "UserModel.h"

bool UserModel::isUnique(string username) {
    //1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select count(*) from user where name = '%s'", username.c_str());

    //2.数据查询
    Mysql mysql;
    if (mysql.connect()) {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr) {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr) {
                int count = stoi(row[0]);
                if (count) return false;
            }
        }
    }
    return true;
}

bool UserModel::insert(User &user) {
    //1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into user(name, password, state) values('%s', '%s', '%s')",
        user.getName().c_str(), user.getPassword().c_str(), user.getState().c_str());
    
    //2.数据插入
    Mysql mysql;
    if (mysql.connect()) {
        if (mysql.update(sql)) {
            //在数据插入成功后 将自动生成的id返回给用户
            //获取插入成功的用户数据 生成的主键id
            int uid = mysql_insert_id(mysql.getConnection());
            user.setId(uid);
            return true;
        }
    }
    return false;
}

User UserModel::query(int uid) {
    //1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select * from user where id = %d", uid);
    
    //2.数据查询
    Mysql mysql;
    if (mysql.connect()) {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr) {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr) {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPassword(row[2]);
                user.setState(row[3]);
                return user;
            }
        }
        mysql_free_result(res);
    }
    return User();
}

bool UserModel::updateState(User user) {
    //1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "update user set state = '%s' where id = %d", user.getState().c_str(), user.getId());
    printf("%s\n", sql);
    
    //2.数据更新
    Mysql mysql;
    if (mysql.connect()) {
        if (mysql.update(sql)) return true;
    }
    return false;
}

bool UserModel::resetState() {
    //1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "update user set state = 'offline' where state = 'online';");
    
    //2.数据更新
    Mysql mysql;
    if (mysql.connect()) {
        if (mysql.update(sql)) return true;
    }
    return false;
}



