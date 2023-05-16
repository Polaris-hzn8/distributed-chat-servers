/*************************************************************************
    > File Name: redis.cpp
    > Author: luochenhao
    > Mail: 3453851623@qq.com
    > Created Time: Thu 11 May 2023 17:20:45 CST
************************************************************************/

#include "redis.h"
#include "read.h"

Redis::Redis() : _publish_context(nullptr), _subscribe_context(nullptr) {}

Redis::~Redis() {
    if (_publish_context != nullptr) redisFree(_publish_context);
    if (_subscribe_context != nullptr) redisFree(_subscribe_context);
}

//连接redis服务器
bool Redis::connect() {
    //1.负责publish发布消息的上下文连接
    _publish_context = redisConnect(redisip, redisport);
    if (_publish_context == nullptr) {
        LOG_INFO << "connect redis failed : _publish_context";
        return false;
    }
    //2.负责subscribe订阅通道的上下文连接
    _subscribe_context = redisConnect(redisip, redisport);
    if (_subscribe_context == nullptr) {
        LOG_INFO << "connect redis failed : _subscribe_context";
        return false;
    }

    //3.创建单独的线程 在该线程中监听通道上的事件 有消息就给业务层进行上报
    // [&]() { observer_channel_message(); };
    thread thread_t( [&]() { observer_channel_message(); } );
    //设置分离线程 防止线程被join 线程执行完成内核中PCB资源可能会泄露 设置成分离线程 线程运行结束PCB自动回收
    thread_t.detach();//默认情况下线程都可以被join，detach可以让其不可join
    
    LOG_INFO << "[to redis] connect redis-server success!";
    return true;
}

//向redis指定的通道channel发布消息
bool Redis::publish(int channel, string message) {
    LOG_INFO << "[to redis] publish msg to redis-server in channel:" << channel;
    //发布消息
    redisReply *reply = (redisReply *)redisCommand(_publish_context, "PUBLISH %d %s", channel, message.c_str());
    if (reply == nullptr) {
        LOG_INFO << "publish command failed!";
        return false;
    }
    freeReplyObject(reply);
    return true;
}

/*
    此处有坑 subscribe & unsubscribe
    publish方法与subscribe方法的处理不同原因：使用redis同步发布订阅机制导致的问题（ redis还支持异步发布订阅的消息 libevent ）
    redisCommand 改为 redisAppendCommand & redisBufferWrite

    redisCommand做的事情：先将需要发送的命令缓存到本地 然后调用redisAppendCommand方法 最后调用redisBufferWrite将命令发送到redisServer上
    最后还会调用redisReply来以阻塞的方式等待命令的执行结果（导致出现问题）

    public方法可以使用redisComand方法的原因：public命令执行后马上就能够得到回复 不用客户端阻塞等待结果
    subscribe方法的调用会阻塞当前线程 所以不能直接调用redisCommand命令
*/

//向redis指定的通道subscribe订阅消息
bool Redis::subscribe(int channel) {
    LOG_INFO << "[to redis] subscribe redis mq channel:" << channel;
    //订阅消息 只订阅通道 不接受通道的消息 通道消息的接受专门会在observser_channel_message函数中的独立线程中进行
    //只负责发送命令 不阻塞接受redis server的响应消息 否则和notifyMsg线程抢占响应资源
    if (redisAppendCommand((this->_subscribe_context), "SUBSCRIBE %d", channel) == REDIS_ERR) {
        LOG_INFO << "subscribe command falied!";
        return false;
    }

    //redisBufferWrite可以循环发送缓冲区 直到缓冲区数据发送完毕为止
    int done = 0;
    while (!done) {
        if (redisBufferWrite(this->_subscribe_context, &done)) {
            LOG_INFO << "subscribe command failed!";
            return false;
        }
    }

    //redisGetReply
    return true;
}

//向redis指定的通道unsubscribe取消订阅消息
bool Redis::unsubscribe(int channel) {
    LOG_INFO << "[to redis] unsubscribe redis mq channel" << channel;
    //取消订阅消息
    //取消订阅消息 只取消订阅通道 不接受通道的消息
    //只负责发送命令 不阻塞接受redis server的响应消息 否则和notifyMsg线程抢占响应资源
    if (redisAppendCommand((this->_subscribe_context), "UNSUBSCRIBE %d", channel) == REDIS_ERR) {
        LOG_INFO << "unsubscribe command falied : redisAppendCommand.";
        return false;
    }

    //redisBufferWrite可以循环发送缓冲区 直到缓冲区数据发送完毕为止
    int done = 0;
    while (!done) {
        if (redisBufferWrite(this->_subscribe_context, &done)) {
            LOG_INFO << "unsubscribe command failed : redisBufferWrite.";
            return false;
        }
    }

    //redisGetReply
    return true;
}

//初始化向业务层上报通道消息的回调对象
void Redis::init_notify_handler(function<void(int, string)> func) {
    this->_notify_message_handler = func;//注册回调
}

//在独立线程中接收订阅通道中的消息
void Redis::observer_channel_message() {
    redisReply *reply = nullptr;
    /* 在 _subscribe_context上以循环阻塞的方式 等待上下文是否有消息发生 */
    while (redisGetReply(this->_subscribe_context, (void **)&reply)) {
        /* 通道上如果有消息依次会返回三个数据 相当于写在了element数组中 下标分别是 0 1 2 */
        /* 
           1) message 
           2) "13" 
           3) "hello world"
           element[1] = channel_id
           element[2] = helloworld
         */
        if (reply != nullptr && reply->element[2] != nullptr && reply->element[2]->str != nullptr) {
            //给业务层上报通道上发生的消息 参数为通道号channel_id 与 通道消息message
            _notify_message_handler(atoi(reply->element[1]->str), reply->element[2]->str);
        }
        freeReplyObject(reply);
    }
    LOG_INFO << "[to redis] observer_channerl_message quit";
}





