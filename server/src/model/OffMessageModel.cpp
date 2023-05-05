/*************************************************************************
    > File Name: OffMessageModel.cpp
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Thu 04 May 2023 22:19:26 CST
************************************************************************/

#include "db.h"
#include "OffMessageModel.h"

//离线消息存储 offmessage表插入操作
bool OffMessageModel::insert(int userid, string message) {
    //1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into offlinemessage(userid, message) values(%d, '%s')", userid, message.c_str());
    
    //2.数据插入
    Mysql mysql;
    if (mysql.connect()) if (mysql.update(sql)) return true;
    return false;
}

//离线消息删除 offmessage表删除操作
bool OffMessageModel::remove(int userid) {
    //1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "delete from offlinemessage where userid = %d", userid);
    
    //2.数据插入
    Mysql mysql;
    if (mysql.connect()) if (mysql.update(sql)) return true;
    return false;
}

//离线消息查询 根据id查询对应的offmessage数据
vector<string> OffMessageModel::query(int userid) {
    //1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select message from offlinemessage where userid = %d", userid);
    
    //2.数据查询
    vector<string> messages;
    Mysql mysql;
    if (mysql.connect()) {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr) {
            /* 将查询到的对应userid的离线消息存入vector数组中 */
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr) {
                messages.push_back(row[0]);
            }
        }
        mysql_free_result(res);
    }
    return messages;
}

