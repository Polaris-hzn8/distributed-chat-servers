/*************************************************************************
    > File Name: OffMessageModel.h
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Thu 04 May 2023 22:03:18 CST
************************************************************************/

#ifndef _OFFMESSAGEMODEL_H
#define _OFFMESSAGEMODEL_H

#include "head.h"
#include "OffMessage.h"

//提供离线消息表的操作接口方法
class OffMessageModel {
public:
    //离线消息存储 offmessage表插入操作
    bool insert(int userid, string message);
    //离线消息删除 offmessage表删除操作
    bool remove(int userid);
    //离线消息查询 根据id查询对应的offmessage数据
    vector<string> query(int userid);
};


#endif // !_USERMODEL_H