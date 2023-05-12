/*************************************************************************
	> File Name: redis.h
	> Author: luochenhao
	> Mail: 3453851623@qq.com
	> Created Time: Thu 11 May 2023 05:20:09 PM CST
 ************************************************************************/

#ifndef _REDIS_H
#define _REDIS_H

#include "head.h"

class Redis {
public:
	Redis();
	~Redis();

	//连接redis服务器
	bool connect();
	//向redis指定的通道channel发布消息
	bool publish(int channel, string message);
	//向redis订阅subscribe指定的通道
	bool subscribe(int channel);
	//向redis取消订阅unsubscribe指定的通道
	bool unsubscribe(int channel);
	
	//在独立线程中接收订阅通道中的消息（接受消息）
	void observer_channel_message();

	//初始化 向业务层上报通道消息的回调对象（设置回调的公有方法）
	void init_notify_handler(function<void(int, string)> func);

private:
	//redisContext 相当于一个 redis-cli 存储了与客户端相关的所有信息
	//当向redis订阅消息之后 当前的上下文对象就会被阻塞 所以发布与订阅消息需要在不同的上下文对象中进行

	//heredis同步上下文对象 负责publish发布消息 
	redisContext *_publish_context;

	//hiredis同步上下文对象 负责subscribe消息
	redisContext *_subscribe_context;

	//回调操作 当订阅的channel收到消息后 会给service层上报该消息
	//需要得到通知 则必须先在redis消息队列中注册一个方法回调 当redis检测到时间发生则调用该方法
	function<void(int, string)> _notify_message_handler;// channel_id message_content
};

#endif

