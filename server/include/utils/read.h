/*************************************************************************
	> File Name: configRead.h
	> Author: luochenhao
	> Mail: 3453851623@qq.com
	> Created Time: Thu 11 May 2023 11:45:24 AM CST
 ************************************************************************/

#ifndef _CONFIGREAD_H
#define _CONFIGREAD_H

#include "head.h"

//服务器配置信息
extern int port;
extern char ip[50];

//数据库信息配置
extern char dbip[50];
extern int dbport;
extern char dbusername[100];
extern char dbpassword[100];
extern char dbname[100];

void initConfig(const char *configpath);

extern char ans[512];//用于存储查询到的配置信息的临时容器
char *get_conf_value(const char *filename, const char *key);

#endif
