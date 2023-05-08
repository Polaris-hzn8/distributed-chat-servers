/*************************************************************************
    > File Name: offmessage.h
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Thu 04 May 2023 21:49:50 CST
************************************************************************/

#ifndef _OFFMESSAGE_H
#define _OFFMESSAGE_H

#include "head.h"

// offmessage表的ORM类
class OffMessage {
public:
    OffMessage(int userid = -1, string message = "") {
        this->uid = uid;
        this->message = message;
    }

    void setUserId(int userid) { this->uid = uid; }
    void setMessage(string message) { this->message = message; }

    int getUserId() { return this->uid; }
    string getMessage() {return this->message; }

private:
    int uid;
    string message;
};

#endif