/*************************************************************************
    > File Name: FriendModel.cpp
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Fri 05 May 2023 18:07:23 CST
************************************************************************/

#include "db.h"
#include "head.h"
#include "FriendModel.h"

//添加好友 好友列表一般存储在客户端
bool FriendModel::insert(int uid, int fid) {
    //1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into friend values(%d, %d)", uid, fid);
    
    //2.数据更新
    Mysql mysql;
    if (mysql.connect()) {
        if (mysql.update(sql)) return true;
    }
    return false;
}


//返回好友列表
vector<User> FriendModel::query(int uid) {
    //1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select a.id, a.name, a.state from user a inner join friend b on b.friendid = a.id where b.userid = %d", uid);
    
    //2.数据查询
    vector<User> friends;
    Mysql mysql;
    if (mysql.connect()) {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr) {
            /* 将查询到的对应userid的离线消息存入vector数组中 */
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr) {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                friends.push_back(user);
            }
        }
        mysql_free_result(res);
    }
    return friends;
}

