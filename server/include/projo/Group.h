/*************************************************************************
    > File Name: Group.h
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Fri 05 May 2023 22:24:43 CST
************************************************************************/

#ifndef _GROUP_H
#define _GROUP_H

#include "head.h"
#include "GroupUser.h"

class Group {
public:
    Group(int gid = -1, string groupname = "", string groupdesc = "") {
        this->gid = gid;
        this->groupname = groupname;
        this->groupdesc = groupdesc;
    }

    void setId(int groupid) { this->gid = gid; }
    void setName(string groupname) { this->groupname = groupname; }
    void setDesc(string groupdesc) { this->groupdesc = groupdesc; }
    
    int getId() { return this->gid; }
    string getName() { return this->groupname; }
    string getDesc() { return this->groupdesc; }
    vector<GroupUser> &getGroupUsers() { return this->groupusers;}

private:
    int gid;
    string groupname;
    string groupdesc;
    vector<GroupUser> groupusers;//存储群成员信息
};


#endif // !_GROUP_H

