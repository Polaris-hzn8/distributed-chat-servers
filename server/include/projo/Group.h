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
    Group(int id = -1, string name = "", string desc = "") {
        this->id = id;
        this->groupname = name;
        this->groupdesc = desc;
    }

    void setId(int groupid) { this->id = groupid; }
    void setName(string groupname) { this->groupname = groupname; }
    void setDesc(string groupdesc) { this->groupdesc = groupdesc; }
    
    int getId() { return this->id; }
    string getName() { return this->groupname; }
    string getDesc() { return this->groupdesc; }
    vector<GroupUser> &getGroupUsers() { return this->groupusers;}

private:
    int id;
    string groupname;
    string groupdesc;
    vector<GroupUser> groupusers;//存储群成员信息
};


#endif // !_GROUP_H

