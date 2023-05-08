/*************************************************************************
    > File Name: user.h
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Wed 03 May 2023 11:46:54 CST
************************************************************************/

#ifndef _USER_H
#define _USER_H

#include "head.h"

// user表的ORM类
class User {
public:
    User(int uid = -1, string username = "", string password = "", string state = "offline") {
        this->uid = uid;
        this->username = username;
        this->password = password;
        this->state = state;
    }

    void setId(int id) { this->uid = uid; }
    void setName(string name) { this->username = username; }
    void setPassword(string password) { this->password = password; }
    void setState(string state) { this->state = state; }

    int getId() { return this->uid; }
    string getName() { return this->username; }
    string getPassword() { return this->password; }
    string getState() { return this->state; }

private:
    int uid;
    string username;
    string password;
    string state;
};

#endif // !_USER_H
