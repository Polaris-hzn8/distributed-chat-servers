/*************************************************************************
    > File Name: GroupModel.cpp
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Fri 05 May 2023 22:44:14 CST
************************************************************************/

#include "db.h"
#include "GroupModel.h"

//创建群组
bool GroupModel::createGroup(Group &group) {
    //1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into allgroup(groupname, groupdesc)values('%s', '%s')", group.getName().c_str(), group.getDesc().c_str());

    //2.向数据库插入群组信息
    Mysql mysql;
    if (mysql.connect()) {
        if (mysql.update(sql)) {
            int gid = mysql_insert_id(mysql.getConnection());
            group.setId(gid);
            return true;
        }
    }
    return false;
}

//加入群组
bool GroupModel::joinGroup(int uid, int gid, string grouprole) {
    //1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into groupuser values('%d', '%d', '%s')", gid, uid, grouprole.c_str());

    //2.向数据库插入群组用户信息
    Mysql mysql;
    if (mysql.connect()) {
        if (mysql.update(sql)) {
            return true;
        }
    }
    return false;
}

//查询用户所有的群组信息
vector<Group> GroupModel::queryGroups(int uid) {
    /*
        1.先根据userid在groupuser表中查询出该用户所有的群组信息
        2.再根据群组信息查询属于该群组的所有用户的userid 并且和user表进行多表联合查询 查询用户的详细信息
    */

    //1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select a.id, a.groupname, a.groupdesc from allgroup a inner join \
    groupuser b on a.id = b.groupid where b.userid = %d", uid);

    //2.连接数据库
    vector<Group> groups;
    Mysql mysql;
    if (!mysql.connect()) {
        LOG_INFO << "mysql connect failed in queryGroups";
        return groups;
    }

    //3.多表查询
    MYSQL_RES *res = mysql.query(sql);
    if (res != nullptr) {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr) {
            Group group;
            group.setId(atoi(row[0]));
            group.setName(row[1]);
            group.setDesc(row[2]);
            groups.push_back(group);
        }
        mysql_free_result(res);
    }

    //4.额外查询处理 vector<GroupUser> users;//存储群成员信息
    for (Group &group : groups) {
        sprintf(sql, "select a.id, a.name, a.state, b.grouprole from user a \
        inner join groupuser b on b.userid = a.id where b.groupid = %d", group.getId());
        
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr) {
                GroupUser groupuser;
                groupuser.setId(atoi(row[0]));
                groupuser.setName(row[1]);
                groupuser.setState(row[2]);
                groupuser.setRole(row[3]);
                group.getGroupUsers().push_back(groupuser);
            }
            mysql_free_result(res);
        }
    }
    return groups;
}

//根据gid查询群组的所有用户id 再将消息群发给群成员（群聊消息）
vector<int> GroupModel::queryGroupUsers(int uid, int gid) {
    char sql[1024] = {0};
    sprintf(sql, "select userid from groupuser where groupid = %d and userid != %d", gid, uid);

    vector<int> ids;
    Mysql mysql;
    if (mysql.connect()) {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr) {
                ids.push_back(atoi(row[0]));
            }
        }
        mysql_free_result(res);
    }
    return ids;
}


//解散群组
bool GroupModel::dismissGroup(Group &group) {

}

//退出群组
bool GroupModel::quitGroup(Group &group) {

}


