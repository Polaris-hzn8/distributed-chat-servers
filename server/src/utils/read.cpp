/*************************************************************************
    > File Name: configRead.cpp
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Thu 11 May 2023 11:45:46 CST
************************************************************************/

#include "head.h"
#include "read.h"

//服务器配置信息
int port;
char ip[50];

//数据库信息配置
char dbip[50];
int dbport;
char dbusername[100];
char dbpassword[100];
char dbname[100];


char ans[512];

void initConfig(const char *configpath) {
    if (access(configpath, R_OK)) {
        LOG_INFO << "config file don't exist!";
        exit(1);
    }

    char *info;
    info = get_conf_value(configpath, "IP");
    if (info == NULL) {
        LOG_INFO << "IP is not setted in configfile";
        exit(1);
    } else {
        sprintf(ip,"%s",info);
    }
    
    info = get_conf_value(configpath, "PORT");
    if (info == NULL) {
        LOG_INFO << "PORT is not setted in configfile";
        exit(1);
    } else {
        port = atoi(info);
    }

    info = get_conf_value(configpath, "DBIP");
    if (info == NULL) {
        LOG_INFO << "DBIP is not setted in configfile";
        exit(1);
    } else {
        sprintf(dbip,"%s",info);
    }

    info = get_conf_value(configpath, "DBPORT");
    if (info == NULL) {
        LOG_INFO << "DBPORT is not setted in configfile";
        exit(1);
    } else {
        dbport = atoi(info);
    }

    info = get_conf_value(configpath, "DBUSERNAME");
    if (info == NULL) {
        LOG_INFO << "DBUSERNAME is not setted in configfile";
        exit(1);
    } else {
        sprintf(dbusername,"%s",info);
    }

    info = get_conf_value(configpath, "DBPASSWORD");
    if (info == NULL) {
        LOG_INFO << "DBPASSWORD is not setted in configfile";
        exit(1);
    } else {
        sprintf(dbpassword,"%s",info);
    }

    info = get_conf_value(configpath, "DBNAME");
    if (info == NULL) {
        LOG_INFO << "DBNAME is not setted in configfile";
        exit(1);
    } else {
        sprintf(dbname,"%s",info);
    }

    printf("configfile read complete, serverip:%s, serverport:%d, dbip:%s, dbport:%d, dbusername:%s, dbpassword:%s, dbname:%s\n", 
    ip, port, dbip, dbport, dbusername, dbpassword, dbname);
}

char *get_conf_value(const char *filename, const char *key) {
    FILE *fp;
    char *line = NULL;//获取的行
    char *sub = NULL;//找到的目标下标指针
    long unsigned int len = 0, nread = 0;
    bzero(ans, sizeof(ans));
    //1.打开配置文件
    if ((fp = fopen(filename, "r")) == NULL) return NULL;
    
	//2.逐行读取文件中的信息 进行目标key的查找
    while ((nread = getline(&line, &len, fp)) != -1) {
        if ((sub = strstr(line, key)) == NULL) continue;
        if (sub == line && line[strlen(key)] == '=') {
            /* sub == line && line[strlen(key)]为等于号 */
            strcpy(ans, line + strlen(key) + 1);
            if (ans[strlen(ans) - 1] == '\n') {
                ans[strlen(ans) - 1] = '\0';//行尾的换行符处理
            }
        }
    }
    fclose(fp);
    free(line);
    if (!strlen(ans)) return NULL;
    //printf("%s\n", ans);
    return ans;
}


