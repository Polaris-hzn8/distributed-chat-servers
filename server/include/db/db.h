/*************************************************************************
    > File Name: db.h
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Wed 03 May 2023 11:16:46 CST
***********************************************************************/

#ifndef _DB_H
#define _DB_H

#include "head.h"
#include <mysql/mysql.h>

//数据库操作类
class Mysql {
public:
    //1.初始化数据库连接
    Mysql();
    //2.释放数据库连接资源
    ~Mysql();
    //3.连接数据库
    bool connect();
    //4.更新操作
    bool update(string sql);
    //5.查询操作
    MYSQL_RES* query(string sql);
    //获取mysql连接
    MYSQL *getConnection();
    
private:
    MYSQL *_conn;
};

#endif