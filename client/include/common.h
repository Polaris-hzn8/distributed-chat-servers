/*************************************************************************
	> File Name: common.h
	> Author: luochenhao
	> Mail: 3453851623@qq.com
	> Created Time: Sun 30 Apr 2023 05:21:57 PM CST
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H

enum EnMsgType {
	LOGIN_MSG = 1, //登录消息
	REG_MSG, //注册消息
	REG_MSG_ACK, //注册响应消息
	LOGIN_MSG_ACK, //登录响应消息
	
	ONE_CHAT_MSG, //单聊消息
	ADD_FRIEND_MSG, //添加好友消息
	
	CREATE_GROUP_MSG, //创建群聊
	ADD_GROUP_MSG, //加入群聊
	GROUP_CHAT_MSG //发送群聊消息
};


#endif
