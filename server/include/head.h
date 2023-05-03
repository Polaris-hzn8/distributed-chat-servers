/*************************************************************************
	> File Name: head.h
	> Author: luochenhao
	> Mail: 3453851623@qq.com
	> Created Time: Sun 30 Apr 2023 02:47:16 PM CST
 ************************************************************************/

#ifndef _HEAD_H
#define _HEAD_H

#include <iostream>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>
#include <functional>
#include <unordered_map>
#include <string>
#include <memory>
#include "json.hpp"

using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

using json = nlohmann::json;

#endif
