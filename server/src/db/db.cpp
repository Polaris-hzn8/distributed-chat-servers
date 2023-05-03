/*************************************************************************
    > File Name: db.cpp
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Wed 03 May 2023 11:36:47 CST
************************************************************************/

#include "db.h"

//数据库信息配置
static string serverip = "192.168.172.133";
static string user = "root";
static string passwd = "20001201";
static string dbname = "chat";

//1.初始化数据库连接
Mysql::Mysql() {
    _conn = mysql_init(nullptr);
}

//2.释放数据库连接资源
Mysql::~Mysql() {
    if (_conn != nullptr) mysql_close(_conn);
}

//3.连接数据库
bool Mysql::connect() {
    MYSQL *p = mysql_real_connect(_conn, serverip.c_str(), user.c_str(), passwd.c_str(), dbname.c_str(), 3306, nullptr, 0);
    if (p != nullptr) {
        //C与C++代码默认的编码字符是ASCII set names gbk设置支持中文
        mysql_query(_conn, "set names gbk");
        LOG_INFO << "connect mysql success";
    } else {
        LOG_INFO << "connect mysql fail";
    }
    return p;
}

//4.更新操作
bool Mysql::update(string sql) {
    if (mysql_query(_conn, sql.c_str())) {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":" << sql << "数据库更新失败!";
        return false;
    }
    return true;
}

//5.查询操作
MYSQL_RES* Mysql::query(string sql) {
    if (mysql_query(_conn, sql.c_str())) {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":" << sql << "数据库查询失败!";
        return nullptr;
    }
    return mysql_use_result(_conn);
}

//获取数据库连接
MYSQL *Mysql::getConnection() { return _conn; }

