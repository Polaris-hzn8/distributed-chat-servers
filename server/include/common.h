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
	LOGIN_MSG_ACK
};

//chatservice方法实现：主要就是将消息id 以及对应的绑定写到map表中

#endif
