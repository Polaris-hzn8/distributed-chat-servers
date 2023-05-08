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
    Friend(int uid = -1, int fid = -1) {
        this->uid = uid;
        this->fid = fid;
    }

    void setUserId(int uid) { this->uid = uid; }
    void setFriendId(int fid) { this->fid = fid; }

    int getUserId() { return this->uid; }
    int getFriendId() { return this->fid; }

private:
    int uid;
    int fid;
};


#endif