/*************************************************************************
    > File Name: GroupUser.h
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Fri 05 May 2023 22:25:18 CST
************************************************************************/

#ifndef _GROUPUSER_H
#define _GROUPUSER_H

#include "User.h"
#include "head.h"

class GroupUser : public User {
public:
    void setRole(string grouprole) { this->grouprole = grouprole; }
    string getRole() { return this->grouprole; }
private:
    string grouprole;
};

#endif // !_GROUPUSER_H