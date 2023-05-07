/*************************************************************************
    > File Name: GroupModel.h
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Fri 05 May 2023 22:44:06 CST
************************************************************************/

#ifndef _GROUPMODEL_H
#define _GROUPMODEL_H

#include "Group.h"

class GroupModel {
public:
    //创建群组
    bool createGroup(Group &group);
    //加入群组
    bool addGroup(int userid, int groupid, string grouprole);
    //查询用户所在群组信息
    vector<Group> queryGroups(int userid);
    //根据gid查询群组用户列表 主要用于群聊消息
    vector<int> queryGroupUsers(int userid, int groupid);
    
    //解散群组
    bool dismissGroup(Group &group);
    //退出群组
    bool quitGroup(Group &group);

private:

};

#endif // !_GROUPMODEL_H


