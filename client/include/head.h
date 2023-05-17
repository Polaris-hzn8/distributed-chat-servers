/*************************************************************************
	> File Name: head.h
	> Author: luochenhao
	> Mail: 3453851623@qq.com
	> Created Time: Sun 30 Apr 2023 02:47:16 PM CST
 ************************************************************************/

#ifndef _HEAD_H
#define _HEAD_H

#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <ctime>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <semaphore.h>
#include <atomic>//C++11原子类型 

#include "json.hpp"

using namespace std;
using json = nlohmann::json;

#endif
