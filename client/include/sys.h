/*************************************************************************
	> File Name: sys.h
	> Author: luochenhao
	> Mail: 3453851623@qq.com
	> Created Time: Tue 09 May 2023 05:18:48 PM CST
 ************************************************************************/

#ifndef _SYS_H
#define _SYS_H

#include "head.h"

//获取系统时间
string getCurrentTime();

//显示当前登录用户的基本信息
void showAccountInfo();

//主聊天页面程序
void mainMenu(int clientfd);

//刷新本地文件
void accountRefresh(json &response);

#endif
