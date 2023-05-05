/*************************************************************************
    > File Name: Friend.h
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Fri 05 May 2023 17:34:40 CST
************************************************************************/

#ifndef _FRIEND_H
#define _FRIEND_H

#include "head.h"

class Friend {
public:
    Friend(int userid = -1, int friendid = -1) {
        this->userid = userid;
        this->friendid = friendid;
    }

    void setUserId(int userid) { this->userid = userid; }
    void setFriendId(int friendid) { this->friendid = friendid; }

    int getUserId() { return this->userid; }
    int getFriendId() { return this->friendid; }

private:
    int userid;
    int friendid;
};


#endif