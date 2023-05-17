/*************************************************************************
    > File Name: db.cpp
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Wed 03 May 2023 11:36:47 CST
************************************************************************/

#include "db.h"
#include "read.h"

//1.初始化数据库连接
Mysql::Mysql() {
    _conn = mysql_init(nullptr);//开辟内存空间
}

//2.释放数据库连接资源
Mysql::~Mysql() {
    if (_conn != nullptr) mysql_close(_conn);//析构时释放内存空间
}

//3.连接数据库
bool Mysql::connect() {
    MYSQL *p = mysql_real_connect(_conn, dbip, dbusername, dbpassword, dbname, dbport, nullptr, 0);
    if (p != nullptr) {
        //C与C++代码默认的编码字符是ASCII set names gbk设置支持中文
        mysql_query(_conn, "set names gbk");
        LOG_INFO << "connect mysql success";
    } else {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ": Failed to connect to database: Error:" << mysql_error(_conn);
    }
    return p;
}

//4.更新操作
bool Mysql::update(string sql) {
    if (mysql_query(_conn, sql.c_str())) {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":" << sql << "database table update failed !";
        return false;
    }
    return true;
}

//5.查询操作
MYSQL_RES* Mysql::query(string sql) {
    if (mysql_query(_conn, sql.c_str())) {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":" << sql << "database table query failed!";
        return nullptr;
    }
    return mysql_use_result(_conn);
}

//获取数据库连接
MYSQL *Mysql::getConnection() { return _conn; }

