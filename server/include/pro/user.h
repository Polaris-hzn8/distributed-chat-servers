/*************************************************************************
    > File Name: user.h
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Wed 03 May 2023 11:46:54 CST
************************************************************************/

#ifndef _USER_H
#define _USER_H

#include "head.h"

// User表的ORM类
class User {
public:
    User(int id = -1, string name = "", string passwd = "", string state = "offline") {
        this->id = id;
        this->name = name;
        this->passwd = passwd;
        this->state = state;
    }

    void setId(int id) { this->id = id; }
    void setName(string name) { this->name = name; }
    void setPasswd(string passwd) { this->passwd = passwd; }
    void setState(string state) { this->state = state; }

    int getId() { return this->id; }
    string getName() { return this->name; }
    string getPasswd() { return this->passwd; }
    string getState() { return this->state; }

private:
    int id;
    string name;
    string passwd;
    string state;
};

#endif // !_USER_H
